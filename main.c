#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BlockHeader {
    size_t size_and_flags;
};

struct BlockFooter {
    size_t size;
};

size_t check_alignment(size_t request) {
    size_t aligned = ((request + 7) / 8) * 8;
    return aligned;
}

size_t get_original_size(struct BlockHeader *curr_header) {
    return curr_header->size_and_flags & ~(size_t)1;
}
size_t allocated(struct BlockHeader *curr_header) {
    return curr_header->size_and_flags & (size_t)1;
}

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

// We'll implement this after allocation works
void *my_free();

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

int test_malloc_basic(void) {
    printf("---- test malloc basic ---- ");
    void *arena = create_arena();

    if (arena == NULL) {
        printf("Failed to create the arena\n");
        return 1;
    }

    size_t request = 17;
    void *payload = my_malloc(request, arena);

    if (payload == NULL) {
        printf("Failed to allocate the request\n");
    }

    unsigned char *base = (unsigned char *)arena;
    struct BlockHeader *first_header = (struct BlockHeader *)base;
    struct BlockFooter *first_footer = (struct BlockFooter *)(base + 32);

    struct BlockHeader *second_header = (struct BlockHeader *)(base + 40);
    struct BlockFooter *second_footer = (struct BlockFooter *)(base + 1016);

    int failures = 0;

#define CHECK(condition, message)                                                                  \
    do {                                                                                           \
        if (condition) {                                                                           \
            printf("PASS: %s\n", message);                                                         \
        } else {                                                                                   \
            printf("Failed: %s\n", message);                                                       \
            failures++;                                                                            \
        }                                                                                          \
    } while (0);

    CHECK(payload == base + 8, "payload starts 8 bytes after the arena");
    CHECK(get_original_size(first_header) == 40, "allocated block size is 40");
    CHECK(allocated(first_header) == 1, "first block is allocated");
    CHECK(first_footer->size == 40, "allocated first footer size is 40");

    CHECK((unsigned char *)second_header == base + 40,
          "remaining block starts 40 bytes after the arena");
    CHECK(get_original_size(second_header) == 984, "remaining second block size is 984");
    CHECK(allocated(second_header) == 0, "remaining block is free");
    CHECK(second_footer->size == 984, "remaining footer size is 984");

#undef CHECK

    printf("Results: %s, (%d)failures\n", failures == 0 ? "PASS" : "FAIL", failures);
    free(arena);
    return failures;
}

int main(void) { return test_malloc_basic() != 0; }
