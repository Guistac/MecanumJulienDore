#include "MecanumRobot.h"
#include "ServoMotor.h"
#include "RadioRemote.h"
#include "ControlLogic.h"

namespace MecanumRobot{

    ServoMotor servoMotors[WHEEL_COUNT] = {
        ServoMotor(FRONT_LEFT_SERVO_PULSE_PIN, FRONT_LEFT_SERVO_DIRECTION_PIN),
        ServoMotor(FRONT_RIGHT_SERVO_PULSE_PIN, FRONT_RIGHT_SERVO_DIRECTION_PIN),
        ServoMotor(BACK_LEFT_SERVO_PULSE_PIN, BACK_LEFT_SERVO_DIRECTION_PIN),
        ServoMotor(BACK_RIGHT_SERVO_PULSE_PIN, BACK_RIGHT_SERVO_DIRECTION_PIN)
    };

    typedef void (*isrFunction)();

    isrFunction isrs[WHEEL_COUNT] = {
        []{ ServoMotor::timedFunction(servoMotors[0]); },
        []{ ServoMotor::timedFunction(servoMotors[1]); },
        []{ ServoMotor::timedFunction(servoMotors[2]); },
        []{ ServoMotor::timedFunction(servoMotors[3]); }
    };



    void init(){
        Serial.begin(9600);
        RadioRemote::init();
        for(int i = 0; i < WHEEL_COUNT; i++){
            servoMotors[i].init();
            servoMotors[i].timer.begin(isrs[i], 100);
        }
        servoMotors[0].b_invert = true;
        servoMotors[1].b_invert = false;
        servoMotors[2].b_invert = true;
        servoMotors[3].b_invert = false;
        pinMode(LED_PIN, OUTPUT);
    }



    void onUpdate(){
        RadioRemote::read();
        
        if(RadioRemote::rpmMinusButton->isLongPressed() || RadioRemote::rpmPlusButton->isLongPressed()) ControlLogic::resetRotationCenter();
        else if(RadioRemote::rpmPlusButton->isPressed()) ControlLogic::selectPreviousRotationCenter();
        else if(RadioRemote::rpmMinusButton->isPressed()) ControlLogic::selectNextRotationCenter();

        ControlLogic::setXVelocity(RadioRemote::xAxisStick->getValue());
        ControlLogic::setYVelocity(RadioRemote::yAxisStick->getValue());
        ControlLogic::setRotationalVelocity(RadioRemote::zAxisStick->getValue());
        ControlLogic::update();

        for(int i = 0; i < WHEEL_COUNT; i++){
            servoMotors[i].setVelocity(ControlLogic::wheelVelocity[i]); 
        }
    }



    uint32_t lastUpdateTime_microseconds = 0;
    uint32_t updateInterval_microseconds = 1000000.0 / UPDATE_FREQUENCY;

    uint32_t lastLedUpdateTime_milliseconds = 0;
    uint32_t ledUpdateInterval_milliseconds = 250;
    bool ledState = false;



    void loop(){
        uint32_t now_microseconds = micros();
        if(now_microseconds > lastUpdateTime_microseconds + updateInterval_microseconds){
            lastUpdateTime_microseconds = now_microseconds;
            onUpdate();
        }
        uint32_t now_milliseconds = millis();
        if(now_milliseconds > lastLedUpdateTime_milliseconds + ledUpdateInterval_milliseconds){
            lastLedUpdateTime_milliseconds = now_milliseconds;
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
        }
    }

}
