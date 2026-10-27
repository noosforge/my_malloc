#ifndef HELPERS
#define HELPERS

#include <stdio.h>

struct BlockHeader {
    size_t size_and_flags;
};

struct BlockFooter {
    size_t size;
};

size_t check_alignment(size_t request);

size_t get_original_size(struct BlockHeader *curr_header);

size_t allocated(struct BlockHeader *curr_header);

#endif // !HELPERS
