#ifndef Sensor_h
#define Sensor_h
#include "Arduino.h"
#include "XcisMessage.h"

// Analog PIN for Battery input
#define BATTERY_VOLTAGE A0


class Sensor {
    public:
        Sensor();
        virtual void initialise()=0;
        virtual void execute()=0;
        virtual void processMessage(uint8_t *data, uint8_t *responseData)=0;
        uint16_t getVoltageBytes();

        XcisMessage xcisMessage;
        
};
#endif