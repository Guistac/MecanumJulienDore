#include <arduino.h>

#define ANALOG_RESOLUTION 12

#define NEGATIVE_X_AXIS_PIN A0
#define POSITIVE_X_AXIS_PIN A1

#define NEGATIVE_Y_AXIS_PIN A2
#define POSITIVE_Y_AXIS_PIN A3

#define NEGATIVE_Z_AXIS_PIN A4
#define POSITIVE_Z_AXIS_PIN A5

#define RPM_POSITIVE_PIN 20
#define RPM_NEGATIVE_PIN 21
#define OPTION_PIN 22
#define SPEED_PIN 23

namespace RadioRemote {

    void init();
    void read();
    void reset();

    extern float xAxis;
    extern float yAxis;
    extern float zAxis;

    extern bool rpmPlusToggle;
    extern bool rpmMinusToggle;
    extern bool optionSwitch;
    extern bool rabbitSwitch;
}