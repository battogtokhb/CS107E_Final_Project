#include "gpio.h"
#include "printf.h"
#include "timer.h"
#include "lock.h"

void main(void)
{
  stepper_init();
  while (1) {
    //half_rotate(5,  CW, MOTOR_1);
    // rotate(80, 0, MOTOR_1);
    // rotate(40-8,  0, MOTOR_1);
    // rotate(40,  1, MOTOR_1);
    // rotate(22,  1, MOTOR_1);
    // rotate(10,  0, MOTOR_1);
    // unlock_lock(8, 30, -1);
    rotate(10, CW, MOTOR_2);
    rotate(10, CCW, MOTOR_2);
     // 8 30 20

  }
}
