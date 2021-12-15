#include "Compass.h"

#include <NXPMotionSense.h>
#include <Wire.h>
#include <EEPROM.h>

#include "IMU/MahonyAHRS.h"
#include "IMU/MadgwickAHRS.h"

namespace Compass{

    NXPMotionSense imu;
    NXPSensorFusion NXPfilter;
    Mahony mahony;
    Madgwick madgwick;

    float headingRaw_degrees = 0.0;
    float headingOffset_degrees = 0.0;
    float heading_degrees = 0.0;
    float smoothingFilter = 0.95;

    void init(){
        imu.begin();
        //NXPfilter.begin(100);
        //mahony.begin(100);
        //madgwick.begin(100);
    }

    void update(){

        if (imu.available()) {
            float ax, ay, az;
            float gx, gy, gz;
            float mx, my, mz;
            imu.readMotionSensor(ax, ay, az, gx, gy, gz, mx, my, mz);
            //NXPfilter.update(gx, gy, gz, ax, ay, az, mx, my, mz);
            //mahony.update(gx, gy, gz, ax, ay, az, mx, my, mz);
            ///madgwick.updateIMU(ax, ay, az, gx, gy, gz);
            //madgwick.update(gx, gy, gz, ax, ay, az);
            //float newHeading_degrees = madgwick.getYaw();
            //float newHeading_degrees = mahony.getYaw();
            //float newHeading_degrees = NXPfilter.getYaw();
            //headingRaw_degrees = heading_degrees * smoothingFilter + (1.0 - smoothingFilter) * newHeading_degrees;
            //headingRaw_degrees = newHeading_degrees;
            //heading_degrees = headingRaw_degrees - headingOffset_degrees;
        }
    }

    void setZeroHeading(){
        headingOffset_degrees = headingRaw_degrees;
    }

    float getHeadingDegrees(){
        return heading_degrees;
    }


}