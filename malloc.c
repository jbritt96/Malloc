#include <stdio.h>
#include <string.h>

typedef struct {
int size;
int magic;
} header_t;

typedef struct __node_t {
int size;
struct __node_t *next;
} node_t;


int main() {
   header_t head;

   head.size = 100;
   head.magic = 1234567;

   printf("%d \n", head.size);
   printf("%d \n", head.magic);

   node_t node;
   node.size = 4096;
   
   printf("%d\n", node.size);
}

