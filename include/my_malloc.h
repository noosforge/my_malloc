#ifndef MY_MALLOC
#define MY_MALLOC

#include "helpers.h"
#include <stdio.h>

typedef struct {
    unsigned char *position;
    size_t new_size;
    size_t remaining_size;
} NewFooterResult;

typedef struct {
    unsigned char *position;
} NewHeaderResult;

void *get_curr_header_bytes(const struct BlockHeader *curr_header);

void *get_curr_payload_bytes(const struct BlockHeader *curr_header);

void *move_curr_header(const void *arena, struct BlockHeader *curr_header);

NewFooterResult create_new_footer(struct BlockHeader *curr_header, const size_t aligned);

void update_curr_header(struct BlockHeader *curr_header, const size_t new_size);

NewHeaderResult create_new_header(void *arena, struct BlockHeader *curr_header, const size_t new_size);

void update_curr_footer(struct BlockFooter *curr_footer, const size_t new_size);

void *my_malloc(size_t request, void *arena);

#endif // !MY_MALLOC
