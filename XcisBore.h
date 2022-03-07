#ifndef XcisBore_h
#define XcisBore_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>
#include <SoftwareSerial.h>

#define PULSE 20 // CHIP PIN 27 BLUE
#define CURRENT A4 // Current Sensor input
#define ON_RELAY 22 // Relay on D22
#define OFF_RELAY 23 // Relay on D23
#define ON_DURATION 2000 // Time for relay to be ON.
#define BORE_ON_SW 18 // Input to switch bore on
#define BORE_OFF_SW 19 // Input to switch bore off

#define BORE_OFF_LED 0 // LED indicator for OFF state
#define BORE_ON_LED 1 // LED indicator for ON state

#define CURRENT_THRESHOLD 20 // Threshold to excuse noise

class XcisBore : public Sensor {
    public:
        XcisBore();
        void initialise();
        void execute();
        void processMessage(uint8_t *data, uint8_t *responseData);

    private:
        void readCurrentValue();
        void turnOn();
        void turnOff();
        void storePulseCount();
        void countPulses();
        void displayStatus();
        void calculateStatus();
        unsigned long delayStart; // the time the delay started
        unsigned long delayStartPulse;

        bool delayRunning; // true if still waiting for delay to finish
        uint16_t pulseCount;
        uint16_t currentValue;
        uint8_t boreState; // true if the bore is running 0 = off, 1= running
        uint8_t boreStatus; // true if the bore is running 0 = off, 1= running, 2 = run_err_current, 3 = run_err_flow, 4 = run_err
        uint16_t accumulatedPulses;
        uint32_t accumulatedDataToken;
        uint16_t duration;
        int inputState_D20;
        int lastInputState_D20;
        int inputState_D18;
        int lastInputState_D18;
        int inputState_D19;
        int lastInputState_D19;

};
#endif