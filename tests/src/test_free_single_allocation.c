#include "create_arena.h"
#include "helpers.h"
#include "my_free.h"
#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>

void test_free_single_allocation(void) {
    void *arena = create_arena();

    // allocate 17 bytes
    size_t request = 17;
    void *payload = my_malloc(request, arena);

    unsigned char *payload_bytes = (unsigned char *)payload;
    struct BlockHeader *first_header =
        (struct BlockHeader *)(payload_bytes - sizeof(struct BlockHeader));

    my_free(payload, arena);

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

    CHECK(allocated(first_header) == 0, "first header is free");

#undef CHECK

    printf("Results: %s, (%d)failures\n", failures == 0 ? "PASS" : "FAIL", failures);
    free(arena);
}
