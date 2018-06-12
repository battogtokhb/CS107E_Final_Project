#include "gpio.h"
#include "printf.h"
#include "timer.h"
#include "lock.h"

void main(void)
{
  stepper_init();
  test_full_unlock();
}

void test_motor_2() {
  while (1) {
    rotate(10, CW, MOTOR_2);
    rotate(10, CCW, MOTOR_2);
  }
}

void test_30_steps_CW_motor_1() {
  rotate(10, CW, MOTOR_1);
  // timer_delay_ms(1000);
  // rotate(10, CW, MOTOR_1);
  // timer_delay_ms(1000);
  // rotate(10, CW, MOTOR_1);
}

void test_half_rotation() {
  // half_rotate(5,  CW, MOTOR_1); DEFUNCT FUNCTION
}

// 8 30 20
void test_one_rotation() {
  rotate(80, 0, MOTOR_1);
  rotate(40-8,  0, MOTOR_1);
  rotate(40,  1, MOTOR_1);
  rotate(22,  1, MOTOR_1);
  rotate(10,  0, MOTOR_1);
}

void test_unlock_with_unknown_val() {
  unlock_lock(8, 30, -1);
}

void test_full_unlock() {
  unlock_lock(37, 23, 9);
}
