#pragma once

#include <arduino.h>

#define PULSES_PER_WHEEL_REVOLUTION 6400
#define MAX_PULSE_FREQUENCY 500000.0
#define MIN_PULSE_FREQUENCY 10.0
#define ACCELERATION_RPS2 1.0

class ServoMotor{
public:

    ServoMotor(uint8_t pulse, uint8_t dir) : pulsePin(pulse), directionPin(dir) {}

    uint8_t pulsePin = -1;
    uint8_t directionPin = -1;

    void init();
    void reset();

    void setVelocity(float vel_rps);
    double targetVelocity_rps;

    //profile generator
    bool b_invert = false;
    double actualVelocity_rps;
    static float maxVelocity_rps;
    static float minVelocity_rps;

    //interrupt service routine
    IntervalTimer timer;
    static void timedFunction(ServoMotor& servoMotor);
    bool b_pulseState;
    bool b_directionState;
    bool b_stopped = true;
    uint32_t previousUpdateTime_microseconds = 0;

};