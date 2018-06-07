#include "backtrace.h"
#include "printf.h"
#include <stdint.h>
#include <stdarg.h>


int const RIGHT_SHIFT_4TH_BYTE = 24;
char * const INVALID_FUNC_NAME =  "???";
int const VALID_FUNC_NAME_START = 0xff;
int * const TERMINATE_SAVED_FP = 0x0;

int backtrace(frame_t f[], int max_frames){
    int n = 0;
    int * cur_fp;
    __asm__("mov %0, fp" : "=r" (cur_fp));
    while (n < max_frames){
      uintptr_t  resume_addr =  (uintptr_t) *(cur_fp - sizeof(char) );
      int * saved_fp =  (int *) *(cur_fp - 3* sizeof(char) ) ;
      int * caller_first = (int *) *saved_fp - 3*sizeof(char);
      //stop backtrace if the saved frame pointer is equal to 0x0
      if (saved_fp == TERMINATE_SAVED_FP){
          break;
      }
      char * name = "/0";
      int ff_check_value = (caller_first[-1] >>  RIGHT_SHIFT_4TH_BYTE ) & 0xff ;
      if (ff_check_value == VALID_FUNC_NAME_START){
         int name_length = ( (caller_first[-1]) & ~0xff000000) ;
         name = (char *)(caller_first - 1  - (name_length/sizeof(int)) );
      }
      else{
        name = INVALID_FUNC_NAME;
      }
      int resume_offset = resume_addr -  (uintptr_t) (caller_first) ;
      frame_t frame = {name, resume_addr, resume_offset};
      f[n] = frame;
      n++;
      cur_fp = saved_fp;
    }
    return n;
}

void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
