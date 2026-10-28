#include "create_arena.h"
#include "helpers.h"
#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>

int test_malloc_basic(void) {
    void *arena = create_arena();

    if (arena == NULL) {
        printf("Failed to create the arena\n");
        return 1;
    }

    size_t request = 17;
    void *payload = my_malloc(request, arena);

    void *payload2 = my_malloc(17, arena);
    printf("First payload:  %p\n", payload);
    printf("Second payload: %p\n", payload2);
    if (payload == NULL) {
        printf("Failed to allocate the request\n");
    }

    unsigned char *base = (unsigned char *)arena;
    struct BlockHeader *first_header = (struct BlockHeader *)base;
    struct BlockFooter *first_footer = (struct BlockFooter *)(base + 32);

    struct BlockHeader *second_header = (struct BlockHeader *)(base + 40);
    struct BlockFooter *last_footer = (struct BlockFooter *)(base + 1016);

    int failures = 0;

#define CHECK(condition, message)                                                                  \
    do {                                                                                           \
        if (condition) {                                                                           \
            printf("PASS: %s\n", message);                                                         \
        } else {                                                                                   \
            printf("FAIL: %s\n", message);                                                         \
            failures++;                                                                            \
        }                                                                                          \
    } while (0);

    CHECK(payload == base + 8, "payload starts 8 bytes after the arena");
    CHECK(get_original_size(first_header) == 40, "allocated block size is 40");
    CHECK(allocated(first_header) == 1, "first block is allocated");
    CHECK(first_footer->size == 40, "allocated first footer size is 40");

    CHECK(payload2 == base + 48, "payload2 starts 48 bytes after the arena");

    CHECK((unsigned char *)second_header == base + 40,
          "second header block starts 40 bytes after the arena");
    CHECK(get_original_size(second_header) == 40, "second block size is 40");
    CHECK(allocated(second_header) == 1, "second block is free");
    CHECK(last_footer->size == 944, "second footer size is 984");

#undef CHECK

    printf("Results: %s, (%d)failures\n", failures == 0 ? "PASS" : "FAIL", failures);
    free(arena);
    return failures;
}
