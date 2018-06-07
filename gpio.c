#include "gpio.h"
#include "assert.h"

volatile unsigned int * const FSEL0 = (unsigned int *) 0x20200000;
volatile unsigned int * const FSEL1 = (unsigned int *) 0x20200004;
volatile unsigned int * const FSEL2 = (unsigned int *) 0x20200008;
volatile unsigned int * const FSEL3 = (unsigned int *) 0x2020000C;
volatile unsigned int * const FSEL4 = (unsigned int *) 0x20200010;
volatile unsigned int * const FSEL5 = (unsigned int *) 0x20200014;
volatile unsigned int * const SET0 = (unsigned int *) 0x2020001C;
volatile unsigned int * const SET1 = (unsigned int *) 0x20200020;
volatile unsigned int * const CLR0 = (unsigned int *) 0x20200028;
volatile unsigned int * const CLR1 = (unsigned int *) 0x2020002C;
volatile unsigned int * const LEV0 = (unsigned int *) 0x20200034;
volatile unsigned int * const LEV1 = (unsigned int *) 0x20200038;

static volatile unsigned int * assign_pin_to_register (unsigned int pin){
  if ((pin >= GPIO_PIN0) && (pin <= GPIO_PIN9) ){
     return FSEL0;
  }
  if ((pin >= GPIO_PIN10) && (pin <= GPIO_PIN19) ){
     return FSEL1;
  }
  if ((pin >= GPIO_PIN20) && (pin <= GPIO_PIN29) ){
     return FSEL2;
  }
  if ((pin >= GPIO_PIN30) && (pin <= GPIO_PIN39) ){
     return FSEL3;
  }
  if ((pin >= GPIO_PIN40) && (pin <=GPIO_PIN49) ){
     return FSEL4;
  }
  if ((pin >= GPIO_PIN50) && (pin <= GPIO_PIN53) ){
     return FSEL5;
  }
  return 0;
}

void gpio_init(void) {
}

void gpio_set_function(unsigned int pin, unsigned int function) {
   if ((pin < GPIO_PIN_FIRST) || (pin > GPIO_PIN_LAST)){
     ;
   }
   else if ((function < 0) || (function > 7)){
     ;
   }
   else{
     volatile unsigned int * r = assign_pin_to_register(pin);
     int mask = 0b111; //111 (necessary because GPIO functions are represented in 3 bits)
     unsigned int value = *r & ~(mask << ((pin % 10) * 3));
     value = value | (function << ((pin % 10) * 3  ) );
     *r = value;
   }

}

unsigned int gpio_get_function(unsigned int pin) {
  if ((pin < GPIO_PIN_FIRST) | (pin > GPIO_PIN_LAST)){
    return GPIO_INVALID_REQUEST;
  }
  volatile unsigned int * r = assign_pin_to_register(pin);
   int value = * r;
   value = value >> ((pin % 10) * 3);
   return value & 0b111;

}

void gpio_set_input(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}

void gpio_set_output(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

void gpio_write(unsigned int pin, unsigned int value) {
  if ((pin < GPIO_PIN_FIRST) || (pin > GPIO_PIN_LAST)){
    ;
  }
  if (value == 1){
    if (pin < GPIO_PIN32){
      *SET0 = 1 << pin;
    }
    else{
      *SET1 = 1 << (pin-32);
    }
  }
  else if (value == 0){

    if (pin < GPIO_PIN32){
      *CLR0 = 1 << pin;
    }
    else{
      *CLR1 = 1 << (pin-32);
    }
  }
  else{
    ;
  }
}

unsigned int gpio_read(unsigned int pin) {
  if ((pin < GPIO_PIN_FIRST) || (pin > GPIO_PIN_LAST)){
     return GPIO_INVALID_REQUEST;
  }

  if (*LEV0 & (1 << pin) || *LEV1 & (1<< (pin-32))){
     return 1;
  }
  else{
     return 0;
  }

}
