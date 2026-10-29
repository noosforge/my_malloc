#include "my_malloc.h"
#include "helpers.h"
#include <stddef.h>
#include <stdio.h>

void *get_curr_header_bytes(const struct BlockHeader *curr_header) {
    return (unsigned char *)curr_header;
}

void *get_curr_payload_bytes(const struct BlockHeader *curr_header) {
    unsigned char *curr_header_bytes = (unsigned char *)curr_header;
    return curr_header_bytes + BLOCK_HEADER_SIZE;
}

void *move_curr_header(const void *arena, struct BlockHeader *curr_header) {
    const size_t curr_header_original_size = get_original_size(curr_header);
    unsigned char *curr_header_bytes = get_curr_header_bytes(curr_header);
    unsigned char *next_header = curr_header_bytes + curr_header_original_size;

    size_t next_offset = next_header - (unsigned char *)arena;
    if (next_offset >= TOTAL_ARENA_BYTES) {
        return NULL;
    }
    return next_header;
}

NewFooterResult create_new_footer(struct BlockHeader *curr_header, const size_t aligned) {
    const size_t original_size = get_original_size(curr_header);
    unsigned char *curr_header_bytes = get_curr_header_bytes(curr_header);
    // header + header_size + new_alignment
    unsigned char *new_footer_pos = curr_header_bytes + BLOCK_HEADER_SIZE + aligned;
    const size_t new_size = (size_t)((new_footer_pos + BLOCK_FOOTER_SIZE) - (curr_header_bytes));

    size_t remaining_size = original_size - new_size;
    if (remaining_size < (size_t)32) {
        return (NewFooterResult){
            .position = NULL, .new_size = original_size, .remaining_size = remaining_size};
    }

    return (NewFooterResult){
        .position = new_footer_pos, .new_size = new_size, .remaining_size = remaining_size};
}

// Marks the curr_header as allocated. Sets the new_size
void update_curr_header(struct BlockHeader *curr_header, const size_t new_size) {
    allocate_header(curr_header);
    curr_header->size_and_flags = new_size;
}

NewHeaderResult create_new_header(void *arena, struct BlockHeader *curr_header,
                                  const size_t prev_header_new_size) {
    unsigned char *curr_header_bytes = get_curr_header_bytes(curr_header);

    // new_size = prev_header to prev_footer
    // new header is immediately after the prev_footer. - They share the same boundary
    unsigned char *new_header_pos = (curr_header_bytes + prev_header_new_size);
    size_t new_header_offset = new_header_pos - (unsigned char *)arena;

    // guard to prevent overflow, to our local arena
    if (new_header_offset > TOTAL_ARENA_BYTES) {
        return (NewHeaderResult){.position = NULL};
    }

    return (NewHeaderResult){.position = new_header_pos};
}

void update_curr_footer(struct BlockFooter *curr_footer, const size_t new_size) {
    // new_size is the remaining size
    curr_footer->size = new_size;
}

void *my_malloc(size_t request, void *arena) {
    struct BlockHeader *header = (struct BlockHeader *)arena;
    struct BlockHeader *curr_header = header;

    size_t offset = (unsigned char *)curr_header - (unsigned char *)arena;

    while (offset < TOTAL_ARENA_BYTES) {
        size_t is_allocated = allocated(curr_header);
        size_t original_size = get_original_size(curr_header);
        size_t aligned = check_alignment(request);

        // Curr header bytes
        unsigned char *curr_header_bytes = get_curr_header_bytes(curr_header);

        // Curr Payload
        unsigned char *curr_payload = get_curr_payload_bytes(curr_header);
        size_t curr_payload_size = original_size - (BLOCK_HEADER_SIZE + BLOCK_FOOTER_SIZE);

        // Curr Footer
        unsigned char *curr_footer_bytes = (curr_header_bytes + original_size) - BLOCK_FOOTER_SIZE;
        struct BlockFooter *curr_footer = (struct BlockFooter *)curr_footer_bytes;

        // guard
        if (is_allocated == 1 || request > curr_payload_size || aligned > curr_payload_size) {
            unsigned char *next_header = move_curr_header(arena, curr_header);
            if (next_header == NULL) {
                return NULL;
            }
            // updates for the next iteration
            curr_header = (struct BlockHeader *)next_header;
            offset = (unsigned char *)curr_header - (unsigned char *)arena;
            continue;
        }

        /* Splitting
         * before = [CurrHeader, CurrFooter]
         * after => [CurrHeader, NewFooter] [NewHeader, CurrFooter]
         * */

        // New Footer
        NewFooterResult new_footer_res = create_new_footer(curr_header, aligned);
        if (new_footer_res.position == NULL) {
            allocate_header(curr_header); // allocate the header
            return curr_payload;
        } else {
            struct BlockFooter *new_footer = (struct BlockFooter *)new_footer_res.position;
            new_footer->size = new_footer_res.new_size;
        }

        // New Header
        NewHeaderResult new_header_res =
            create_new_header(arena, curr_header, new_footer_res.new_size);
        if (new_header_res.position == NULL) {
            allocate_header(curr_header); // allocate the header
            return curr_payload;
        } else {
            struct BlockHeader *new_header = (struct BlockHeader *)new_header_res.position;
            new_header->size_and_flags = new_footer_res.remaining_size;
        }

        update_curr_header(curr_header, new_footer_res.new_size); // size and allocation status

        update_curr_footer(curr_footer, new_footer_res.remaining_size); // size status

        // allocate header and return payload
        allocate_header(curr_header);
        return curr_payload;
    }

    // Nothing found!
    return NULL;
}
