#include "assert.h"
#include "backtrace.h"
#include "malloc.h"
#include "nameless.h"
#include "rand.h"
#include <stdint.h>
#include "strings.h"
#include "uart.h"

void main(void);
void heap_dump();  // Debugging function implemented in step 6

#define TOTAL_HEAP_SIZE 0x1000000
#define HEADER_SIZE 4

static int rec_1(int x);
static int rec_2(int x);

static int abs(int v)
{
    int result = v < 0 ? -v : v;
    frame_t f[5];
    int frames_filled = backtrace(f, 5);
    print_frames(f, frames_filled);
    return result;
}

static int diff(int a, int b)
{
    return abs(a - b);
}

static void test_backtrace_custom(){
  int x = 33, y = 107;
  int d = diff(x, y);

}

static int rec_2(int x){
  if (x > 0){
    return rec_1(x-1);
  }
  else{
    return x;
  }
}

static void see_frame(){
  frame_t f[20];
  int frames_filled = backtrace(f, 20);
  print_frames(f, frames_filled);
}


static int rec_1(int x){
  if (x > 0){
    return rec_2(x-1);
  }
  else if (x == 0){
    see_frame();
    return x;
  }
  else{
    return x;
  }
}

static int test_backtrace_custom_recursion(){
  return rec_1(6);

}

static void test_backtrace_simple()
{
    frame_t f[2];
    int frames_filled = backtrace(f, 2);

    assert(frames_filled == 2);
    assert(strcmp(f[0].name, "test_backtrace_simple") == 0);
    assert(f[0].resume_addr == (uintptr_t)test_backtrace_simple + f[0].resume_offset);
    assert(strcmp(f[1].name, "main") == 0);
    assert(f[1].resume_addr == (uintptr_t)main + f[1].resume_offset);
    print_frames(f, frames_filled);
}

static int recursion_fun(int n)
{
    if (n == 0)
        return mystery();   // look in nameless.c
    else
        return 1 + recursion_fun(n-1);
}

static int test_backtrace_complex(int n)
{
    return recursion_fun(n);
}

static void test_heap_limit(void)
{
    char * a = malloc(TOTAL_HEAP_SIZE - HEADER_SIZE); //this should not allocate anything to the heap
    assert(a == NULL);
    a = malloc(TOTAL_HEAP_SIZE - 2*HEADER_SIZE ); //this should allocate the heap to the largest size possible
    memcpy(a, "hello\0", 6);
    assert(strcmp(a, "hello") == 0);
    char * b = malloc(1); //this should not allocate anything to the heap
    assert(b == NULL);
    heap_dump();
    free(a);
    heap_dump();
}

static void test_heap_edge_cases(void)
{
    char *s = realloc(NULL, 6); //this should work exactly like malloc(6)
    memcpy(s, "hello", 6);
    heap_dump();
    assert(strcmp(s, "hello") == 0);
    realloc(s, 0); //this should work exactly like free(s);
    free(NULL); //this shouldn't free anything
    char * a = malloc(0); //this shouldn't allocate memory
    assert(a == NULL);
    heap_dump();
}


// array of dynamically-allocated strings, each
// string filled with repeated char, e.g. "A" , "BB" , "CCC"
// Examine each string, verify expected contents intact.
static void test_heap_simple(void)
{
    char *s = malloc(6);
     memcpy(s, "hello", 6);
     heap_dump();
     assert(strcmp(s, "hello") == 0);
     free(s);
     heap_dump();

     s = malloc(6);
     memcpy(s, "hello", 6);
     heap_dump();
     s = realloc(s, 12);
     strlcat(s, " world", 12);
     heap_dump();
     assert(strcmp(s, "hello world") == 0);
     free(s);
    heap_dump();
}


static void test_heap_multiple(void)
{
    int n = 26;
    char *arr[n];

    for (int i = 0; i < n; i++) {
        int num_repeats = i + 1;
        char *ptr = malloc(num_repeats + 1);
        assert(ptr != NULL);
        memset(ptr, 'A' - 1 + num_repeats, num_repeats);
        heap_dump();
        ptr[num_repeats] = '\0';
        arr[i] = ptr;

    }
    for (int i = n-1; i >= 0; i--) {
        int len = strlen(arr[i]);
        char first = arr[i][0], last = arr[i][len -1];
        assert(first == 'A' - 1 + len);
        assert(first == last);
        free(arr[i]);
    }


}

#define max(x, y) ((x) > (y) ? (x) : (y))

static void test_heap_recycle(int max_iterations)
{
    extern int __bss_start__;
    void *heap_low = &__bss_start__;
    void *heap_high = NULL;

    int i;
    void *p = malloc(1);

    heap_dump();
      printf("*****P: %p\n", (char *) p);

     for (i = 0; i < max_iterations; i++) {
          printf("ITERATION %d\n", i);
          int size = rand() % 1024;
          printf("*****SIZE: %d\n", size);
           void *q = malloc(size);
           heap_dump();
             printf("*****Q: %p\n", (char *) q);
           p = realloc(p, size);

           heap_dump();
             printf("*****P: %p\n", (char *) p);
           heap_high = max(heap_high, max(p, q));
           printf("*****HEAP HIGH: %p\n", (char *) heap_high);
           printf("*****HEAP SUBTRACT: %d\n", ((char *)heap_high - (char *)heap_low) );
          free(q);
          heap_dump();
     }
     free(p);

     printf("\nCompleted %d iterations. Heap grew to peak size of %d bytes.\n", i, (char *)heap_high - (char *)heap_low);
}


static void test_heap_redzones(void)
{
    // DO NOT ATTEMPT THIS TEST unless your heap has red zone protection!
    char *ptr;

    ptr = malloc(9);
    memset(ptr, 'a', 9);
    free(ptr); // ptr is OK

    ptr = malloc(5);
    ptr[-1] = 0x45; // write before payload
    free(ptr);      // ptr is NOT ok

    ptr = malloc(12);
    ptr[13] = 0x45; // write after payload
    free(ptr);      // ptr is NOT ok
}


void main(void)
{
    uart_init();

   test_backtrace_simple();
   test_backtrace_custom();
   test_backtrace_custom_recursion();
  test_backtrace_simple(); // Again so you can see the main offset change!

     int a = test_backtrace_complex(7);  // Slightly tricky backtrace.
     printf("a var: %d", a);

   test_heap_limit();
   test_heap_simple();
    test_heap_edge_cases();
    test_heap_multiple();
    test_heap_recycle(5); // increase the number for stress test
   // test_heap_redzones();
}
