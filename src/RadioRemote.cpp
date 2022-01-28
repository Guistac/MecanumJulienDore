#include "RadioRemote.h"

namespace RadioRemote{

    //======== public data ========

    Switch* optionSwitch = new Switch(OPTION_PIN);
    Switch* rabbitSwitch = new Switch(SPEED_PIN);

    Button* rpmPlusButton = new Button(RPM_POSITIVE_PIN);
    Button* rpmMinusButton = new Button(RPM_NEGATIVE_PIN);

    Button* motorStartButton = new Button(MOTOR_START_PIN);
    Button* motorStopButton = new Button(MOTOR_STOP_PIN);

    Joystick* xAxisStick = new Joystick(NEGATIVE_X_AXIS_PIN, POSITIVE_X_AXIS_PIN);
    Joystick* yAxisStick = new Joystick(NEGATIVE_Y_AXIS_PIN, POSITIVE_Y_AXIS_PIN);
    Joystick* zAxisStick = new Joystick(NEGATIVE_Z_AXIS_PIN, POSITIVE_Z_AXIS_PIN);

    void init(){
        analogReadAveraging(32);
        rpmPlusButton->init();
        rpmMinusButton->init();
        rabbitSwitch->init();
        optionSwitch->init();
        motorStopButton->init();
        motorStartButton->init();
        xAxisStick->init();
        yAxisStick->init();
        zAxisStick->init();
        reset();
    }


    void read(){
        optionSwitch->update();
        rabbitSwitch->update();
        rpmMinusButton->update();
        rpmPlusButton->update();
        motorStopButton->update();
        motorStartButton->update();
        xAxisStick->update();
        yAxisStick->update();
        zAxisStick->update();

        //#define PRINT_DEBUG_REMOTE
        #ifdef PRINT_DEBUG_REMOTE
        Serial.println("---Radio Remote-----------------------------------------");
        Serial.printf("RPM+: %s %s\n", rpmPlusButton->isPressed() ? "PRESS !" : (rpmPlusButton->isDown() ? "pressed" : "not pressed"), rpmPlusButton->isLongPressed() ? "(Long Pressed)" : "");
        Serial.printf("RPM-: %s %s\n", rpmMinusButton->isPressed() ? "PRESS !" : (rpmMinusButton->isDown() ? "pressed" : "not pressed"), rpmMinusButton->isLongPressed() ? "(Long Pressed)" : "");
        Serial.printf("Option:  %s\n", optionSwitch->isFlipped() ? "FLIPPED" : (optionSwitch->isOn() ? "On" : "Off"));
        Serial.printf("Rabbit:  %s\n", rabbitSwitch->isFlipped() ? "FLIPPED" : (rabbitSwitch->isOn() ? "On" : "Off"));
        Serial.printf("Motor Start+: %s %s\n", motorStopButton->isPressed() ? "PRESS !" : (motorStopButton->isDown() ? "pressed" : "not pressed"), motorStopButton->isLongPressed() ? "(Long Pressed)" : "");
        Serial.printf("Motor Stop-: %s %s\n", motorStartButton->isPressed() ? "PRESS !" : (motorStartButton->isDown() ? "pressed" : "not pressed"), motorStartButton->isLongPressed() ? "(Long Pressed)" : "");
        Serial.printf("X: %.3f  %.3f\n", xAxisStick->getValue(), xAxisStick->getRawValue());
        Serial.printf("Y: %.3f  %.3f\n", yAxisStick->getValue(), yAxisStick->getRawValue());
        Serial.printf("Z: %.3f  %.3f\n", zAxisStick->getValue(), zAxisStick->getRawValue());
        #endif
    }


    void reset(){
        optionSwitch->reset();
        rabbitSwitch->reset();
        rpmMinusButton->reset();
        rpmPlusButton->reset();
        xAxisStick->reset();
        yAxisStick->reset();
        zAxisStick->reset();
    }

}







Button::Button(int p) : pin(p) {}

void Button::init(){
    pinMode(pin, INPUT);
}

void Button::update(){
    bool b_previousReading = b_reading;
    b_reading = digitalRead(pin);

    uint32_t now_milliseconds = millis();

    if(b_reading && !b_previousReading){
        pressTime_milliseconds = now_milliseconds;
    }else if(b_reading && b_previousReading){
        //regular press detection
        b_down = false;
        b_pressed = false;
        if(now_milliseconds - pressTime_milliseconds > pressValidationDelay_milliseconds){
            b_down = true;
            if(!b_wasPressed){
                b_pressed = true;
                b_wasPressed = true;
            }
        }
        //long press detection
        if(now_milliseconds - pressTime_milliseconds > longPressValidationDelay_milliseconds){
            b_longPressed = true;
        }
    }else{
        b_pressed = false;
        b_longPressed = false;
        b_wasPressed = false;
        b_down = false;
    }
}

void Button::reset(){
    b_reading = false;
    b_pressed = false;
    b_wasPressed = false;
    b_down = false;
    b_longPressed = false;
    pressTime_milliseconds = 0;
}

bool Button::isPressed(){
    return b_pressed;
}

bool Button::isDown(){
    return b_down;
}

bool Button::isLongPressed(){
    return b_longPressed;
}

Switch::Switch(int p) : pin(p) {}

void Switch::init(){
    pinMode(pin, INPUT);
}

void Switch::update(){

    bool b_previousReading = b_reading;
    b_reading = digitalRead(pin);

    uint32_t now_milliseconds = millis();

    if(b_previousReading != b_reading){
        b_flipState = b_reading;
        flipTime_milliseconds = now_milliseconds;
        b_flipped = false;
    }
    else if(now_milliseconds - flipTime_milliseconds > flipValidationDelay_milliseconds){
        //if output has not changed since timer start
        if(b_flipState == b_reading){
            b_flipped = false;
            if(b_isOn != b_reading) b_flipped = true;
            b_isOn = b_reading;
        }
    }
}

void Switch::reset(){
    b_reading = false;
    b_isOn = false;
    b_flipState = false;
    b_flipped = false;
    flipTime_milliseconds = 0;
}

bool Switch::isOn(){
    return b_isOn;
}

bool Switch::isOff(){
    return !b_isOn;
}

bool Switch::isFlipped(){
    return b_flipped;
}



Joystick::Joystick(int nPin, int pPin) : negativePin(nPin), positivePin(pPin) {}

void Joystick::init(){
    pinMode(negativePin, INPUT);
    pinMode(positivePin, INPUT);
}

void Joystick::update(){
    rawValue = ((float)analogRead(positivePin) / 1024.0) - (analogRead(negativePin) / 1024.0);
    smoothedValue = filter * smoothedValue + (1.0f - filter) * rawValue;
    if(abs(smoothedValue) < threshold) outputValue = 0.0;
    else {
        outputValue = map(abs(smoothedValue), threshold, 1.0, 0.0, 1.0);
        if(smoothedValue < 0.0) outputValue *= -1.0;
    }
}

void Joystick::reset(){
    rawValue = 0.0;
    smoothedValue = 0.0;
    outputValue = 0.0;
}

float Joystick::getRawValue(){
    return rawValue;
}

float Joystick::getValue(){
    return outputValue;
}