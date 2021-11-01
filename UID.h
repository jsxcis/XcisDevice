// UID Chip Class#ifndef Device_h
#ifndef UID_h
#define UID_h
#include "Arduino.h"
#include <SPI.h>

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

class UID
{
    public:
    UID();
    void initialise();
    void readUIDManufacturer();
    void readDeviceCode();
    void readStatusReg();
    uint32_t readID();

    private:
    byte inBuffer[9]; // eeprom data in

};


#endif