#include "MecanumRobot.h"
#include "ServoMotor.h"
#include "RadioRemote.h"

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
        
        for(int i = 0; i < WHEEL_COUNT; i++){
            if(RadioRemote::rabbitSwitch) servoMotors[i].setVelocity(RadioRemote::yAxis * ServoMotor::maxVelocity_rps);    
            else servoMotors[i].setVelocity(RadioRemote::yAxis * 0.5);    
        }
        
        //#define MOTOR_VEL_PRINT
        #ifdef MOTOR_VEL_PRINT
        Serial.printf("--------------------\n");
        for(int i = 0; i < 4; i++){
            ServoMotor& motor = servoMotors[i];
            Serial.printf("%v: %.3f  %.3f %s\n", motor.targetVelocity_rps, motor.actualVelocity_rps, motor.b_stopped ? "Stopped" : "Moving");
        }
        #endif
        
        //update robot vectors
        //calculate new velocities for each wheel
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
