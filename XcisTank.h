#ifndef XcisTank_h
#define XcisTank_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>
#include <SoftwareSerial.h>
#include "Device.h"


#define SERIAL_RX 20 // CHIP PIN 26 GREEN
#define SERIAL_TX 21 // CHIP PIN 27 BLUE

class XcisTank : public Sensor {
    public:
        XcisTank();
        void initialise();
        void execute();
        void execute(int mode);
        void processMessage(uint8_t *data, uint8_t *responseData);

    private:
        void readDataStream();
        uint16_t distance;

        unsigned long delayStart; // the time the delay started

        bool delayRunning; // true if still waiting for delay to finish
         
        //XcisMessage xcisMessage;

};
#endif