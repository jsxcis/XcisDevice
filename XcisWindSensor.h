#ifndef XcisWindSensor_h
#define XcisWindSensor_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>
#include <SoftwareSerial.h>
#include "Device.h"
#include <Wire.h>
#include <SPI.h>
#include "TimerOne.h"
#include <math.h>

#define WindSensorPin (3) // The pin location of the anemometer sensor 
#define WindVanePin (A0) // The pin the wind vane sensor is connected to 
#define VaneOffset 0; // define the anemometer offset from magnetic north 

class XcisWindSensor : public Sensor {
    public:
        XcisWindSensor();
        void initialise();
        void execute();
        void execute(int mode);
        void processMessage(uint8_t *data, uint8_t *responseData);
    
    private:
        void getWindDirection();
        uint16_t distance;

        unsigned long delayStart; // the time the delay started

        bool delayRunning; // true if still waiting for delay to finish
       
        int vaneValue; // raw analog value from wind vane 
        int windDirection; // translated 0 - 360 direction 
        int windCalDirection; // converted value with offset applied 
        String windCompassDirection; // wind direction as compass points
        int lastWindDirectionValue; // last direction value 
        float windSpeed; // speed miles per hour 

};

#endif