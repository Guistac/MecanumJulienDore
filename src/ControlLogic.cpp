#include "ControlLogic.h"
#include "MecanumRobot.h"

namespace ControlLogic{

    bool b_rabbitMode = false;

    void setRabbitMode(){ b_rabbitMode = true; }
    void setSnailMode(){ b_rabbitMode = false; }

    double xVelocityActual_millimetersPerSecond = 0.0;
    double yVelocityActual_millimetersPerSecond = 0.0;
    double rotationalVelocityActual_degreesPerSecond = 0.0;

    double xVelocityTarget_millimetersPerSecond = 0.0;
    double yVelocityTarget_millimetersPerSecond = 0.0;
    double rotationalVelocityTarget_degreesPerSecond = 0.0;

    double translationAcceleration_millimetersPerSecondSquared = 300.0;
    double rotationalAcceleration_degreesPerSecondSquared = 300.0;

    double maxTranslationVelocity_millimetersPerSecond = 500.0;
    double maxRotationalVelocity_degreesPerSecond = 100.0;

    double wheelDiameter_millimeters = 152.4;
    double wheelCircumference_millimeters = PI * wheelDiameter_millimeters;

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

    void setXVelocity(float v){
        xVelocityTarget_millimetersPerSecond = v;
    }
    
    void setYVelocity(float v){
        yVelocityTarget_millimetersPerSecond = v;
    }

    void setRotationalVelocity(float v){
        rotationalVelocityTarget_degreesPerSecond = v;
    }

    uint32_t previousUpdateTime_microseconds = 0.0;

    void update(){

        uint32_t now = micros();
        double deltaT_seconds = (double)(now - previousUpdateTime_microseconds) / 1000000.0;
        previousUpdateTime_microseconds = now;

        double deltaVTranslation_millimetersPerSecond = translationAcceleration_millimetersPerSecondSquared * deltaT_seconds;

        if(xVelocityActual_millimetersPerSecond < xVelocityTarget_millimetersPerSecond){
            xVelocityActual_millimetersPerSecond += deltaVTranslation_millimetersPerSecond;
            if(xVelocityActual_millimetersPerSecond > xVelocityTarget_millimetersPerSecond) xVelocityActual_millimetersPerSecond = xVelocityTarget_millimetersPerSecond;
        }else if(xVelocityActual_millimetersPerSecond > xVelocityTarget_millimetersPerSecond){
            xVelocityActual_millimetersPerSecond -= deltaVTranslation_millimetersPerSecond;
            if(xVelocityActual_millimetersPerSecond < xVelocityTarget_millimetersPerSecond) xVelocityActual_millimetersPerSecond = xVelocityTarget_millimetersPerSecond;
        }

        if(yVelocityActual_millimetersPerSecond < yVelocityTarget_millimetersPerSecond){
            yVelocityActual_millimetersPerSecond += deltaVTranslation_millimetersPerSecond;
            if(yVelocityActual_millimetersPerSecond > yVelocityTarget_millimetersPerSecond) yVelocityActual_millimetersPerSecond = yVelocityTarget_millimetersPerSecond;
        }else if(yVelocityActual_millimetersPerSecond > yVelocityTarget_millimetersPerSecond){
            yVelocityActual_millimetersPerSecond -= deltaVTranslation_millimetersPerSecond;
            if(yVelocityActual_millimetersPerSecond < yVelocityTarget_millimetersPerSecond) yVelocityActual_millimetersPerSecond = yVelocityTarget_millimetersPerSecond;
        }

        double deltaVRotation_degreesPerSecond = rotationalAcceleration_degreesPerSecondSquared * deltaT_seconds;

        if(rotationalVelocityActual_degreesPerSecond < rotationalVelocityTarget_degreesPerSecond){
            rotationalVelocityActual_degreesPerSecond += deltaVRotation_degreesPerSecond;
            if(rotationalVelocityActual_degreesPerSecond > rotationalVelocityTarget_degreesPerSecond) rotationalVelocityActual_degreesPerSecond = rotationalVelocityTarget_degreesPerSecond;
        }else if(rotationalVelocityActual_degreesPerSecond > rotationalVelocityTarget_degreesPerSecond){
            rotationalVelocityActual_degreesPerSecond -= deltaVRotation_degreesPerSecond;
            if(rotationalVelocityActual_degreesPerSecond < rotationalVelocityTarget_degreesPerSecond) rotationalVelocityActual_degreesPerSecond = rotationalVelocityTarget_degreesPerSecond;
        }

        Vector2 centerOfRotation = rotationCenters[currentRotationCenter];

        for(int i = 0; i < WHEEL_COUNT; i++){
            //start calculation from zero
            wheelVelocity[i] = 0.0;

            //X component velocity
            wheelVelocity[i] += xVelocityActual_millimetersPerSecond / wheelFrictionVector_millimetersPerRevolution[i].x;

            //Y component velocity
            wheelVelocity[i] += yVelocityActual_millimetersPerSecond / wheelFrictionVector_millimetersPerRevolution[i].y;

            //Rotational Component velocity

            //wheel position relative to rotation center
            Vector2 relativeWheelPosition;
            relativeWheelPosition.x = wheelPosition_millimeter[i].x - centerOfRotation.x;
            relativeWheelPosition.y = wheelPosition_millimeter[i].y - centerOfRotation.y;

            float rotationRadius = sqrt(sq(relativeWheelPosition.x) + sq(relativeWheelPosition.y));
            float rotationCirclePerimeter = 2.0 * PI * rotationRadius;
            float rotationVectorMagnitude = rotationCirclePerimeter * rotationalVelocityActual_degreesPerSecond / 360.0;
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
            Serial.printf("Velocity (mm/s & deg/s)   X: %.3f  Y: %.3f  R: %.3f\n", xVelocityActual_millimetersPerSecond, yVelocityActual_millimetersPerSecond, rotationalVelocityActual_degreesPerSecond);
            Serial.printf("Rotation Center (mm) #%i   X: %.3f  Y: %.3f\n", currentRotationCenter, centerOfRotation.x, centerOfRotation.y);
            Serial.printf("Wheel Velocities (rps)    FL: %.3f  FR: %.3f\n                          BL: %.3f  BR: %.3f\n", wheelVelocity[0], wheelVelocity[1], wheelVelocity[2], wheelVelocity[3]);
        #endif
    }

    void reset(){
        xVelocity_millimetersPerSecond = 0.0;
        yVelocity_millimetersPerSecond = 0.0;
        rotationalVelocity_degreesPerSecond = 0.0;
    }
}