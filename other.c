// #include "stdio.h"
//
// int main(void) {
//   int x = 42;
//   int y = x;
//
//   printf("x = %d\n", x);
//   printf("y = %d\n", y);
//
//   printf("pointer_x: %p\n", (void *)&x);
//   printf("pointer_y: %p\n", (void *)&y);
//
//   printf("sizeof_x = %zu\n", sizeof(x));
//   printf("sizeof_&x = %zu\n", sizeof(&x));
//
//   return 0;
// }
//
// #include "stdio.h"
//
// int main(void) {
//   char a = 'A';
//   char b = 'B';
//   char c = 'C';
//
//   printf("&a = %p\n", (void *)(&a));
//   printf("&b = %p\n", (void *)(&b));
//   printf("&c = %p\n", (void *)(&c));
//
//   printf("sizeof_char = %zu\n", sizeof(char));
//
//   return 0;
// }
//

// // int main(void) {
//   int x = 42;
//   int *p = &x;
//
//   printf("x  = %d\n", x);
//   printf("&x = %p\n", (void *)(&x));
//   printf("p  = %p\n", (void *)(p));
//   printf("*p = %d\n", (*p));
//   printf("&p = %p\n", (void *)(&p));
//
//   *p = 100;
//   printf("x = %d\n", x);
//   printf("*p = %d\n", (*p));
//
//   return 0;
// }

// int main(void) {
//   char arr[5] = {'a', 'b', 'c', 'd'};
//
//   printf("arr        = %p\n", (void *)arr);
//   printf("&arr[0]    = %p\n", (void *)&arr[0]);
//   printf("&arr[1]    = %p\n", (void *)&arr[1]);
//   printf("arr[0]     = %c\n", arr[0]);
//   printf("arr[2]     = %c\n", arr[2]);
//   printf("*(arr + 2) = %c\n", *(arr + 2));
//
//   return 0;
// }

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int *a = malloc(sizeof(int));
  int *b = malloc(sizeof(int));

  if (a == NULL || b == NULL) {
    return 1;
  }

  printf("a: %p\n", (void *)a);
  printf("b: %p\n", (void *)b);

  *a = 45;
  *b = 99;

  printf("value_a: %d\n", *a);
  printf("value_b: %d\n", *b);

  free(a);
  free(b);

  return 0;
}
