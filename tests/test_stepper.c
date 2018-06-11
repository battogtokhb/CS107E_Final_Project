#include "gpio.h"
#include "printf.h"
#include "timer.h"
#include "lock.h"

void main(void)
{
  stepper_init();
//  while (1) {
    //half_rotate(5,  CW, MOTOR_1);
    rotate(1,  CW, MOTOR_1);

//  }
}
