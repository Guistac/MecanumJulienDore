#include "MecanumRobot.h"
#include "ServoMotor.h"
#include "RadioRemote.h"
#include "ControlLogic.h"
#include "Signal.h"

namespace MecanumRobot{

    ServoMotor servoMotors[WHEEL_COUNT] = {
        ServoMotor(FRONT_LEFT_SERVO_PULSE_PIN, FRONT_LEFT_SERVO_DIRECTION_PIN, FRONT_LEFT_ENABLED_PIN, FRONT_LEFT_FAULT_PIN),
        ServoMotor(FRONT_RIGHT_SERVO_PULSE_PIN, FRONT_RIGHT_SERVO_DIRECTION_PIN, FRONT_RIGHT_ENABLED_PIN, FRONT_RIGHT_FAULT_PIN),
        ServoMotor(BACK_LEFT_SERVO_PULSE_PIN, BACK_LEFT_SERVO_DIRECTION_PIN, BACK_LEFT_ENABLED_PIN, BACK_LEFT_FAULT_PIN),
        ServoMotor(BACK_RIGHT_SERVO_PULSE_PIN, BACK_RIGHT_SERVO_DIRECTION_PIN, BACK_RIGHT_ENABLED_PIN, BACK_RIGHT_FAULT_PIN)
    };

    typedef void (*isrFunction)();

    isrFunction isrs[WHEEL_COUNT] = {
        []{ ServoMotor::timedFunction(servoMotors[0]); },
        []{ ServoMotor::timedFunction(servoMotors[1]); },
        []{ ServoMotor::timedFunction(servoMotors[2]); },
        []{ ServoMotor::timedFunction(servoMotors[3]); }
    };

    InputSignal emergencyStopClearSignal(EMERGENCY_STOP_PIN, 100); //50ms debounce

    OutputSignal enablePowerStageSignal(ENABLE_ALL_POWER_STAGES_PIN);
    OutputPulseSignal resetAllFaultsSignal(RESET_ALL_FAULTS_PIN, false, 50); //default low, 50ms length

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
        
        emergencyStopClearSignal.init();
        enablePowerStageSignal.init(false); 
        resetAllFaultsSignal.init();

        ControlLogic::setSnailMode();
        ControlLogic::setRelativeMode();
    }


    bool isEmergencyStopPressed(){ return !emergencyStopClearSignal.getState(); }


    void requestEnablePowerStages(){ enablePowerStageSignal.set(true); }

    void requestDisablePowerStages(){ enablePowerStageSignal.set(false); }

    bool isRequestingPowerStageEnable(){ return enablePowerStageSignal.getState(); }

    void resetAllMotors(){ for(int i = 0; i < 4; i++) servoMotors[i].reset(); }
    
    void resetAllFaults(){ resetAllFaultsSignal.pulse(); }

    bool isResettingFaults(){ return resetAllFaultsSignal.isBusy(); }


    bool hasAnyServoMotorFault(){
        for(int i = 0; i < 4; i++) if(servoMotors[i].hasFault()) return true;
        return false;
    }

    bool areAllServoMotorsEnabled(){
        for(int i = 0; i < 4; i++) if(!servoMotors[i].isEnabled()) return false;
        return true;
    }


    bool b_booted = false;
    bool b_faults = false;
    bool b_allEnabled = false;

    void onUpdate(){

        //read remote inputs
        RadioRemote::read();

        //update input signals
        emergencyStopClearSignal.update();
        for(int i = 0; i < 4; i++) servoMotors[i].updateSignals();

        if(emergencyStopClearSignal.changed() && emergencyStopClearSignal.getState()) Serial.printf("update ! %i\n", millis());

        //wait for remote controls to settle before allowing any motion
        if(!b_booted && RadioRemote::xAxisStick->getValue() == 0.0 && RadioRemote::yAxisStick->getValue() == 0.0 && RadioRemote::zAxisStick->getValue() == 0.0){
            b_booted = true;
            ControlLogic::reset();
        }
        
        b_faults = hasAnyServoMotorFault();
        b_allEnabled = areAllServoMotorsEnabled();
        
        //if not all servos are enabled and estop is not pressed
        //if there are errors try to clear them
        //else try to enable power stage
        if(!b_allEnabled && !isEmergencyStopPressed()){

            if(b_faults && !isResettingFaults()){
                resetAllFaults();
                ControlLogic::reset();
                resetAllMotors();
            }else if(!b_faults && b_booted){
                requestEnablePowerStages();
                ControlLogic::reset();
                resetAllMotors();
            }
        }
        //else if all
        else if(isRequestingPowerStageEnable() && (isEmergencyStopPressed() || b_faults || !b_allEnabled)){
            requestDisablePowerStages();
            ControlLogic::reset();
            resetAllMotors();
        }
        
        //digitalWrite(LED_PIN, isRequestingPowerStageEnable());

        //apply remote setttings to control logic
        if(RadioRemote::rabbitSwitch->isFlipped()){
            if(RadioRemote::rabbitSwitch->isOn()) ControlLogic::setRabbitMode();
            else ControlLogic::setSnailMode();
        }

        //if(RadioRemote::rpmMinusButton->isLongPressed() || RadioRemote::rpmPlusButton->isLongPressed()) ControlLogic::resetRotationCenter();
        //else if(RadioRemote::rpmPlusButton->isPressed()) ControlLogic::selectPreviousRotationCenter();
        //else if(RadioRemote::rpmMinusButton->isPressed()) ControlLogic::selectNextRotationCenter();

        //apply remote motion requests to control logic
        ControlLogic::setXVelocityNormalized(RadioRemote::xAxisStick->getValue());
        ControlLogic::setYVelocityNormalized(RadioRemote::yAxisStick->getValue());
        ControlLogic::setRotationalVelocityNormalized(RadioRemote::zAxisStick->getValue());

        //update control logic
        if(b_allEnabled && b_booted) ControlLogic::update();
        //else keep control logic reset
        else {
            ControlLogic::reset();
            resetAllMotors();
        }

        //set output signals
        resetAllFaultsSignal.update();

        //set motor velocities
        if(b_booted) for(int i = 0; i < WHEEL_COUNT; i++) servoMotors[i].setVelocity(ControlLogic::wheelVelocity[i]);

        //#define GENERAL_ROBOT_PRINT
        #ifdef GENERAL_ROBOT_PRINT
            Serial.printf("---Mecanum Robot Info---------------------------\n");
            Serial.printf("Power Stage Request: %s\n", isRequestingPowerStageEnable() ? "Enabling" : "Disabling");
            Serial.printf("Power Stage State: %s\n", b_allEnabled ? "Enabled" : "Disabled");
            Serial.printf("EStop: %s\n", isEmergencyStopPressed() ? "Down" : "Clear");
            Serial.printf("FL-Servo Enabled: %s  Fault: %s\n", servoMotors[0].isEnabled() ? "Yes" : "No", servoMotors[0].hasFault() ? "FAULT" : "OK");
            Serial.printf("FR-Servo Enabled: %s  Fault: %s\n", servoMotors[1].isEnabled() ? "Yes" : "No", servoMotors[1].hasFault() ? "FAULT" : "OK");
            Serial.printf("BL-Servo Enabled: %s  Fault: %s\n", servoMotors[2].isEnabled() ? "Yes" : "No", servoMotors[2].hasFault() ? "FAULT" : "OK");
            Serial.printf("BR-Servo Enabled: %s  Fault: %s\n", servoMotors[3].isEnabled() ? "Yes" : "No", servoMotors[3].hasFault() ? "FAULT" : "OK");
        #endif
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
            //digitalWrite(LED_PIN, false);
        }
    }

}
