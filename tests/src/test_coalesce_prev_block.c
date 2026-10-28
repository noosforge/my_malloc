#include "create_arena.h"
#include "helpers.h"
#include "my_free.h"
#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>

void test_coalesce_prev_block(void) {
    void *arena = create_arena();

    // allocate first 17 bytes
    size_t request1 = 17;
    void *payload1 = my_malloc(request1, arena);

    unsigned char *payload_bytes1 = (unsigned char *)payload1;
    struct BlockHeader *first_header =
        (struct BlockHeader *)(payload_bytes1 - sizeof(struct BlockHeader));

    // allocate second 17 bytes
    size_t request2 = 17;
    void *payload2 = my_malloc(request2, arena);

    my_free(payload1, arena);
    my_free(payload2, arena);

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
    CHECK(get_original_size(first_header) == 80,
          "after coalescing first and second header, total_size = 80");

#undef CHECK

    printf("Results: %s, (%d)failures\n", failures == 0 ? "PASS" : "FAIL", failures);
    free(arena);
}
