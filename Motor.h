#pragma once

void init_motor();
void reset_servos();

void outofcontrol_recovery(uint32_t ooc_time);
void connectionrecovery();

void motor_control(uint8_t rudder, uint8_t throttle);
