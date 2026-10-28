#include "create_arena.h"
#include "helpers.h"
#include "my_free.h"
#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>

void check_first_header(struct BlockHeader *first_header, int *failures) {
    printf("\n");
    printf("--check first header\n");
#define CHECK(condition, message)                                                                  \
    do {                                                                                           \
        if (condition) {                                                                           \
            printf("PASS: %s\n", message);                                                         \
        } else {                                                                                   \
            printf("FAIL: %s\n", message);                                                         \
            (*failures)++;                                                                         \
        }                                                                                          \
    } while (0);

    CHECK(allocated(first_header) == 0, "first header is free");
    CHECK(get_original_size(first_header) == 1024,
          "after coalescing first and second header, total_size = 1024");

#undef CHECK
    printf("Results: %s, (%d)failures\n", failures == 0 ? "PASS" : "FAIL", *failures);
}

void check_second_header(struct BlockHeader *second_header, int *failures) {
    printf("\n");
    printf("--check second header\n");
#define CHECK(condition, message)                                                                  \
    do {                                                                                           \
        if (condition) {                                                                           \
            printf("PASS: %s\n", message);                                                         \
        } else {                                                                                   \
            printf("FAIL: %s\n", message);                                                         \
            (*failures)++;                                                                         \
        }                                                                                          \
    } while (0);

    CHECK(allocated(second_header) == 0, "second header is free");
    CHECK(get_original_size(second_header) == 984,
          "after coalescing second and rest of arena, total_size = 984");

#undef CHECK
    printf("Results: %s, (%d)failures\n", failures == 0 ? "PASS" : "FAIL", *failures);
}

void test_coalesce_next_block(void) {
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

    unsigned char *payload_bytes2 = (unsigned char *)payload2;
    struct BlockHeader *second_header =
        (struct BlockHeader *)(payload_bytes2 - sizeof(struct BlockHeader));

    int failures = 0;

    my_free(payload2, arena);

    check_second_header(second_header, &failures);

    my_free(payload1, arena);

    check_first_header(first_header, &failures);

    free(arena);
}
