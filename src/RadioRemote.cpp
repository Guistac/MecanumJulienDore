#include "RadioRemote.h"

namespace RadioRemote{

    //public data

    float xAxis = 0.0;
    float yAxis = 0.0;
    float zAxis = 0.0;

    bool rpmPlusToggle = false;
    bool rpmMinusToggle = false;
    bool optionSwitch = false;
    bool rabbitSwitch = false;

    //private data

    float filter = 0.90;
    float threshold = 0.03;

    float xSmooth = 0.0;
    float ySmooth = 0.0;
    float zSmooth = 0.0;

    void init(){
        pinMode(NEGATIVE_X_AXIS_PIN, INPUT);
        pinMode(POSITIVE_X_AXIS_PIN, INPUT);
        pinMode(NEGATIVE_Y_AXIS_PIN, INPUT);
        pinMode(POSITIVE_Y_AXIS_PIN, INPUT);
        pinMode(NEGATIVE_Z_AXIS_PIN, INPUT);
        pinMode(POSITIVE_Z_AXIS_PIN, INPUT);
        pinMode(RPM_POSITIVE_PIN, INPUT);
        pinMode(RPM_NEGATIVE_PIN, INPUT);
        pinMode(OPTION_PIN, INPUT);
        pinMode(SPEED_PIN, INPUT);

        analogReadAveraging(32);
        reset();
    }

    void read(){

        float xRaw = ((float)analogRead(POSITIVE_X_AXIS_PIN) / 1024.0) - (analogRead(NEGATIVE_X_AXIS_PIN) / 1024.0);
        float yRaw = ((float)analogRead(POSITIVE_Y_AXIS_PIN) / 1024.0) - (analogRead(NEGATIVE_Y_AXIS_PIN) / 1024.0);
        float zRaw = ((float)analogRead(POSITIVE_Z_AXIS_PIN) / 1024.0) - (analogRead(NEGATIVE_Z_AXIS_PIN) / 1024.0);

        xSmooth = filter * xSmooth + (1.0f - filter) * xRaw;
        ySmooth = filter * ySmooth + (1.0f - filter) * yRaw;
        zSmooth = filter * zSmooth + (1.0f - filter) * zRaw;

        if(abs(xSmooth) < threshold) xAxis = 0.0;
        else {
            xAxis = map(abs(xSmooth), threshold, 1.0, 0.0, 1.0);
            if(xSmooth < 0.0) xAxis *= -1.0;
        }

        if(abs(ySmooth) < threshold) yAxis = 0.0;
        else {
            yAxis = map(abs(ySmooth), threshold, 1.0, 0.0, 1.0);
            if(ySmooth < 0.0) yAxis *= -1.0;
        }

        if(abs(zSmooth) < threshold) zAxis = 0.0;
        else {
            zAxis = map(abs(zSmooth), threshold, 1.0, 0.0, 1.0);
            if(zSmooth < 0.0) zAxis *= -1.0;
        }

        rpmPlusToggle = digitalRead(RPM_POSITIVE_PIN);
        rpmMinusToggle = digitalRead(RPM_NEGATIVE_PIN);
        optionSwitch = digitalRead(OPTION_PIN);
        rabbitSwitch = digitalRead(SPEED_PIN);

        #define PRINT_DEBUG_REMOTE
        #ifdef PRINT_DEBUG_REMOTE
        Serial.println("--------------------------------------------");
        Serial.printf("RPM+:%i  RPM-:%i  Option:%i  %s\n", rpmPlusToggle, rpmMinusToggle, optionSwitch, rabbitSwitch ? "Rabbit" : "Snail");
        Serial.printf("X: %.3f  %.3f\n", xAxis, xSmooth);
        Serial.printf("X: %.3f  %.3f\n", yAxis, ySmooth);
        Serial.printf("X: %.3f  %.3f\n", zAxis, zSmooth);
        #endif
    }

    void reset(){
        xAxis = 0.0;
        yAxis = 0.0;
        zAxis = 0.0;
        /*
        nX = 0.0;
        pX = 0.0;
        nY = 0.0;
        pY = 0.0;
        nZ = 0.0;
        pZ = 0.0;
        */
       xSmooth = 0.0;
       ySmooth = 0.0;
       zSmooth = 0.0;
    }


}