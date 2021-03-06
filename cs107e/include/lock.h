#ifndef LOCK_H
#define LOCK_H

/*
 * Interface to the motors connected to a Raspberry Pi for opening a standard
 * master lock.
 *
 * Author: Bilguunzaya (Zaya) Battogtokh <bilguunzaya.batt@stanford.edu>
 * Author: Josh Wolff <jw1@stanford.edu>
 * Date: June 9 2018
 */

/* NOTE:
 * A step is defined by a step on the motor.
 * An increment is defined by an increment on the lock.
 */

#define INCREMENTS_PER_LOCK 40
// The total steps per increment on the clock. Varies depending on motor, voltage, etc.
#define STEPS_PER_INCREMENT 5


// Total phases per step. Depends on type of motor. This assumes a bipolar stepper motor.
#define PHASES_PER_STEP 4

/*
 * These enumerated values establish the pins for connection on the Pi.
 * These pins route directly to a motor.
 *
 */
enum {
  MOTOR_1_A1 = 26,
  MOTOR_1_A2 = 19,
  MOTOR_1_B1 = 13,
  MOTOR_1_B2 = 6,

  MOTOR_2_A1 = 21,
  MOTOR_2_A2 = 20,
  MOTOR_2_B1 = 16,
  MOTOR_2_B2 = 12,
};

/*
 * These enumerated values define the motors used.
 * MOTOR_1 turns the lock.
 * MOTOR_2 opens the lock.
 */
enum {
  MOTOR_1 = 1,
  MOTOR_2 = 2,
};

/*
 * These enumerated values define values for clockwise and
 * counterclockwise rotation.
 */
enum {
  CCW = 0,
  CW = 1,
};

/*
 * Initialize the stepper code module.
 */
void stepper_init(void);

/*
 * Rotate the stepper motor by the specified amount of steps and with the
 * specified time.
 *
 * Takes approximately one "delay" for one step. Delay must be a multiple
 * of PHASES_PER_STEP.
 *
 * Clockwise is a 1 or 0 to indicate direction of rotation.
 */
void rotate(int increments, int clockwise, int motor);

/*
 * Opens the lock by rotating the stepper motor such that it tugs on the lock.
 *
 * Returns 1 if successful. 0 if not.h
 */
int open_lock(int motor);

/*
 * Unlocks the given master lock given a specified code.
 *
 * Put -1 for each unknown code.
 *
 * Assumes a starting position of 0.
 */
int unlock_lock(int first, int second, int third);


#endif
