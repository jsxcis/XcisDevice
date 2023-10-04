#ifndef XcisFence_h
#define XcisFence_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>
#include <SoftwareSerial.h>

#define SERIAL_RX 20 // CHIP PIN 26 GREEN
#define SERIAL_TX 21 // CHIP PIN 27 BLUE

#define ADC_POSITIVE A7 // Fence Sensor Positive
#define ADC_NEGATIVE A6 // Fence Sensor Negative

enum readType {POS,NEG};
const int numReadings = 10;

class XcisFence : public Sensor {
    public:
        XcisFence();
        void initialise();
        void execute();
        void execute(int mode){;}
        void processMessage(uint8_t *data, uint8_t *responseData);

    private:
        void readDataStream();
        double readFenceInput(readType toRead);
        void readPositive();
        void readNegative();
        
        long readVcc();

        double adc_pos;
        double adc_neg;
        double value;
        double max_value;

        int pos_readings[numReadings];      // the readings from the analog input
        int pos_readIndex = 0;              // the index of the current reading
        int pos_total = 0;                  // the running total
        double pos_average = 0;                // the average
        double pos_fenceVoltage = 0;

        int neg_readings[numReadings];      // the readings from the analog input
        int neg_readIndex = 0;              // the index of the current reading
        int neg_total = 0;                  // the running total
        double neg_average = 0;                // the average
        double neg_fenceVoltage = 0;


        unsigned long delayStart; // the time the delay started

        bool delayRunning; // true if still waiting for delay to finish
         
        //XcisMessage xcisMessage;

};
#endif