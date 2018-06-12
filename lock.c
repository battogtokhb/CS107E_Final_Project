// MARK:- INCLUDES
#include "lock.h"
#include "gpio.h"
#include "timer.h"
#include "printf.h"
#include "malloc.h"


// MARK:- CONSTANTS
static const int ROTATIONS_TO_RESET = 2;
static const int STANDARD_DELAY = 25;


static int BACKWARD_STEPS[4][4] = { {  1, 0 , 1,0  },
                            { 0, 1, 1,0} ,
                            {0, 1, 0, 1} ,
                            {1, 0, 0, 1} };


static int FORWARD_STEPS[4][4] = { {  1, 0, 0, 1  },
                                      { 0, 1, 0, 1} ,
                                      {0, 1, 1,0} ,
                                      {1, 0 , 1,0} };

static int * FORWARD_INDEX_1 ;
static int * BACKWARD_INDEX_1  ;

static int * FORWARD_INDEX_2 ;
static int * BACKWARD_INDEX_2  ;


// MARK:- GLOBALS



// MARK:- HELPER FUNCTIONS

/*
 * Moves the pointer to zero.
 * Steps is the amount of steps away from zero.
 */
void reset_lock_to_zero(int steps, int clockwise) {
  rotate(steps, clockwise, MOTOR_1);
}

/*
 * Resets the lock after trying to open it.
 */
void reset_lock(int motor) {
  for (int i = 0; i < ROTATIONS_TO_RESET; i++) {
    rotate(INCREMENTS_PER_LOCK, CCW, MOTOR_1);
  }
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




  *FORWARD_INDEX_1 = 1;
  *BACKWARD_INDEX_1 = 1;

  *FORWARD_INDEX_2 = 1;
  *BACKWARD_INDEX_2 = 1;

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
    gpio_write(MOTOR_1_B2, b_2);
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

void clear_pins(int motor){
  set_step(0,0,0,0,motor);
}


void step(int clockwise, int motor) {
  int delay = STANDARD_DELAY;
  int (*reference) [4] ;
  int * index;
  if (clockwise){
    if (motor == MOTOR_1){
        index = FORWARD_INDEX_1;
    }
    else{
      index = FORWARD_INDEX_2;
    }

    reference = FORWARD_STEPS;
  }
  else{
    if (motor == MOTOR_1){
        index = BACKWARD_INDEX_1;
    }
    else {
      index = BACKWARD_INDEX_2;
    }

    reference = BACKWARD_STEPS;

  }
  int n = 0;
  while (n < STEPS_PER_INCREMENT){
    if (*index== 4){
      *index=0;
    }
    int * sequence;
    sequence = reference[*index];
    set_step(sequence[0], sequence[1], sequence[2], sequence[3],  motor);
    timer_delay_ms(delay);

    *index = *index + 1;
    n++;
  }
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
  */

 void rotate(int increments, int clockwise, int motor){
   for (int i = 0; i < increments; i++){
       step(clockwise, motor);
   }
 }

 /*
  * Opens the lock by rotating the stepper motor such that it tugs on the lock.
  *
  * Returns 1 if successful. 0 if not.
  */
 int open_lock(int motor) {
   // go back same amount of times
   return 0;
 }

 /*
  * Unlocks the given master lock given a specified code.
  *
  * Assumes a starting position of 0.
  * Returns 1 if successful. Returns 0 if not.
  *
  * For later: might want to integrate unlock_lock(...) and break_lock(...) as
  * one function with a variable number of arguments.
  */
 int unlock_lock(int first, int second, int third) {
   int first_final = INCREMENTS_PER_LOCK;
   int second_final = INCREMENTS_PER_LOCK;
   int third_final = INCREMENTS_PER_LOCK;
   (first == -1) ? (first = 0) : (first_final = first + 1);
   (second == -1) ? (second = 0) : (second_final = second + 1);
   (third == -1) ? (third = 0) : (third_final = third + 1);
   int opened = 0;
   for (int i = first; i < first_final; i++) {
     for (int j = second; j < second_final; j++) {
       for (int k = third; k < third_final; k++) {
         if (k >= j) continue;
         reset_lock(MOTOR_1);
         rotate(INCREMENTS_PER_LOCK - i,CCW, MOTOR_1);
         rotate(INCREMENTS_PER_LOCK, CW, MOTOR_1);
         rotate(j - i, CW, MOTOR_1);
         rotate(j - k, CCW, MOTOR_1);
         int opened = open_lock(MOTOR_1);
         reset_lock_to_zero(INCREMENTS_PER_LOCK - k, CW);
         if (opened) return opened;
       }
     }
   }
   return opened;
 }
