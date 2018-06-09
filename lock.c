// MARK:- INCLUDES
#include "lock.h"

// MARK:- CONSTANTS



// MARK:- GLOBALS



// MARK:- HELPER FUNCTIONS


/*
 * Resets the lock after trying to open it.
 */
void reset_lock(void) {

}

/*
 * Sets the motor pins as outputs. Pin numbers declared in lock.h.
 * Called to initialize the stepper motors.
 */
void set_stepper_pins(void){
  gpio_set_output(MOTOR_1_A1);
  gpio_set_output(MOTOR_1_A2);
  gpio_set_output(MOTOR_1_B1);
  gpio_set_output(MOTOR_1_B2);

  gpio_set_output(MOTOR_2_A1);
  gpio_set_output(MOTOR_2_A2);
  gpio_set_output(MOTOR_2_B1);
  gpio_set_output(MOTOR_2_B2);
}

/*
 * Sets one of the four phases in a single rotation.
 * Returns 1 if successful. Returns 0 if not.
 */
int set_step(int a_1, int a_2, int b_1, int b_2, int motor){
  if (motor == MOTOR_1) {
    gpio_write(MOTOR_1_A1, a_1);
    gpio_write(MOTOR_1_A2, a_2);
    gpio_write(MOTOR_1_B1, b_1);
    gpio_write(MOTOR_2_B2, b_2);
    return 1;
  } else if (motor == MOTOR_2) {
    gpio_write(MOTOR_2_A1, a_1);
    gpio_write(MOTOR_2_A2, a_2);
    gpio_write(MOTOR_2_B1, b_1);
    gpio_write(MOTOR_2_B2, b_2);
    return 1;
  }
  return 0;
}

/*
 * Set-step order for CW: 1010, 0110, 0101, 1001
 * Set-step order for CCW: 1001, 0101, 0110, 1010
 */
void rotate_step(int delay, int clockwise, int motor) {
 delay = delay / PHASES_PER_STEP;
 set_step(1,0, clockwise, !clockwise, motor); // 1010 if CW, 1001 if CCW
 timer_delay_ms(delay);
 set_step(0,1, clockwise, !clockwise, motor); // 0110 if CW, 0101 if CCW
 timer_delay_ms(delay);
 set_step(0,1, !clockwise, clockwise, motor); // 0101 if CW, 0110 if CCW
 timer_delay_ms(delay);
 set_step(1,0, !clockwise, clockwise, motor); // 1001 if CW, 1010 if CCW
 timer_delay_ms(delay);
}


// MARK:- PUBLIC FUNCTIONS DEFINED IN HEADER FILE

 /*
  * Initialize the stepper code module.
  */
 void stepper_init(void) {
   set_stepper_pins();
 }

 /*
  * Rotate the stepper motor by the specified amount of steps and with the
  * specified time. Takes approximately one "delay" for one step.
  * Clockwise is a 1 or 0 to indicate direction of rotation.
  *
  * Delay must be a multiple of PHASES_PER_STEP * STEPS_PER_INCREMENT.
  */
 void rotate(int steps, int delay, int clockwise, int motor) {
   delay = delay / STEPS_PER_INCREMENT;
   for (int i = 0; i < steps; i++) {
     for (int j = 0; j < STEPS_PER_INCREMENT; j++) {
       rotate_step(delay, clockwise, motor);
     }
   }
 }

 /*
  * Opens the lock by rotating the stepper motor such that it tugs on the lock.
  *
  * Returns 1 if successful. 0 if not.
  */
 int open_lock(int motor) {

 }

 /*
  * Unlocks the given master lock given a specified code.
  * Assumes a starting position of 0.
  */
 void unlock_lock(int first, int second, int third) {

   reset_lock();
 }

 /*
  * Breaks into the lock using brute force.
  * Returns the code that worked. Assumes a starting position of 0.
  */
 int break_lock(void) {

   reset_lock();
 }
