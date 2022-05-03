# Malloc & Free

## Running
To run the program containing ```myMalloc()``` and ```myFree()```, simply run the provided makefile.

## Testing
To allocate memory with ```myMalloc()```, open ```malloc.c```, and in main, create a void pointer, and initialize it to the return value of ```myMalloc(size)```, where size is an integer less than or equal to 4080. Anything larger will cause the function to print an error message and return NULL, and the program will continue.

To free memory, you can run ```myFree(chunk)``` where ```chunk``` is a memory block previously returned by an instance of ```myMalloc()```.

You can use ```printHeap()``` to see the status of the memory heap and whether individual blocks of memory are allocated or free, along with their addresses, size, and ```next``` pointers.

##### Example
```
int main() {
   void* chunk7 = myMalloc(4080);
   void* chunk1 = myMalloc(300);
   void* chunk2 = myMalloc(120);
   void* chunk3 = myMalloc(400);

   printHeap();

   myFree(chunk1);
   myFree(chunk2);
   myFree(chunk3);

   printHeap();

   return 0;
}
```
