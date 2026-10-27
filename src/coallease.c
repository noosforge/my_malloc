#include "helpers.h"

int coallease(struct BlockHeader *header, struct BlockFooter *footer) {
    if (header == NULL || footer == NULL) {
        return 1;
    }

    unsigned char *header_bytes = (unsigned char *)header;
    unsigned char *footer_bytes = (unsigned char *)footer;
    size_t new_size = footer_bytes - header_bytes;
    size_t aligned_size = check_alignment(new_size);

    header->size_and_flags = aligned_size;
    footer->size = aligned_size;

    return 0;
}
