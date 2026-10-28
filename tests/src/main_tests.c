#include "test_basics.h"
#include "test_coalesce_next_block.h"
#include "test_coalesce_prev_block.h"
#include "test_free_single_allocation.h"
#include "test_single_allocation.h"
#include <stdio.h>

int main(void) {
    printf("\n");
    printf("---- test malloc basic ----\n");
    test_malloc_basic();

    printf("\n");
    printf("---- test single allocation ----\n");
    test_single_allocation();

    printf("\n");
    printf("---- test free single allocation ----\n");
    test_free_single_allocation();

    printf("\n");
    printf("---- test coalesce previous block ----\n");
    test_coalesce_prev_block();

    printf("\n");
    printf("---- test coalesce next block ----\n");
    test_coalesce_next_block();
    return 0;
}
