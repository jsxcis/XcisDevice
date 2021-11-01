#ifndef XcisRainGauge_h
#define XcisRainGauge_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>

// Digital PIN for Pulse Inpit
#define PULSE 20

class XcisRainGauge : public Sensor {
    public:
        XcisRainGauge();
        void initialise();
        void execute();
        void processMessage(uint8_t *data, uint8_t *responseData);

    private:
        void storePulseCount();
        void countPulses();
        unsigned long delayStart; // the time the delay started
        unsigned long delayStartPulse;

        bool delayRunning; // true if still waiting for delay to finish
        uint16_t pulseCount;
        uint16_t accumulatedPulses;
        uint32_t accumulatedDataToken;
        int inputState;
        int lastInputState;
        //XcisMessage xcisMessage;

};
#endif