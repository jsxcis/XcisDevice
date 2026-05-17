#ifndef Device_h
#define Device_h
#include "Arduino.h"
#include <SPI.h>
#include "UID.h"
#include "Radio.h"
#include "Sensor.h"
#include "XcisFlowMeter.h"
#include "XcisRainGauge.h"
#include "XcisTrough.h"
#include "XcisTank.h"
#include "XcisFence.h"
#include "XcisBore.h"
#include "XcisTestMode.h"
#include "XcisWeather.h"
#include "XcisWindSensor.h"
#include "pmem.h"
#include <avr/wdt.h>
#include <TimeLib.h>

#ifdef ARDUINO_AVR_UNO
#define SW1 11
#define SW2 12
#define SW3 13
#define DEFAULT 7
#elif ARDUINO_AVR_MEGA2560
#define SW1 11
#define SW2 12
#define SW3 13
#define DEFAULT 7
#elif __AVR_ATmega1284P__
#define SW1 12
#define SW2 13
#define SW3 14
#define DEFAULT 15
#elif __AVR_ATmega1284__
#define SW1 12
#define SW2 13
#define SW3 14
#define DEFAULT 15
#endif

// Configuration switchs atmega1284p
//#define SW1 12
//#define SW2 13
//#define SW3 14
// Default switch
//#define DEFAULT 15

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

#define STATUS A3 // STATUS LED - DEVICE INITIIALISED
// Analog LED for Sensor Status
#define SENSOR_STATUS A1 // STATUS LED - DEVICE ACTIVE - PWR ON

#define DEFAULT_LORA_ID 0x05 // Used to hard code the LORA ID = decimal 28


class Device
{
    public:
    static Device* Instance();
  
    void sayHello();
    void initialise(String board);
    void initialise();
    void onReceive();
    void execute();
    uint8_t getDeviceType() {return deviceType;}
    Sensor* getSensor() {return pSensor;}
    uint32_t getUID(){return uid_d;}
    bool getLoraInitState(){return pmem.getInitState();}
    void setLoraID(uint8_t loraID);
    void restart();
    void getDefaultSwitch();
    void setDefaults();

    private:
    Device();
    Device(Device const&){};
    static Device* m_pInstance;

    UID uid;
    PMEM pmem;
    uint8_t readDIPSwitches();
    Sensor *pSensor;
    uint8_t deviceType;
    uint8_t loraID;
    uint32_t uid_d;
    bool defaultLoraID;
    bool defaultUID; // Set to false for normal operation
   
    int mode;

   
};
#endif