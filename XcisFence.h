#ifndef XcisFence_h
#define XcisFence_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>
#include <SoftwareSerial.h>

#define SERIAL_RX 20 // CHIP PIN 26 GREEN
#define SERIAL_TX 21 // CHIP PIN 27 BLUE

class XcisFence : public Sensor {
    public:
        XcisFence();
        void initialise();
        void execute();
        void processMessage(uint8_t *data, uint8_t *responseData);

    private:
        void readDataStream();
        uint16_t value;

        unsigned long delayStart; // the time the delay started

        bool delayRunning; // true if still waiting for delay to finish
         
        //XcisMessage xcisMessage;

};
#endif