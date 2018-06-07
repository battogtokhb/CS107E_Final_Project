
#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "timer.h"
#include "assert.h"
#include "interrupts.h"
#include "ringbuffer.h"
#define MILLISECOND 1000 //millisecond defined in terms of microseconds

const unsigned int CLK  = GPIO_PIN23;
const unsigned int DATA = GPIO_PIN24;

static rb_t* all_scancodes;




static int isalpha(char c)
{
    return (c >= 'a' && c <= 'z');
}


void wait_for_falling_clock_edge() {
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}



static void keyboard_handler(unsigned int pc){
  static  int odd_bits = 0;
  static  int parity = 0;
  static int start  = 1;
  static int code = 0;
  static int count = 0;

    if (gpio_check_and_clear_event(GPIO_PIN23) ){

      if (start == 0){
        if (count == 9){
          int stop = gpio_read(DATA);
          int check_parity = 0;
          if (odd_bits % 2 == 0){
            check_parity = 1;
          }
          if ( (parity == check_parity) && (stop == 1 )) {
            rb_enqueue(all_scancodes, code);
          }
          code = 0;
          count = 0;
          odd_bits = 0;
          parity = 0;
          start = 1;
        }

        else if (count == 8){
          parity = gpio_read(DATA);
          count++;
        }
        else{
        //  printf("count: %d\n", count);
          int bit = gpio_read(DATA);
          if (bit == 1) {
            odd_bits++;
          }
          code |= (bit << count);
          count++;

        }

      }
      else{
         start = gpio_read(DATA);
      }

    }

  }


void keyboard_init(void)
{
    gpio_set_input(CLK);
    gpio_set_pullup(CLK);

    gpio_set_input(DATA);
    gpio_set_pullup(DATA);
    all_scancodes = rb_new();

    gpio_disable_all_event_detection(CLK);
    gpio_enable_event_detection(CLK, GPIO_DETECT_FALLING_EDGE);
    bool ok = interrupts_attach_handler(keyboard_handler);
    assert(ok);
    interrupts_enable_source(INTERRUPTS_GPIO3);
    interrupts_global_enable();

}

/*
  The function input_sync is used to determine if the
  current falling clock edge occured more than 3ms
  after the previous one.
*/


/*
  See keyboard.h for details on keyboard_read_scancode().
  Note: if the current falling clock edge occured more than 3ms after
  the previous one, the scan code reader is reset
*/
unsigned char keyboard_read_scancode(void){



  



  while(1){

      int value = 0;
      if( !rb_empty(all_scancodes) ) {

          rb_dequeue(all_scancodes, &value);
          return (unsigned char ) value;

     }

   }

}

/*
  See keyboard.h for details on keyboard_read_sequence().
*/
int keyboard_read_sequence(unsigned char seq[])
{
  seq[0] = keyboard_read_scancode();
  int n = 1;
  if ((int) seq[0] == 0xf0 || (int ) seq[0] == 0xe0 ){
    seq[n] = keyboard_read_scancode();
    n++;
    if ( (int) seq[n-1] == 0xf0){
      seq[n] = keyboard_read_scancode();
      n++;
    }
  }
  return n;
}

/*
  The function which_modifier returns the bit flag associated
  with each state of the various modifiers based on a ps2_key_t paramter
*/
static keyboard_modifiers_t which_modifier(ps2_key_t key){

  if (key.ch == PS2_KEY_SCROLL_LOCK ){
    return KEYBOARD_MOD_SCROLL_LOCK ;
  }
  if (key.ch == PS2_KEY_NUM_LOCK ){
    return KEYBOARD_MOD_NUM_LOCK ;
  }
  if (key.ch == PS2_KEY_CAPS_LOCK ){
    return KEYBOARD_MOD_CAPS_LOCK ;
  }
  if (key.ch == PS2_KEY_SHIFT ){

    return KEYBOARD_MOD_SHIFT;
  }
  if (key.ch == PS2_KEY_ALT ){
    return  KEYBOARD_MOD_ALT ;
  }
  if (key.ch == PS2_KEY_CTRL ){
    return KEYBOARD_MOD_CTRL ;
  }
  return (keyboard_modifiers_t) 0;

}

/*
  The function calculate_modifier determines the current state of
  the key modifier based on past and present behaviors
*/
static keyboard_modifiers_t calculate_modifier(ps2_key_t key,keyboard_action_t action){
  static int MODIFIER = 0x0;
  static int CAPS_LOCK = 0;
  keyboard_modifiers_t modifier = which_modifier(key);
  /*
  The caps lock key is special because you have to press it to active/deactivate;
  unlike the other modifiers, it doesn't automatically deactivate at the key release.
  Therefore, I have a specific variable, CAPS_LOCK to track the caps lock key.
  */
  if (modifier == KEYBOARD_MOD_CAPS_LOCK){
    if (CAPS_LOCK == 0 && action == KEYBOARD_ACTION_DOWN){
      MODIFIER |= modifier;
    }
    if (CAPS_LOCK == 0 && ((MODIFIER & modifier) == modifier) && action == KEYBOARD_ACTION_UP){
      CAPS_LOCK = 1;
    }
    if (CAPS_LOCK == 1 && action == KEYBOARD_ACTION_DOWN ){
      MODIFIER &= ~(modifier);
    }
    if (CAPS_LOCK == 1 && ((MODIFIER & modifier) != modifier)&& action == KEYBOARD_ACTION_UP){
      CAPS_LOCK = 0;
    }
  }
  if ( modifier != KEYBOARD_MOD_CAPS_LOCK && action == KEYBOARD_ACTION_DOWN ){
    if ( (MODIFIER & modifier) != modifier){
      MODIFIER |= modifier;
    }
  }
  if ( modifier != KEYBOARD_MOD_CAPS_LOCK && action == KEYBOARD_ACTION_UP ){
    MODIFIER &= ~(modifier);
  }
  return (keyboard_modifiers_t) MODIFIER;

}

/*
  See keyboard.h for details on keyboard_read_event().
*/
key_event_t keyboard_read_event(void)
{

    unsigned char seq[3];
    int len = keyboard_read_sequence(seq);
    key_event_t event;
    event.seq[3] = *seq;
    event.seq_len = len;
    event.key = ps2_keys[seq[len-1]];
    keyboard_action_t action = KEYBOARD_ACTION_DOWN;
    //if statements to determine if a key is released
    if (len == 2){
      if ((int) seq[0] == PS2_CODE_RELEASE) {
          action = KEYBOARD_ACTION_UP;

      }
    }

    if (len == 3){
        if ( (int) seq[1] == PS2_CODE_RELEASE  ){
          action = KEYBOARD_ACTION_UP;
        }

    }

    event.action = action;
    event.modifiers = calculate_modifier(ps2_keys[seq[len-1]], action);
    return event;
}


/*
  See keyboard.h for details on keyboard_read_next().
*/
unsigned char keyboard_read_next(void)
{
    while (1){
      key_event_t event = keyboard_read_event();
      int modifier = (int) event.modifiers;


       if (event.action == KEYBOARD_ACTION_UP) continue; //if key is in release state, do not return

      if (which_modifier(event.key) != 0) continue;
      int caps_mod = ( (modifier & KEYBOARD_MOD_CAPS_LOCK) == KEYBOARD_MOD_CAPS_LOCK) ? 1 : 0;
      int shift_mod = ( (modifier & KEYBOARD_MOD_SHIFT) == KEYBOARD_MOD_SHIFT) ? 1 : 0;
      int ctrl_mod  = ( (modifier & KEYBOARD_MOD_CTRL) == KEYBOARD_MOD_CTRL) ? 1 : 0;
      int alt_mod  = ( (modifier & KEYBOARD_MOD_ALT) == KEYBOARD_MOD_ALT) ? 1 : 0;
      if ( alt_mod) continue; //if the modifier for the alt key is implemented, do not return
      if (event.key.ch <= 0x7F){ //ascii characters
        if (isalpha(event.key.ch)){ //if key is an alphabetic character
          if (caps_mod ) return event.key.other_ch;
        }
        if (shift_mod) return event.key.other_ch;
        if (event.key.ch == 'a' && ctrl_mod) return 0x17;
        if (event.key.ch == 'e' && ctrl_mod) return 0x18;
        if (event.key.ch == 'u' && ctrl_mod) return 0x19;

        return event.key.ch;
      }
      else{ //non ascii characters
        if (shift_mod) continue; //if shift is implemented with non-ascii character, do not return
        return event.key.ch;
      }

    }
}
