#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <inttypes.h>

int MAGIC_NUM = 1234567;

typedef struct {
int size;
int magic;
} header_t;

typedef struct __node_t {
int size;
struct __node_t *next;
} node_t;

bool isInitialized = false;
node_t *head = NULL;

void *myMalloc(size_t size){
   if (isInitialized == false) {
      // mmap() returns a pointer to a chunk of free space
      head = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
      head->size = 4096 - sizeof(node_t);
      head->next = NULL;
      isInitialized = true;
   }
   // round to wordsize
   size += 7;
   //adding header to size
   size += 16;
   int remainder = size % 8;
   size -= remainder;

   node_t *start = head;

   //search for best fit
   node_t *smallest = start;
   while(start->next != NULL){
      if(smallest->size < start->size && smallest->size >= size){
         smallest = start;
      }
      start = start->next;

   }
    //create new node to put in linked list

  node_t *chopped = (node_t *) (((void *) smallest) + size);
  chopped->size = (int) size;
  chopped->next = smallest->next;
  smallest->next = chopped;

   //create memory chunk and return it
   header_t *memChunk = (header_t *) ((void *) smallest);
   memChunk->size = (int) size;
   memChunk->magic = MAGIC_NUM;

   /*return chopped;*/
   // printf("%d\n", 5);
   printf("%p\n", (void *) &memChunk);
   return memChunk;

}

void myFree(void *ptr){

}

void printHeap(){
   //printf("%p\n", (void *) &head);
   // while(head != NULL){

   // }
}

int main() {

   myMalloc(300);
   //printHeap();


}
