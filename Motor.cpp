#include "Options.h"
#include <Servo.h>

/* wemos
#define AILRON      14 //4 // D5
#define ELEVATOR    13 // D1
#define THROTTLELEFT  12 // D6
#define THROTTLERIGHT 16 // D2
*/

/* esp07 */
#define AILRON      4 // D5
#define ELEVATOR    13 // D1
#define THROTTLELEFT  12 // D6
#define THROTTLERIGHT 16 // D2

uint8_t thr = 0;

const uint8_t full_thr = 255;
const float duration = 10000; /* 10 sec */
const uint8_t phase_one_speed = 0.6 * full_thr;
const uint8_t phase_two_speed = 0.4 * full_thr;
const uint8_t phase_three_speed = 0.2 * full_thr;
const uint32_t phase_one_time = 0.12 * duration;
const uint32_t phase_two_time = 0.25 * duration;
const uint32_t phase_three_time = 0.62 * duration;

Servo servoElevator;
Servo servoAilron;

void init_motor() {
  pinMode(THROTTLELEFT, OUTPUT);
  pinMode(THROTTLERIGHT, OUTPUT);

  servoElevator.attach(ELEVATOR);
  servoElevator.write(90);

  servoAilron.attach(AILRON);
  servoAilron.write(90);

  /* initialize throttle */
  analogWrite(THROTTLERIGHT, 0);
  analogWrite(THROTTLELEFT, 0);
  
  /* set a new input range for throttle */
  analogWriteRange(full_thr);
}

void outofcontrol_recovery(uint32_t ooc_time) {
  uint8_t thr_to_set = 0;
  uint8_t thr_to_set_left = 0;
  uint8_t phase_to_set = 0;
  uint8_t rud = 45;

  float speed_diff = 0.8;

  if (phase_one_speed < thr) {
    phase_to_set = 1;
  }
  else if (phase_two_speed < thr) {
    phase_to_set = 2;
  }
  else if (phase_three_speed < thr) {
    phase_to_set = 3;
  }
  else {
    ; /* do nothing */
  }

  switch(phase_to_set)
  {
    case 1:
      if (phase_one_time > ooc_time) {
        thr_to_set = thr;
      }
      else if (phase_two_time > ooc_time) {
        thr_to_set = phase_one_speed;
      }
      else if (phase_three_time > ooc_time) {
        thr_to_set = phase_two_speed;
      }
      else if (duration > ooc_time) {
        thr_to_set = phase_three_speed;
      }
      else {
        thr_to_set = 0;
      }
      break;
    case 2:
      if (phase_two_time > ooc_time) {
        thr_to_set = phase_one_speed;
      }
      else if (phase_three_time > ooc_time) {
        thr_to_set = phase_two_speed;
      }
      else if (duration > ooc_time) {
        thr_to_set = phase_three_speed;
      }
      else {
        thr_to_set = 0;
      }
      break;
    case 3:
      if (phase_three_time > ooc_time) {
        thr_to_set = phase_two_speed;
      }
      else if (duration > ooc_time) {
        thr_to_set = phase_three_speed;
      }
      else {
        thr_to_set = 0;
      }
      break;
    default:
      if (duration > ooc_time) {
        thr_to_set = thr;
      }
      else {
        thr_to_set = 0;
      }
      break;
  }

  thr_to_set_left = thr_to_set * speed_diff;
#ifdef __LOG__
  Serial.printf("THROTTLELEFT: %d\n", thr_to_set_left);
  Serial.printf("THROTTLERIGHT: %d\n", thr_to_set);
#endif /* __LOG__ */
  analogWrite(THROTTLELEFT, thr_to_set_left);
  analogWrite(THROTTLERIGHT, thr_to_set);
}

void motor_control(uint8_t rudder, uint8_t throttle) {
#ifdef __LOG__
  Serial.print("Rudder: ");
  Serial.println(rudder);
  Serial.print("throttle: ");
  Serial.println(throttle);
#endif /* __LOG__ */
  servoAilron.write(rudder);
  analogWrite(THROTTLELEFT, throttle);
}

void reset_servos(){
  servoAilron.write(90);
  servoElevator.write(90);
}
