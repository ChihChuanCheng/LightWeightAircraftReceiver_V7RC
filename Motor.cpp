#include "Options.h"
#include <Servo.h>

/* esp07 */
#define RUDDER     4 // D5
#define THROTTLE  12 // D6

const uint8_t full_thr = 255;

Servo servoRudder;

void init_motor() {
  pinMode(THROTTLE, OUTPUT);

  /* initialize servo */
  servoRudder.attach(RUDDER);
  servoRudder.write(90);

  /* initialize throttle */
  analogWrite(THROTTLE, 0);
  
  /* set a new input range for throttle */
  analogWriteRange(full_thr);
}

void motor_control(uint8_t rudder, uint8_t throttle) {
#ifdef __LOG__
  Serial.print("Rudder: ");
  Serial.print(rudder);
  Serial.print(" throttle: ");
  Serial.println(throttle);
#endif /* __LOG__ */
  servoRudder.write(rudder);
  analogWrite(THROTTLE, throttle);
}
