#include "helpers.h"
#include <stddef.h>

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
