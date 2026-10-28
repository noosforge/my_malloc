#include "coallease.h"
#include "helpers.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void *my_free(void *payload, void *arena) {
    if (payload == NULL) {
        return NULL;
    }

    unsigned char *arena_start = (unsigned char *)arena;
    unsigned char *arena_end = (unsigned char *)arena + (size_t)1024;

    // Header - 8bytes before payload
    unsigned char *header_bytes = (unsigned char *)payload - (size_t)8;
    struct BlockHeader *header = (struct BlockHeader *)header_bytes;

    size_t header_original_size = get_original_size(header);

    // Header Footer
    unsigned char *header_footer_bytes =
        header_bytes + (header_original_size - sizeof(struct BlockFooter));
    struct BlockFooter *header_footer = (struct BlockFooter *)header_footer_bytes;

    // free the header, its allocated
    header->size_and_flags &= ~(size_t)1;

    unsigned char *curr_back_header = header_bytes; // used to track the most left free header
    struct BlockHeader *back_header = NULL;

    while (curr_back_header > arena_start) {
        // BACK Footer
        unsigned char *back_footer_bytes = curr_back_header - (size_t)8;
        struct BlockFooter *back_footer = (struct BlockFooter *)back_footer_bytes;

        // Back Header
        // to get to the header, move footer_start_bytes back.
        unsigned char *back_header_bytes =
            back_footer_bytes - (back_footer->size - sizeof(struct BlockFooter));

        struct BlockHeader *prev_back_header = (struct BlockHeader *)back_header_bytes;

        size_t is_allocated = allocated(prev_back_header);
        if (is_allocated == 1) { // It's Not Free
            break;
        }

        back_header = prev_back_header;
        curr_back_header = (unsigned char *)back_header;
    }

    // header is already freed. -It contains the original size.
    unsigned char *curr_front_header = header_bytes + header->size_and_flags;
    struct BlockHeader *front_header = NULL;
    struct BlockFooter *front_footer = NULL;
    while (curr_front_header <= (arena_end - (size_t)32)) {
        // Front Header
        struct BlockHeader *next_front_header = (struct BlockHeader *)curr_front_header;

        size_t is_allocated = allocated(next_front_header);
        if (is_allocated) { // It's not Free
            break;
        }

        front_header = next_front_header;

        // get the origial front_header_size, it could be allocated
        size_t front_header_original_size = get_original_size(front_header);

        curr_front_header =
            (unsigned char *)front_header +
            front_header_original_size; // move the current_header for the next iteration

        // Front Footer
        unsigned char *front_footer_bytes =
            (unsigned char *)front_header +
            (front_header_original_size - sizeof(struct BlockFooter));
        front_footer = (struct BlockFooter *)front_footer_bytes;
    }

    // coallease
    coallease(back_header, header_footer);
    coallease(header, front_footer);

    return back_header;
}
