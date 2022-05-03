#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <inttypes.h>

int MAGIC_NUM = 1234567;

typedef struct {
size_t size;
int magic;
} header_t;

typedef struct __node_t {
size_t size;
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
   size_t remainder = size % 8;
   size -= remainder;

   node_t *start = head;

   //search for best fit
   node_t *smallest = start;
   while(start->next != NULL){
      if(smallest->size < start->size && smallest->size >= size){
         smallest = start;
      }
      *start = *start->next;

   }
    //create new node to put in linked list

   node_t *chopped = (node_t *) (((void *) smallest) + size);
   chopped->size = (size_t) size;
   chopped->next = smallest->next;
   smallest->next = chopped;


   //create memory chunk and return it
   header_t *memChunk = (header_t *) ((void *) smallest);
   memChunk->size = (size_t) size;
   memChunk->magic = MAGIC_NUM;

   /*return the 2nd half of the split memory chunk;*/
   //printf("%p\n", (void *) &memChunk);
   return memChunk;
}

void myFree(void *ptr){
   size_t pointerSize = sizeof(void *);
   header_t *getMagicNum = (header_t *) ptr;
   if(getMagicNum->magic != MAGIC_NUM){
      printf("%s\n", "Doesn't have magic num");
      return;
   }

   header_t *getSize = (header_t *) ptr;
   //printf("%lu\n", getSize->size);
   //find where it goes on list
   node_t *start = head;


   // node_t **prev = &head;
   // for (node_t *current = head; current; prev = &current->next, current = current-> next)

   //printf("%p\n", );
   while(start != NULL){
      //if the address of the current node, minus the size offset, is the same as the address of the passed-in pointer:
      //if ( ( (*start) - pointerSize) == (size_t) ptr) {
      //  printf("%p\n", (void *) start->next);
      //}
      // printf("%p\n", (void *) start->next);

      
      start = start->next;
      
   }
   //coalesce block


}

/*void printHeap(){
   //printf("%p\n", (void *) &head);
   // while(head != NULL){

   // }
}*/

int main() {

   header_t *chunk = myMalloc(300);
   
   myFree(chunk);
   //printHeap();
   return 0;
}
