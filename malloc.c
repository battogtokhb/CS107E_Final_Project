/*
 * File: malloc.c
 * --------------
 * This is the simple "bump" allocator from lecture.
 * An allocation request is serviced by tacking on the requested
 * space to the end of the heap thus far.
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include <stddef.h> // for NULL
#include "strings.h"
#include "printf.h"


extern int __bss_end__;

// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define TOTAL_heap_used 0x1000000 // 16 MB
static int const HEADER_SIZE = sizeof(int);
static int const ROUND_UP_VAL = 8;
static int const LAST_BIT_VAL = 0b1;
static void *heap_start = NULL;
static int const FREE = 0;
static int const IN_USE = 1;
static int heap_used = 0, heap_max = TOTAL_heap_used;


/*Function to verify if an address to a header/block is within
bounds of the heap */
static int is_valid_addr(void *ptr){
  if (heap_start){
    if (ptr >= heap_start && (char * )ptr  <  ((char *)heap_start + heap_max) ) {
      return 1;
    }
  }
  return 0;
}

/*Get the address for memory block given a pointer to a header*/
static int * get_block_addr (int * hdr){
  return (hdr + 1);
}

/*Calculate block status from dereferenced int * hdr instance.
Block status will be represented in the last bit. */
static int get_block_status (int * hdr){
  return( *hdr  & LAST_BIT_VAL );
}

/*Calculate block size from dereferenced int * hdr instance. */
static int get_block_size (int * hdr){
  return ( *hdr >> sizeof(int));
}

/*Function to calculate the next header; if the next header
is is out of bounds (invalid), return NULL */
static int * next_header (int * hdr){
  char * block_addr = (char *) get_block_addr(hdr);
  int * next_hdr =  (int *)( block_addr + get_block_size(hdr));
  if (is_valid_addr(next_hdr)){
    return next_hdr;
  }
  else{
    return NULL;
  }
}

/*Function call to intialize the heap as one giant free block*/
static void initialize_heap ( char * address){
    int * hdr;
    hdr = (int *) address;
    *hdr =   ( (TOTAL_heap_used - HEADER_SIZE) <<  sizeof(int)) | FREE;
    heap_used += sizeof(int);

}

/*Function used to search freed blocks for reuse, walking
the heap from beginning to end.
*/
static int * find_free_block ( size_t nbytes){
  int * current = heap_start;
  while (current != NULL && (get_block_status(current) == IN_USE || get_block_size(current) < nbytes)){
    current = next_header(current);
  }
  if (get_block_status(current) == IN_USE){
    return NULL;
  }
  return current;
}


/*Split a block of memory into an in-use block of size
nbytes and some portion of a remainder that is free
*/
static int * split_block(int * hdr, size_t nbytes ){
  int value = get_block_size(hdr) - nbytes;
  if ( value >= HEADER_SIZE){
    int * nhdr;
    char * block_addr =  (char *) get_block_addr (hdr);
    nhdr = (int *)(block_addr + nbytes );
    *nhdr = (get_block_size(hdr) - nbytes - HEADER_SIZE) << sizeof(int) | FREE;
    *hdr = nbytes << sizeof(int) | IN_USE;
    heap_used += HEADER_SIZE;
    return hdr;
   }
  else{
    return NULL;
  }

}

/*If forward block neighbors to a particular block are free,
coalesce the two blocks. Note that the initial, particular
block doesn't have to be free: this is such that this function
can be used in both realloc and free.
*/
static int * coalesce_blocks(int * hdr){
  int * next = next_header(hdr);
  while( next && (get_block_status(next) == FREE )){
    *hdr +=  (HEADER_SIZE + get_block_size(next) ) << sizeof(int);
    heap_used -= HEADER_SIZE;
    next = next_header(next);
  }
  return hdr;
}

//print details about a particular header given its pointer
static void print_header (int * hdr){
  if (is_valid_addr(hdr)){
    printf("Address of header: %p\n", hdr);
    printf("Address of header+1 (address of memory block): %p\n", get_block_addr(hdr));
    printf("Size of memory block: %d\n", get_block_size(hdr));
    printf("Status of header: %d\n", get_block_status(hdr));
    if (get_block_status(hdr)){
      printf("Content stored in memory block: %s\n", (char *) get_block_addr(hdr)  );
    }
    printf("Pointer to next header:%p\n\n", next_header(hdr));
  }
  else{
      printf("CANNOT PRINT HEADER B/C ADDRESS INVALID\n\n");
  }

}

//print information about heap
 static void heap_status(){
   printf ("----HEAP STATUS---\n");
   printf ("heap_used: %d\n", heap_used);
   printf ("max heap size: %d\n", heap_max);
   printf ("address of __bss_end__end: %p\n", &__bss_end__);
   printf ("address of heap_start: %p\n", heap_start);
   printf ("------------------\n");

 }

//print information about each header + the heap
void heap_dump () {
   int * current = heap_start;
   int n = 1;
    printf ("\n------START OF HEAP DUMP------\n\n");
   while (current != NULL){
     printf("Header #%d: \n",n );
     print_header(current);
     current = next_header(current);
     n++;
   }
   heap_status();
   printf ("\n------END OF HEAP DUMP------\n\n");
}

// /* Global variables for heap allocator
//  *
//  * `heap_start` tracks where heap segment begins in memory.
//  * It is initialized to point to the address at end of data segment.
//  * It uses symbol __bss_end__ from memmap to locate end.
//  * `heap_used` tracks the number of bytes allocated thus far.
//  * The next available memory begins at heap_start + heap_used.
//  * `heap_max` is total number of bytes set aside for heap segment.
//  */

//see malloc.h for details
void *malloc(size_t nbytes){
    nbytes = roundup(nbytes, ROUND_UP_VAL);
    int * hdr;
    if (!heap_start) {
      heap_start = &__bss_end__;
      initialize_heap(heap_start);
    }
    if (nbytes == 0){
      return NULL;
    }
    hdr = find_free_block( nbytes);
    if (hdr){
        hdr = split_block(hdr, nbytes); // attempt to split block
        heap_used += nbytes;
        return get_block_addr (hdr);
    }
    return NULL;
}

//return header from pointer to memory block
int * get_header(void * ptr){
  return (int *) ptr - 1;
}

//see malloc.h for details
void free(void *ptr)
{
    int * hdr;
    if (is_valid_addr(ptr)){
      hdr = get_header(ptr);
      int size = get_block_size(hdr);
      *hdr ^= 0b1;
      hdr = coalesce_blocks(hdr);
      heap_used -= size;
    }
}

//see malloc.h for details
void *realloc (void *old_ptr, size_t new_size){
    int * hdr;
    int * new_alloc;
    if (!old_ptr){
      return malloc(new_size); //if old_ptr is NULL, realloc behaves like malloc
    }
    new_size = roundup(new_size, ROUND_UP_VAL);
    if (is_valid_addr(old_ptr)){
      if (new_size == 0){ //if new_size is zero, realloc behaves like free
        free(old_ptr);
        return NULL;
      }
      hdr = get_header(old_ptr);
      int old_size = get_block_size(hdr);
      if (old_size >= new_size){
        return old_ptr;
      }
      hdr = coalesce_blocks(hdr);
      hdr = split_block(hdr, new_size);
      if (hdr == NULL){
        //try to allocate new_size, if unable, allocate whatever space available
        int size = (new_size <= heap_max - heap_used ) ? new_size : heap_max-heap_used;
        new_alloc = malloc(size);
        if (!new_alloc){
            return NULL;
        }
        memcpy(new_alloc, old_ptr, size);
        free(old_ptr);
        return new_alloc;
      }
      else{
        heap_used += new_size-old_size;
        return get_block_addr(hdr);
      }
    }
    return NULL;
}
