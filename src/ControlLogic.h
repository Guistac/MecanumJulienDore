#include <Arduino.h>

#include "MecanumRobot.h"

struct Vector2{
    double x = 0.0;
    double y = 0.0;
};

namespace ControlLogic{

    void setRabbitMode();
    void setSnailMode();

    void setXVelocity(float v);
    void setYVelocity(float v);
    void setRotationalVelocity(float v);

    void selectNextRotationCenter();
    void selectPreviousRotationCenter();
    void resetRotationCenter();

    void update();
    void reset();
}