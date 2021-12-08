#pragma once

#include <arduino.h>

#define WHEEL_COUNT 4

#define UPDATE_FREQUENCY 500.0;

#define LED_PIN 13

#define BACK_LEFT_SERVO_DIRECTION_PIN 3
#define BACK_LEFT_SERVO_PULSE_PIN 4

#define BACK_RIGHT_SERVO_DIRECTION_PIN 5
#define BACK_RIGHT_SERVO_PULSE_PIN 6

#define FRONT_LEFT_SERVO_DIRECTION_PIN 7
#define FRONT_LEFT_SERVO_PULSE_PIN 8

#define FRONT_RIGHT_SERVO_DIRECTION_PIN 9
#define FRONT_RIGHT_SERVO_PULSE_PIN 10

namespace MecanumRobot{

    void init();
    void loop();

}







