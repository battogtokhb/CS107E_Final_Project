#include "printf.h"
#include "strings.h"
#include <stdarg.h>
#include "uart.h"

#define MAX_OUTPUT_LEN 1024



//implementation of memset but for explicit usage for srings
//note that strcpy null terminates
static int strcpy(char *dst, const char *src, size_t n){
    int i = 0;
    while (*src != '\0' && n > 0){
      *dst++ = *src++;
      n--;
      i++;
    }
    *dst = '\0';
    return i;
}

static int isdigit(char c)
{
    if (c >= '0' && c <= '9') {
      return 1;
    }
    else{
        return 0;
    }

}


//Based on mind_width and the conversionLength, calculates
//how many zeros to pad
static int zeroPad(int bufsize, int min_width, int conversionLength){
    if (conversionLength >= min_width){ //c
      return 0;
    }
    int zeros = min_width - conversionLength;
    return zeros;

}

//helper function for unsigned_to_base
//converts last digit in decimal/hexidecimal
//to string representation
static char strConv(unsigned int v, int base){
  int r = v % base;
  if (r < 10){
    return '0' + r;
  }
  else{
    return 'a' + (r - 10);
  }
}


static int unsigned_to_base(char *buf, int bufsize, unsigned int val, int base, int min_width)
{
     if (bufsize == 0){
      return 0;
     }
    int neg = *buf == '-' ? 1: 0;
    //char * s converts all the digits in the integer representation
    //to string form in reverse order
    char * s = NULL;
    unsigned int v = val < 0 ? -val : val;
    int conversionLength  = 0;

    if ( v == 0){
      s[conversionLength] = strConv(v, base);
      conversionLength++;
    }
    else{
      while (v ){
         s[conversionLength] = strConv(v, base);
         v = v / base;
         conversionLength++;
      }
    }
    //writeLocation keeps track of which index we write to buf
    int writeLocation = 0 + neg;
    int bytesUnlimited = writeLocation;
    int zeros = zeroPad(bufsize, min_width-neg, conversionLength);
    bytesUnlimited += zeros;
    //if bufsize-1-neg (depending on prescence of negative size) is less than
    //the number of zeros we need to pad, set a limit
    int padLimit =  (bufsize -1-neg < zeros ) ? (bufsize-1 -neg): zeros;
    memset(buf+neg, '0', padLimit);
    writeLocation += padLimit;
    int i = 0;
    //if the value is negative, buf already contains the negative sign,
    //therefore we write to bufsize-1, NOT bufsize-1-neg
    //NOTE: writeLocation already considers the presence of the negative
    while (writeLocation < bufsize - 1 && (i < conversionLength)){ //bufsize-1 because of null-terminator
      //write from s to buf in the correct string order (representation in s is backwards)
      *(buf+writeLocation) = s[conversionLength - i - 1];
      writeLocation++;
      bytesUnlimited++;
      i++;
    }
    //add a null terminator
    *(buf+writeLocation) = '\0';
    //if not all the digits were written (sans null terminator) to the buffer, calculate
    //how much should've been written if we had space
    while (i < conversionLength){
      bytesUnlimited++;
      i++;
    }
    return bytesUnlimited;
}

static int signed_to_base(char *buf, int bufsize, int val, int base, int min_width)
{
  if (bufsize == 0){
    return 0;
  }
  //if the value if negative, write a negative sign to the buffer
  if (val < 0){
    memset(buf, '-',1 );
    return unsigned_to_base(buf, bufsize, -val, base, min_width);
  }
  else{
    return unsigned_to_base(buf, bufsize, val, base, min_width);
  }
}

int vsnprintf(char *buf, int bufsize, const char *format, va_list args)
{

  char temp_buf[MAX_OUTPUT_LEN];
  memset(temp_buf, '0', MAX_OUTPUT_LEN);
  int strLength = 0;
  while(*format){
    if (*format != '%'){
        temp_buf[strLength++] = *format++;
      }
    else{
       char percent = *format++; //skip the %
       int min_width = 0;
       char * min_width_str = '\0' ;
       const char ** end = '\0';
       int n = 0;
       //keep track of min_width identifer if available
       while (isdigit(*format) == 1){
         min_width_str[n++] = *format ;
         format++;
       }
       min_width_str[n] = '\0';
       min_width = strtonum(min_width_str, end);
        char code = *format++;
       if (code == 'd'){
          int num = va_arg(args, int);
          strLength += signed_to_base(&temp_buf[strLength], MAX_OUTPUT_LEN, num, 10, min_width );
      }
      else if (code == 'x' || code == 'p'){
        if (code == 'p'){
          temp_buf[strLength++] = '0';
          temp_buf[strLength++] = 'x';
        }

        int num = va_arg(args, int);
        strLength += signed_to_base(&temp_buf[strLength], MAX_OUTPUT_LEN, num, 16, min_width );
      }
      else if (code == 'c'){
         int c = va_arg(args, int);
        temp_buf[strLength++] = (char) c;
      }
      else if (code == 's'){
        char * s = va_arg(args, char *);
        int length = strlen(s);
        strLength += strcpy(&temp_buf[strLength], s, length);
      }
      else{
        //if there is no valid code, simply write the percent sign into the buffer
        temp_buf[strLength++] = percent;
      }
    }
  }

  va_end(args);
  if (bufsize == 0){
    return strLength;
  }
  int bytes = (strLength <= bufsize-1 )? strLength : bufsize-1;
  strcpy(buf, temp_buf,bytes );
  return strLength;
}


int snprintf(char *buf, int bufsize, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int r = vsnprintf(buf, bufsize, format, args);
    va_end(args);
    return r;
}

int printf(const char *format, ...){
    char buf[MAX_OUTPUT_LEN];
    va_list args;
    va_start(args, format);
    int r = vsnprintf(buf, MAX_OUTPUT_LEN, format, args);
    va_end(args);
    for (int i = 0; buf[i] != '\0'; i++) {
        uart_putchar(buf[i]);
    }

    return r;

}
