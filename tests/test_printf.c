#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "keyboard.h"
#include "interrupts.h"
#include "strings.h"
#include "uart.h"


static void test_memset(void)
{

    //standard generic tests
    int num1 = 0xefefefef;
    int num2;
    memset(&num2, 0xef, sizeof(int));
    assert(num1 == num2);

    char * s = "hello";
    memset(s, 65, 2);
    assert(strcmp(s, "AAllo") == 0);

    char str[50] = "CS107 is for programming geeks!";
    memset(str + 5, '.', 8*sizeof(char));
    assert(strcmp(str, "CS107........programming geeks!") == 0);
}

static void test_memcpy(void)
{
    //standard generic tests
     int num1 = 0x12345678;
     int num2 = 0;
     memcpy(&num2, &num1, sizeof(int));
     assert(num1 == num2);

     int num3 = 0;
     memcpy(&num3, &num1, sizeof(int)/4);
     assert(num3 == 0x78);

     int num4 = 0;
     memcpy(&num4, &num3, sizeof(int));
     assert(num4 == 0x78);

}

static void test_strlen(void)
{
    //standard generic tests
    assert(strlen("green") == 5);
    assert(strlen("butter") == 6);
    assert(strlen("") == 0);
    assert(strlen("  ") == 2);
}

static void test_strcmp(void)
{
    //standard generic tests
    assert(strcmp("apples", "apples") == 0);
    assert(strcmp("apple", "apples") == -1);
    assert(strcmp(" ", "apples") == -1);
    assert(strcmp(" ", " ") == 0);
    assert(strcmp("zebra", "banana") == 1);
    assert(strcmp("Zebra", "zebra") == -1);
}

static void test_strlcat(void)
{
    //standard generic test
    char buf[20] = "CS";
    strlcat(buf, "107e", 20);
    assert(strcmp(buf, "CS107e") == 0);

    //standard test using my name
    char buf1[50] = "Bilguunzaya";
    strlcat(buf1, " Battogtokh", 50);

    assert(strcmp(buf1, "Bilguunzaya Battogtokh") == 0);

    //test for when maxsize == 0
    char buf2[50] = "I'm fine just the way I am";
    strlcat(buf2, "No, you are not!", 0);
    assert(strcmp(buf2, "I'm fine just the way I am") == 0);

     char buf3[10] = "CS107e" ;
     strlcat(buf3, "CS107e", 10);

}

static void test_strtonum(void)
{
  //  test for decimal
    const char *input = "013", *rest;
    int val = strtonum(input, &rest);
    assert(val == 13 && rest == (input + strlen(input)));

    //test for hexadecimal
    const char *a = "0xbeef", *b;
    int c = strtonum(a, &b);
    assert(c == 48879 && b == (a + strlen(a)) );

    //testing for illegal characters
    const char *d = "0xbeefOops", *e;
    int f = strtonum(d, &e);
    assert(f == 48879 && e == (d + strlen(a)) );

    //testing for negatives
    const char *g = "-0x8000000F", *h;
    int i = strtonum(g, &h);
    assert(i == 0 && h == (g ) );

    //testing for illegal characters (when we use hex but identify '0x' at the beginning)
    const char *j = "013F", *k;
    int l = strtonum(j, &k);
    assert(l == 13 && k == (j + 3));

    const char *m = "0xdeadbeef", *n;
    unsigned int o = strtonum(m, &n);
    assert(o == 3735928559 && n == (m + strlen(m)));
}


// These aren't part of printf public interface, we must declare them here to
// be able to use them in this test file.
int unsigned_to_base(char *buf, int n, unsigned int val, int base, int min_width);
int signed_to_base(char *buf, int n, int val, int base, int min_width);

static void test_to_base(void)
{
    //test taken from lab spec
     char buf[5];
     int a = signed_to_base(buf, 5, -9999, 10, 6);
     assert(strcmp(buf, "-099") == 0 && a == 6);

    ////test taken from lab spec
    char buf1[20];
   int b = unsigned_to_base(buf1, 20, 128, 16, 6);
    assert(strcmp(buf1, "000080") == 0 && b == 6);

    //test taken from lab spec
    char buf2[20];
    int c = signed_to_base(buf2, 20, 94305, 10, 0);
    assert(strcmp(buf2, "94305") == 0 && c == 5);

    //testing case when bufsize < minwidth
     char buf3[3];
     int d = unsigned_to_base(buf3, 3, 23, 10, 5);
     assert(strcmp(buf3, "00") == 0 && d == 5);

     //testing negative case when bufsize < minwidth
     char buf4[4];
     int e = signed_to_base(buf4, 3,  -673920, 16, 7);
     assert(strcmp(buf4, "-0") == 0 && e == 7);

     //testing minwith with hexadecimals
     char buf5[20];
     int f = signed_to_base(buf5, 20,  -673920, 16, 10);
     assert(strcmp(buf5, "-0000a4880") == 0 && f == 10);

     //check that nothing is written into the buffer if bufsize = 0
     char buf6[20];
     int g = signed_to_base(buf6, 0,  -673920, 16, 10);
     assert(g == 0);

     char buf7[100];
     int h = unsigned_to_base(buf7, 100, 0x80000000, 10, 0);
     assert(strcmp(buf7, "2147483648") == 0 && h == 10);

     char buf8[100];
     int i = signed_to_base(buf8, 100, 0x80000000, 10, 0);
     assert(strcmp(buf8, "-2147483648") == 0 && i == 11);

}


static void test_snprintf(void)
{
    char buf[100];
//    char * p = buf;
//     printf ("buf-point : %p\n", p);
     int bufsize = sizeof(buf);
//
//     //Bsic test
//     int a = snprintf(buf, bufsize, "Hello, world!");
//     assert(strcmp(buf, "Hello, world!") == 0 && a == 13);
//
//   //  Decimal
//     int b = snprintf(buf, bufsize, "%d", 45);
//     assert(strcmp(buf, "45") == 0 && b == 2);
// // //
//     // Hexadecimal
//     int c = snprintf(buf, bufsize, "%04x", 0xef);
//     assert(strcmp(buf, "00ef") == 0 && c == 4);
//
// //    Pointer
//     int d = snprintf(buf, bufsize, "%p", (void *) 0x20200004);
//     assert(strcmp(buf, "0x20200004") == 0 && d == 10);
//
//     // Character
//     int e = snprintf(buf, bufsize, "%c", 'A');
//      assert(strcmp(buf, "A") == 0 && e == 1);
// //
// //     // String
//     int f = snprintf(buf, bufsize, "%s", "binky");
//     assert(strcmp(buf, "binky") == 0 && f == 5);
// //
//     // Format string with intermixed codes
//     snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
//     assert(strcmp(buf, "CS107e!") == 0);
//
//     // Test return value
//     assert(snprintf(buf, bufsize, "Hello") == 5);
//     assert(snprintf(buf, 2, "Hello") == 5);
//
//     //Test minwidth
//     int g =  snprintf ( buf, bufsize, "The half of %02d is %03d", 60, 30 );
//     assert(strcmp(buf, "The half of 0060 is 00030") && g == 21 );
//
//     char buf1[10];
//     int h =  snprintf ( buf1, 10, "%020x", 5 );
//     //printf("buf1: %s | value of h: %d", buf1, h);
//     assert(strcmp(buf1, "000000000") == 0 && h == 20 );


    int i =  snprintf ( buf, bufsize, "%02x", 5);
     printf(" buf: %s, value of i : %x\n",  buf, i);


  //   int j =  snprintf ( buf, bufsize, " value of i : %x", 4);
    //  printf("buf: %s\n", buf);
    //   printf(" value of i : %x\n", 4);
    //assert(strcmp(buf1, "000000000") == 0 && h == 20 );





  //  char buf2[10];
    //int i =  snprintf ( buf2, 0, "%020x", 5 );

  //  assert(strcmp(buf1, "000000000") == 0 && h == 20 );





 }


void main(void)
{

    volatile unsigned int * const FSEL2 = (unsigned int *) 0x20200008;
     uart_init();
     test_snprintf();
    // test_memset();
    // test_memcpy();
    // test_strlen();
    // test_strcmp();
     //test_strlcat();
//    test_strtonum();
  //   test_to_base();

     //testing for different printf arguments
    // int num = 0x0;
    // char * address =  num;
    //printf("%08p: %08x\n", address, *address);

      printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
      int i = 0x80;
      static unsigned char scancode = '\0';
      while(1){
            unsigned char scancode = keyboard_read_scancode();
            if (scancode != '\0'){
              printf("whoopie!\n");
              scancode = '\0';
            }


      }

     // printf("%d = -5\n", -5);
     // printf("\n");
     // printf("%p = 0x20200008\n", FSEL2);
     // printf("%04x = 00ab\n", 0xab);
     // printf("My favorite letter is %c\n", 'Z');
     // printf("My favorite movie is %s\n", "Spongebob Squarepants");
     // printf("Scancode: %x", (unsigned char) 240);

}
