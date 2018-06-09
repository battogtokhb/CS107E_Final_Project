#include "gpio.h"
#include "printf.h"
#include "timer.h"
#include "lock.h"

void main(void)
{
  stepper_init();
  while (1) {
    rotate(5, PHASES_PER_STEP * STEPS_PER_INCREMENT, CW, MOTOR_1)
    rotate(5, PHASES_PER_STEP * STEPS_PER_INCREMENT, CCW, MOTOR_1)
  }
}
