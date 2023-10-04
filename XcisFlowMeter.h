#ifndef XcisFlowMeter_h
#define XcisFlowMeter_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>

// Digital PIN for Pulse Inpit
#define PULSE 20

class XcisFlowMeter : public Sensor {
    public:
        XcisFlowMeter();
        void initialise();
        void execute();
        void execute(int mode){;}
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