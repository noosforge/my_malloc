#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  size_t bytes = 1024;
  void *arena = malloc(bytes);

  if (arena == NULL) {
    return 1;
  }

  struct BlockHeader {
    size_t size_and_flags;
  };

  printf("BlockHeader size: %zu\n", sizeof(struct BlockHeader));

  struct BlockHeader header = {.size_and_flags = bytes};
  size_t original_size = header.size_and_flags & ~1;
  int allocated = header.size_and_flags & 1;

  printf("original_size: %zu\n", original_size);
  printf("allocated: %d\n", allocated);

  free(arena);

  return 0;
}
