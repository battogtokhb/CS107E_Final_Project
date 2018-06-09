#include "gpio.h"
#include "printf.h"
#include "timer.h"

static const A1 = 26;
static const A2 = 19;
static const B1 = 13;
static const B2 = 6;

void set_stepper_pins(void){
  gpio_set_output(A1);
  gpio_set_output(A2);
  gpio_set_output(B1);
  gpio_set_output(B2);
}

void set_step(int a_1, int a_2, int b_1, int b_2){
  gpio_write(A1, a_1);
  gpio_write(A2, a_2);
  gpio_write(B1, b_1);
  gpio_write(B2, b_2);
}

void forward(int steps, int delay){
  int i = 0;
  delay = delay / 4;
  while (steps > i){
    set_step(1,0,1,0);
    timer_delay_ms(delay);
    set_step(0,1,1,0);
    timer_delay_ms(delay);
    set_step(0,1,0,1);
    timer_delay_ms(delay);
    set_step(1,0,0,1);
    timer_delay_ms(delay);
    i++;
  }
}

void backward(int steps, int delay){
  int i = 0;
  while (steps > i){
    set_step(1,0,0,1);
    timer_delay_ms(delay);
    set_step(0,1,0,1);
    timer_delay_ms(delay);
    set_step(0,1,1,0);
    timer_delay_ms(delay);
    set_step(1,0,1,0);
    timer_delay_ms(delay);
    i++;
  }
}



void main(void)
{
  set_stepper_pins();
  while (1){
    forward(5, 100);
    backward(5, 100);

  }


}
