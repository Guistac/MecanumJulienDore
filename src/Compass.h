#pragma once

#include <Arduino.h>

namespace Compass{

    void init();
    void update();
    void setZeroHeading();
    float getHeadingDegrees();
    
}