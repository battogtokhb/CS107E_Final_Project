#include "strings.h"


//helper function for strtonum
static int isdigit(char c)
{
    return (c >= '0' && c <= '9');
}


//helper function for strtonum
static int isalpha(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}


//helper function for strtonum
static int isspace(char c)
{
    int value =  (c == ' ') || (c == '\t') || (c == '\n');
    return value;
}


//helper function for strtonum
static int isupper(char c)
{
    return (c >= 'A' && c <= 'Z');
}


//see string.h for description
void *memset(void *s, int c, size_t n)
{
    unsigned char * ptr = (unsigned char* )s;
    unsigned char value = (unsigned char)c;
    while (n > 0){
      *ptr++ = value;
      n--;
    }
    return s;
}

//see string.h for description
void *memcpy(void *dst, const void *src, size_t n)
{
    char * d = ( char *)dst;
    const char * s = ( char *)src;
    while (n > 0){
      *d++ = *s++;
      n--;
    }
    return dst;
}

//see string.h for description
int strlen(const char *s)
{
    int i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

//see string.h for description
int strcmp(const char *s1, const char *s2)
{
  for (;  *s1 == *s2; s1++, s2++){
    if (*s1 == 0 ){
      return 0;
    }
  }
  return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

//see string.h for description
int strlcat(char *dst, const char *src, int maxsize)
{
   int d_len = strlen(dst);
   int s_len = strlen(src);


    int n = d_len;
    int x = 0;
    while ( (x <= maxsize - 1 -d_len) && (*src != '\0') ){
      dst[n] = *src++;
      n++;
      x++;
    }
    dst[n] = '\0';

   return d_len + s_len;


}

//see string.h for description
unsigned int strtonum(const char *str, const char **endptr)
{
  unsigned int value = 0;
  unsigned int base = 10;
  unsigned char * s = (unsigned char *)str;
  volatile char c = *s++;
  int bounds = 0;
  while ( (isspace(c) == 1) ){
    c = *s++;
  }
  if (c == '-'){
     *endptr = (char *) s-1;
     return 0;
   }
   if  (c == '0' && ( *s == 'x' || *s == 'X') ){
     base = 16;
      c = s[1];
      s += 2;
   }
   unsigned int cutoff =  4294967295;
   for (; bounds == 0 && (c != '\0'); c = *s++){
     if (isdigit(c)){
       c -= '0';
     }
     else if (isalpha(c)){
       c -= isupper(c) ?  'A' - 10 :  'a' -10 ;
     }
     else{
       break;
     }
     if (c >= base){
       break;
     }
     value *= base;
      value += c;
      if ( value >= cutoff  ){
        bounds = 1;
        break;
      }
   }

   if (endptr != NULL){
     *endptr = (char *) s-1;
   }
   if (bounds){
     return  cutoff;
   }
  return  value ;
}
