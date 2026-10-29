#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void *create_arena(void) {
    void *arena = malloc(TOTAL_ARENA_BYTES);

    if (arena == NULL) {
        return NULL;
    }

    // Header
    struct BlockHeader *header = (struct BlockHeader *)(arena);
    header->size_and_flags = TOTAL_ARENA_BYTES;

    // PAYLOAD
    size_t payload = header->size_and_flags - (BLOCK_HEADER_SIZE + BLOCK_FOOTER_SIZE);

    // FOOTER
    unsigned char *arena_start_bytes = (unsigned char *)arena;
    arena_start_bytes += (TOTAL_ARENA_BYTES - BLOCK_FOOTER_SIZE);
    size_t original_size = get_original_size(header);
    struct BlockFooter *footer_ptr = (struct BlockFooter *)arena_start_bytes;
    footer_ptr->size = original_size;

    printf("Bozy size: %zu\n", payload);
    printf("BlockFooter size: %zu\n", BLOCK_FOOTER_SIZE);
    printf("Size in footer: %zu\n", footer_ptr->size);

    // RULES
    size_t min_block_size = BLOCK_HEADER_SIZE + (size_t)16 + BLOCK_FOOTER_SIZE;
    if (original_size != footer_ptr->size || original_size < min_block_size || payload < 16) {
        free(arena);
        return NULL;
    }

    printf("Arena size: %zu\n", header->size_and_flags);

    return arena;
}
