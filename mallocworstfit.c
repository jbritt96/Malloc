#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <inttypes.h>

int MAGIC_NUM = 1234567;
size_t TOTAL_HEAP_SIZE = 4096;

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
      head->size = TOTAL_HEAP_SIZE;
      head->next = NULL;
      isInitialized = true;
   }

   // round to wordsize
   size += 7;
   //adding header to size
   size += 16;
   size_t remainder = size % 8;
   size -= remainder;

   //search for worst fit
   node_t *start = head;
   node_t *largest = head;

   while(start->next != NULL){
      if(start->size >= largest->size){
        largest = start;
      }
      start = start->next;
   }

   // Check that the selected block is the right size.
   if(start->size < size){
     printf("%s\n\n", "No chunk big enough to return");
     return NULL;
   }

   // create new node to put in linked list
   node_t *chopped = (node_t *) ((void *) start);
   size_t sizeOfChunkBeforeSplit = start->size;
   //printf("%lu: size of chunk before split\n", sizeOfChunkBeforeSplit);
   chopped->size = start->size - size;
   chopped->next = start->next;
   //start->next = chopped;
   //create memory chunk and return it
   header_t *memChunk = (header_t *) (((void *) start) + start->size);
   memChunk->size = (size_t) size;
   memChunk->magic = MAGIC_NUM;

   /*printf("%lu: size of the first  half of the split block\n", chopped->size);
   printf("%lu: size of the second half of the split block\n", memChunk->size);

   // printf("%d Magic number in malloc\n", memChunk->magic);
   printf("%p Address of first half\n", (void *) chopped);
   printf("%p Address of memChunk\n", (void *) memChunk);*/

   //return the 2nd half of the split memory chunk;
   return memChunk + 1;
}

void myFree(void *ptr){
   size_t pointerSize = sizeof(void *);
   //printf("%lu\n", pointerSize);
   header_t *allocatedBlock = ((header_t *) ptr - 1);
   // header_t *allocatedBlock = ((header_t *) ptr);
   //printf("%p\n", (void *) allocatedBlock);
   //printf("%d\n", allocatedBlock->magic);
   if(allocatedBlock->magic != MAGIC_NUM){
      printf("%s\n", "Doesn't have magic num");
      return;
   }


   node_t *freeBlock = (node_t *) allocatedBlock;
   // freeBlock's next field hasn't been set yet

   //find where it goes on list

   // we need a pointer to the previous node's 'next' member
   node_t **prevNextPtr = &head;

   // find where the freeBlock belongs
   // if it doesn't go within the list put it at the end
   while((*prevNextPtr) && ((*prevNextPtr)->next > freeBlock)){
     prevNextPtr = &((*prevNextPtr)->next);
   }

   // have the free block point to what the prev block was pointing to before

   // now have the prev block point to the free block

   node_t *chopped = (node_t *) ((void *) freeBlock + freeBlock->size);
   /*printf("%p\n", chopped);
   printf("%p Freeblock->next\n",(void *)((char*) freeBlock->next));
   printf("%p Freeblock address\n", (void *) freeBlock);
   printf("%p Freeblock address + size\n", (void *) freeBlock + freeBlock->size);*/
   if((void *)((char*) freeBlock + freeBlock->size) == (void *)((char*)freeBlock->next)){
     //coalesce block
     freeBlock->next = chopped;
     chopped->next = (*prevNextPtr)->next;
     (*prevNextPtr)->next = freeBlock;
   } else {
      freeBlock->next = (*prevNextPtr)->next;
      (*prevNextPtr)->next = freeBlock;
   }


   size_t totalBytes = 0;
   node_t *start = head;
   node_t *nextChunkCoalesce;
   while(totalBytes < TOTAL_HEAP_SIZE && nextChunkCoalesce->next != NULL){
      //typecast to see if is header_t/allocated block
      header_t *startHeader_t = (header_t *) start;

      //determine if its allocated block
      if(startHeader_t->magic == MAGIC_NUM){
         totalBytes += startHeader_t->size;
      } 
      //Must be a free block on the list
      else {
         void *nextChunkCoalesceAddress = (void *)((char*)start + start->size);
         header_t *nextChunkCoalesceHeader = (header_t *) nextChunkCoalesceAddress;
         //next chunk is an free block
         if(nextChunkCoalesceHeader->magic != MAGIC_NUM){
            //combine sizes of the free blocks
            nextChunkCoalesce = (node_t *) nextChunkCoalesceAddress;
            start->size += (nextChunkCoalesce->size);
            start->next = nextChunkCoalesce->next;
            
         }

         totalBytes += start->size;
      }
      //calculate address of next block
      void *nextChunk = (void*)((char*)start + start->size);
      start = nextChunk;
   }

}



void printHeap(){
   printf("%s\n\n", "***Printing heap***");
   size_t totalBytes = 0;
   node_t *start = head;

   while(totalBytes < TOTAL_HEAP_SIZE){
      //typecast to see if is header_t/allocated block
      header_t *startHeader_t = (header_t *) start;

      //determine if its allocated block
      if(startHeader_t->magic == MAGIC_NUM){
         printf("%s\n", "This block is an allocated block");
         printf("Pointer address is: %p\n", startHeader_t);
         printf("The size is: %lu\n\n", startHeader_t->size);

         totalBytes += startHeader_t->size;
      } 
      //Must be a free block on the list
      else {
         printf("%s\n", "This block is a free block");
         printf("Pointer address is: %p\n", start);
         printf("This block is pointing to: %p\n", start->next);
         printf("The size is: %lu\n\n", start->size);
      
         totalBytes += start->size;
      }
      //calculate address of next block
      void *nextChunk = (void*)((char*)start + start->size);
      start = nextChunk;
   }

   printf("total bytes accounted for: %lu\n\n", totalBytes);
   return;
}

int main() {

   //void* chunk7 = myMalloc(5000);
   void* chunk = myMalloc(300);
   void* chunk2 = myMalloc(120);
   void* chunk3 = myMalloc(400);
   void* chunk4 = myMalloc(500);
   void* chunk5 = myMalloc(600);
   void* chunk6 = myMalloc(5000);
   printHeap();

   //myFree(chunk);
   
   
   //myFree(chunk3);
   myFree(chunk2);
   //myFree(chunk5);
   myFree(chunk4);

   printHeap();

   return 0;
}
