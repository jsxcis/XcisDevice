// UID Chip Class#ifndef Device_h
#ifndef PMEM_h
#define PMEM_h
#include "Arduino.h"
#include <EEPROM.h>

 // Sensor status payload 
    typedef struct{
        uint8_t loraID;
        uint8_t initFlag;
    } lorastate;

class PMEM
{
    public:
    PMEM();
    void initialise();
    void reset();
    void setLoraID(uint8_t loraID);
    void displayPMEM();
    uint8_t getLoraID();
    bool getInitState();

    private:
    uint8_t addr;

};


#endif