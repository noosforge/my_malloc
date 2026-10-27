#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void *create_arena(void) {
    size_t bytes = 1024;
    void *arena = malloc(bytes);

    if (arena == NULL) {
        return NULL;
    }

    // Header
    struct BlockHeader *header_ptr = (struct BlockHeader *)(arena);
    header_ptr->size_and_flags = bytes;

    // BODY
    size_t header_size = sizeof(struct BlockHeader);
    size_t footer_size = sizeof(struct BlockFooter);

    // PAYLOAD
    size_t payload = header_ptr->size_and_flags - (header_size + footer_size);
    printf("Bozy size: %zu\n", payload);

    // FOOTER
    unsigned char *memory = (unsigned char *)arena;
    memory += (bytes - footer_size);
    size_t original_size = header_ptr->size_and_flags & ~(size_t)1;
    struct BlockFooter *footer_ptr = (struct BlockFooter *)memory;
    footer_ptr->size = original_size;

    printf("BlockFooter size: %zu\n", footer_size);
    printf("Size in footer: %zu\n", footer_ptr->size);

    // RULES
    size_t min_block_size = header_size + (size_t)16 + footer_size;
    if (original_size != footer_ptr->size || original_size < min_block_size || payload < 16) {
        free(arena);
        return NULL;
    }

    printf("Arena size: %zu\n", header_ptr->size_and_flags);

    return arena;
}
