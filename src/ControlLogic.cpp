#include "ControlLogic.h"
#include "MecanumRobot.h"

namespace ControlLogic{

    double wheelDiameter_millimeters = 152.4;
    double wheelCircumference_millimeters = PI * wheelDiameter_millimeters;

    double xVelActual_mmPerSec = 0.0;
    double yVelActual_mmPerSec = 0.0;
    double rotVelActual_degPerSec = 0.0;

    double xVelTarg_mmPerSec = 0.0;
    double yVelTarg_mmPerSec = 0.0;
    double rotVelTarg_degPerSec = 0.0;

    double translVelLim_mmPerSec = 0.0;
    double rotVelLim_degPerSec = 0.0;
    double translAcc_mmPerSecSq = 0.0;
    double rotAcc_degPerSecSq = 0.0;

    //====== Rabbit & Snail Modes ======

    double rabbitTranslationVelocityLimit_mmPerSec = 500.0;
    double rabbitRotationVelocityLimit_degPerSec = 80.0;
    double rabbitTranslationAcceleration_mmPerSecSq = 500.0;
    double rabbitRotationAcceleration_degPerSecSq = 500.0;

    double snailTranslationVelocityLimit_mmPerSecond = 200.0;
    double snailRotationVelocityLimit_degPerSecond = 25.0;
    double snailTranslationAcceleration_mmPerSecSq = 250.0;
    double snailRotationAcceleration_degPerSecSq = 250.0;

    void setRabbitMode(){
        translAcc_mmPerSecSq = rabbitTranslationAcceleration_mmPerSecSq;
        rotAcc_degPerSecSq = rabbitRotationAcceleration_degPerSecSq;
        translVelLim_mmPerSec = rabbitTranslationVelocityLimit_mmPerSec;
        rotVelLim_degPerSec = rabbitRotationVelocityLimit_degPerSec;
    }

    void setSnailMode(){
        translAcc_mmPerSecSq = snailTranslationAcceleration_mmPerSecSq;
        rotAcc_degPerSecSq = snailRotationAcceleration_degPerSecSq;
        translVelLim_mmPerSec = snailTranslationVelocityLimit_mmPerSecond;
        rotVelLim_degPerSec = snailRotationVelocityLimit_degPerSecond;
    }

    Vector2 wheelFrictionVector_millimetersPerRevolution[WHEEL_COUNT] = {
        {sin(45.0) * wheelCircumference_millimeters, sin(45.0) * wheelCircumference_millimeters},  //front left
        {-sin(45.0) * wheelCircumference_millimeters, sin(45.0) * wheelCircumference_millimeters},   //front right
        {-sin(45.0) * wheelCircumference_millimeters, sin(45.0) * wheelCircumference_millimeters}, //back left
        {sin(45.0) * wheelCircumference_millimeters, sin(45.0) * wheelCircumference_millimeters}   //back right
    };

    Vector2 wheelPosition_millimeter[WHEEL_COUNT] = {
        {-550.0, 550.0},    //front left
        {550.0, 550.0},     //front right
        {-550.0, -550.0},   //back left
        {550.0, -550.0}     //back right
    };

    double wheelVelocity[4] = {
        0.0,    //front left
        0.0,    //front right
        0.0,    //back left
        0.0     //back right
    };


    const int rotationCenterCount = 9;
    Vector2 rotationCenters[rotationCenterCount] = {
        {0.0, 0.0},         //center
        {-550.0, 550.0},    //front left
        {0.0, 550.0},       //front
        {550.0, 550.0},     //front right
        {550.0, 0.0},       //right
        {550.0, -550.0},    //back right
        {0.0, -550.0},      //back
        {-550.0, -550.0},   //back left
        {-550.0, 0.0}       //left
    };
    int currentRotationCenter = 0;

    void selectNextRotationCenter(){
        if(currentRotationCenter == rotationCenterCount - 1) currentRotationCenter = 0;
        else currentRotationCenter++;
    }

    void selectPreviousRotationCenter(){
        if(currentRotationCenter == 0) currentRotationCenter = rotationCenterCount - 1;
        else currentRotationCenter--;
    }

    void resetRotationCenter(){
        currentRotationCenter = 0;
    }

    void setXVelocityNormalized(float v_n){
        xVelTarg_mmPerSec = v_n * translVelLim_mmPerSec;
    }
    
    void setYVelocityNormalized(float v_n){
        yVelTarg_mmPerSec = v_n * translVelLim_mmPerSec;
    }

    void setRotationalVelocityNormalized(float v_n){
        rotVelTarg_degPerSec = v_n * rotVelLim_degPerSec;
    }

    uint32_t previousUpdateTime_microseconds = 0.0;

    void update(){

        uint32_t now = micros();
        double deltaT_seconds = (double)(now - previousUpdateTime_microseconds) / 1000000.0;
        previousUpdateTime_microseconds = now;

        double deltaVTranslation_millimetersPerSecond = translAcc_mmPerSecSq * deltaT_seconds;

        if(xVelActual_mmPerSec < xVelTarg_mmPerSec){
            xVelActual_mmPerSec += deltaVTranslation_millimetersPerSecond;
            if(xVelActual_mmPerSec > xVelTarg_mmPerSec) xVelActual_mmPerSec = xVelTarg_mmPerSec;
        }else if(xVelActual_mmPerSec > xVelTarg_mmPerSec){
            xVelActual_mmPerSec -= deltaVTranslation_millimetersPerSecond;
            if(xVelActual_mmPerSec < xVelTarg_mmPerSec) xVelActual_mmPerSec = xVelTarg_mmPerSec;
        }

        if(yVelActual_mmPerSec < yVelTarg_mmPerSec){
            yVelActual_mmPerSec += deltaVTranslation_millimetersPerSecond;
            if(yVelActual_mmPerSec > yVelTarg_mmPerSec) yVelActual_mmPerSec = yVelTarg_mmPerSec;
        }else if(yVelActual_mmPerSec > yVelTarg_mmPerSec){
            yVelActual_mmPerSec -= deltaVTranslation_millimetersPerSecond;
            if(yVelActual_mmPerSec < yVelTarg_mmPerSec) yVelActual_mmPerSec = yVelTarg_mmPerSec;
        }

        double deltaVRotation_degreesPerSecond = rotAcc_degPerSecSq * deltaT_seconds;

        if(rotVelActual_degPerSec < rotVelTarg_degPerSec){
            rotVelActual_degPerSec += deltaVRotation_degreesPerSecond;
            if(rotVelActual_degPerSec > rotVelTarg_degPerSec) rotVelActual_degPerSec = rotVelTarg_degPerSec;
        }else if(rotVelActual_degPerSec > rotVelTarg_degPerSec){
            rotVelActual_degPerSec -= deltaVRotation_degreesPerSecond;
            if(rotVelActual_degPerSec < rotVelTarg_degPerSec) rotVelActual_degPerSec = rotVelTarg_degPerSec;
        }

        Vector2 centerOfRotation = rotationCenters[currentRotationCenter];

        for(int i = 0; i < WHEEL_COUNT; i++){
            //start calculation from zero
            wheelVelocity[i] = 0.0;

            //X component velocity
            wheelVelocity[i] += xVelActual_mmPerSec / wheelFrictionVector_millimetersPerRevolution[i].x;

            //Y component velocity
            wheelVelocity[i] += yVelActual_mmPerSec / wheelFrictionVector_millimetersPerRevolution[i].y;

            //Rotational Component velocity

            //wheel position relative to rotation center
            Vector2 relativeWheelPosition;
            relativeWheelPosition.x = wheelPosition_millimeter[i].x - centerOfRotation.x;
            relativeWheelPosition.y = wheelPosition_millimeter[i].y - centerOfRotation.y;

            float rotationRadius = sqrt(sq(relativeWheelPosition.x) + sq(relativeWheelPosition.y));
            float rotationCirclePerimeter = 2.0 * PI * rotationRadius;
            float rotationVectorMagnitude = rotationCirclePerimeter * rotVelActual_degPerSec / 360.0;
            rotationVectorMagnitude *= -1.0; //invert rotation vector to match remote control

            //vector perpendicular to radius of wheel position around rotation center vector
            Vector2 wheelRotationVector;
            wheelRotationVector.x = -relativeWheelPosition.y;
            wheelRotationVector.y = relativeWheelPosition.x;

            //normalize the perpendicular vector
            double normalisationMagnitude = sqrt(sq(wheelRotationVector.x) + sq(wheelRotationVector.y));
            if(normalisationMagnitude != 0.0){
                wheelRotationVector.x /= normalisationMagnitude;
                wheelRotationVector.y /= normalisationMagnitude;

                //set the rotation vector magnitude to the rotation speed
                wheelRotationVector.x *= rotationVectorMagnitude;
                wheelRotationVector.y *= rotationVectorMagnitude;
            }else{
                wheelRotationVector.x = 0.0;
                wheelRotationVector.y = 0.0;
            }

            //decompose the rotation vector and add to wheel velocity
            wheelVelocity[i] += wheelRotationVector.x / wheelFrictionVector_millimetersPerRevolution[i].x;
            wheelVelocity[i] += wheelRotationVector.y / wheelFrictionVector_millimetersPerRevolution[i].y;
        }


        #define CONTROL_PROFILE_PRINT
        #ifdef CONTROL_PROFILE_PRINT
            Serial.println("---Control Logic---------------------------");
            Serial.printf("Velocity (mm/s & deg/s)   X: %.3f  Y: %.3f  R: %.3f\n", xVelActual_mmPerSec, yVelActual_mmPerSec, rotVelActual_degPerSec);
            Serial.printf("Translation Limit %.3fmm/s %.3fmm/s2\n", translVelLim_mmPerSec, translAcc_mmPerSecSq);
            Serial.printf("Rotation Limit %.3fdeg/s %.3fdeg/s2\n", rotVelLim_degPerSec, rotAcc_degPerSecSq);
            Serial.printf("Rotation Center (mm) #%i   X: %.3f  Y: %.3f\n", currentRotationCenter, centerOfRotation.x, centerOfRotation.y);
            Serial.printf("Wheel Velocities (rps)    FL: %.3f  FR: %.3f\n                          BL: %.3f  BR: %.3f\n", wheelVelocity[0], wheelVelocity[1], wheelVelocity[2], wheelVelocity[3]);
        #endif
    }

    void reset(){
        xVelActual_mmPerSec = 0.0;
        yVelActual_mmPerSec = 0.0;
        rotVelActual_degPerSec = 0.0;
        xVelTarg_mmPerSec = 0.0;
        yVelTarg_mmPerSec = 0.0;
        rotVelTarg_degPerSec = 0.0;
    }
}