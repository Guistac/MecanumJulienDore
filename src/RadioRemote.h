#include <arduino.h>

#define ANALOG_RESOLUTION 12

#define NEGATIVE_X_AXIS_PIN A0
#define POSITIVE_X_AXIS_PIN A1
#define NEGATIVE_Y_AXIS_PIN A2
#define POSITIVE_Y_AXIS_PIN A3
#define NEGATIVE_Z_AXIS_PIN A6
#define POSITIVE_Z_AXIS_PIN A7
#define RPM_POSITIVE_PIN 38
#define RPM_NEGATIVE_PIN 39
#define OPTION_PIN 40
#define SPEED_PIN 41

//new buttons (subject to change)
#define MOTOR_START_PIN 36
#define MOTOR_STOP_PIN 37


class Button{
public:

    Button(int pin);
    void init();
    void update();
    void reset();

    bool isPressed();
    bool isDown();
    bool isLongPressed();

private:

    int pin = -1;
    bool b_reading = false;
    bool b_pressed = false;
    bool b_wasPressed = false;
    bool b_down = false;
    bool b_longPressed = false;
    uint32_t pressTime_milliseconds = 0;
    const uint32_t pressValidationDelay_milliseconds = 50;
    const uint32_t longPressValidationDelay_milliseconds = 500;
};

class Switch{
public:

    Switch(int pin);
    void init();
    void update();
    void reset();

    bool isOn();
    bool isOff();
    bool isFlipped();

private:

    int pin = -1;
    bool b_reading = false;
    bool b_isOn = false;
    bool b_flipState = false;
    bool b_flipped = false;
    uint32_t flipTime_milliseconds = 0;
    const uint32_t flipValidationDelay_milliseconds = 50;

};

class Joystick{
public:

    Joystick(int nPin, int pPin);
    void init();
    void update();

    float getRawValue();
    float getValue();

    void reset();

    int negativePin = -1;
    int positivePin = -1;

private:

    const float filter = 0.90;
    const float threshold = 0.03;

    float rawValue = 0.0;
    float smoothedValue = 0.0;
    float outputValue = 0.0;

};



namespace RadioRemote {

    void init();
    void read();
    void reset();

    extern Joystick* xAxisStick;
    extern Joystick* yAxisStick;
    extern Joystick* zAxisStick;

    extern Button* rpmPlusButton;
    extern Button* rpmMinusButton;
    extern Switch* optionSwitch;
    extern Switch* rabbitSwitch;

    extern Button* motorStartButton;
    extern Button* motorStopButton;
}