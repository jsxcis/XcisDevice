#ifndef Device_h
#define Device_h
#include "Arduino.h"
#include <SPI.h>
#include "UID.h"
#include "Radio.h"
#include "Sensor.h"


// Configuration switch
#define SW1 12
#define SW2 13
#define SW3 14

//opcodes
#define WREN  0x06 // write enable command
#define WRDI  0x04 // write disable
#define RDSR  0x05 // read the status register
#define WRSR  0x01 // write the status register 
#define READ  0x03 // read data
#define WRITE 0x02 // write data

#define DATAOUT 5//MOSI PB5
#define DATAIN  6//MISO PB6
#define SPICLOCK  7//sck PB7
#define SLAVESELECT 10//PD2
//#define HOLD 9 // Not used

#define STATUS 0 // STATUS LED - DEVICE ACTIVE


class Device
{
    public:
    Device();
    void sayHello();
    void initialise(String board);
    void onReceive();
    void execute();

    Sensor* getSensor() {return pSensor;}

    private:
    UID uid;
    uint8_t readDIPSwitches();
    Sensor *pSensor;
    uint8_t deviceType;

   
};
#endif