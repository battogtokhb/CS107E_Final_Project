#include "keyboard.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "console.h"
#include "interrupts.h"

#define ESC_SCANCODE 0x76

static int isalpha(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}


static void test_keyboard_scancodes(void)
{
    printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
    while (1) {
          //printf("before reading scancode\n");
          unsigned char scancode = keyboard_read_scancode();
        //  printf("after reading scancode\n");
          printf("Scancode: %d", scancode);

          if (scancode == ESC_SCANCODE) break;
         unsigned char ch = ps2_keys[scancode].ch;
        //
      if ( (ch >= ' ' && ch <= '~') && ( (int) scancode != 0xf0 && (int) scancode != 0xe0)) printf("(ascii '%c')", ch);

        printf("\n");


    }
    printf("Done with scancode test.\n");
}

static void test_keyboard_sequences(void)
{
    printf("\nNow reading scancode sequences. Type ESC to finish this test.\n");
    while (1) {
        unsigned char seq[3];
        int len = keyboard_read_sequence(seq);
        printf("Scan code sequence: ");
        for (int i = 0; i < len; i++)
            printf("%02x ", seq[i]);
        if (seq[0] == ESC_SCANCODE) break;
        unsigned char ch = ps2_keys[seq[len-1]].ch;
        if (ch >= ' ' && ch <= '~') printf("(ascii '%c')", ch);
        printf("\n");
    }
    printf("Done with scancode sequences test.\n");
}

static void test_keyboard_events(void)
{
    printf("\nNow reading key events. Type ESC to finish this test.\n");
    while (1) {
        key_event_t evt = keyboard_read_event();
        printf("%s Key: [%02x] ",  evt.action == KEYBOARD_ACTION_DOWN? "Down" : "Up", evt.key.ch  );
         if (isalpha(evt.key.ch)){
           printf("[Character: %c] ", evt.key.ch);
         }
        printf("Modifiers: 0x%x\n", evt.modifiers );
        if (evt.key.ch == ps2_keys[ESC_SCANCODE].ch && evt.action == KEYBOARD_ACTION_DOWN) break;
    }
    printf("Done with key events test.\n");
}

static void test_keyboard_chars(void)
{
    //console_printf("\nNow reading chars. Type ESC to finish this test.\n");
    while (1) {
         char c = keyboard_read_next();
      //  if (c >= '\t' && c <= 0x80){
          console_printf("%c", c);
      
        // }

        if (c == ps2_keys[ESC_SCANCODE].ch) break;
    }
    console_printf("\nDone with key chars test.\n");
}

void main(void)
{
    uart_init();
    keyboard_init();

  //  keyboard_use_interrupts();
    console_init(20, 40);

    printf("Testing keyboard module.\n");
//
    test_keyboard_scancodes();
    //timer_delay_ms(500);

  //  test_keyboard_sequences();
    //timer_delay_ms(500);

  //  test_keyboard_events();
  //  timer_delay_ms(500);

    test_keyboard_chars();

    printf("All done!\n\04");
}
