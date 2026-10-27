#include "create_arena.h"
#include "my_malloc.h"
#include <stdio.h>

int main(void) {
    void *arena = create_arena();
    size_t request = 34;

    void *payload = my_malloc(request, arena);
    printf("payload: %p\n", (void *)payload);
}
