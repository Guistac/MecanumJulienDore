#include <Arduino.h>

#include "MecanumRobot.h"

struct Vector2{
    double x = 0.0;
    double y = 0.0;
};

namespace ControlLogic{

    extern double wheelVelocity[4];

    void setRabbitMode();
    void setSnailMode();

    void setXVelocityNormalized(float v);
    void setYVelocityNormalized(float v);
    void setRotationalVelocityNormalized(float v);

    void selectNextRotationCenter();
    void selectPreviousRotationCenter();
    void resetRotationCenter();

    void update();
    void reset();
}