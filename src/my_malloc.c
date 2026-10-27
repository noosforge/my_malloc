#include "helpers.h"
#include <stdio.h>

void *my_malloc(size_t request, void *arena) {
    struct BlockHeader *header = (struct BlockHeader *)arena;
    struct BlockHeader *curr_header = header;

    size_t total_bytes = 1024;
    size_t offset = (unsigned char *)curr_header - (unsigned char *)arena;

    while (offset < total_bytes) {
        size_t is_allocated = allocated(curr_header);
        size_t original_size = get_original_size(curr_header);
        size_t aligned = check_alignment(request);

        // Curr Header
        size_t curr_header_size = sizeof(struct BlockHeader);
        unsigned char *curr_header_bytes = (unsigned char *)curr_header;

        // Next header -for the next iteration
        unsigned char *next_header = curr_header_bytes + original_size;
        size_t next_offset = next_header - (unsigned char *)arena;

        // Curr Footer
        size_t footer_size = sizeof(struct BlockFooter);
        unsigned char *curr_footer_pos = (curr_header_bytes + original_size) - footer_size;
        struct BlockFooter *curr_footer = (struct BlockFooter *)curr_footer_pos;

        // Curr Payload
        size_t curr_payload_size = original_size - (curr_header_size + footer_size);
        unsigned char *curr_payload = curr_header_bytes + curr_header_size;

        // guard
        if (is_allocated == 1 || request > curr_payload_size || aligned > curr_payload_size) {
            if (next_offset >= total_bytes) {
                return NULL;
            }
            curr_header = (struct BlockHeader *)next_header;
            offset = (unsigned char *)curr_header - (unsigned char *)arena;
            continue;
        }

        // Got the block -create/update CurrHeader, NewFooter, NextHeader, CurrFooter
        // NewFooter
        unsigned char *new_footer_pos =
            curr_header_bytes + curr_header_size + aligned; // header + new_aligment
        size_t new_footer_size = (size_t)((new_footer_pos + footer_size) - (curr_header_bytes));

        size_t new_size = original_size - new_footer_size;
        if (new_size < (size_t)32) {
            curr_header->size_and_flags |= (size_t)1;
            return curr_payload;
        }

        struct BlockFooter *new_footer = (struct BlockFooter *)new_footer_pos;
        new_footer->size = new_footer_size;

        // CurrHeader
        curr_header->size_and_flags = new_footer->size;
        curr_header->size_and_flags |= (size_t)1;

        // NextHeader
        unsigned char *next_header_pos =
            (curr_header_bytes + new_footer->size); // immediately after footer.
        size_t next_header_offset = next_header_pos - (unsigned char *)arena;
        if (next_header_offset > total_bytes) { // guard to prevent overflow, to our local arena
            return curr_payload;
        }

        struct BlockHeader *new_header = (struct BlockHeader *)next_header_pos;
        new_header->size_and_flags = new_size;

        // CurrFooter
        curr_footer->size = new_size;

        // return payload
        return curr_payload;
    }

    // Nothing found!
    return NULL;
}
