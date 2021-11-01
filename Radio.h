// LORA Radio
#ifndef Radio_h
#define Radio_h
#include "Arduino.h"
#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <XcisMessage.h>
#include "Sensor.h"
#include "Device.h"
//#include "XcisFlowMeter.h"
//#include "XcisRainGauge.h"
//#include "XcisTrough.h"
//#include "XcisTank.h"
//#include "XcisFence.h"

#define RH_HAVE_SERIAL
// Digital PIN for LORA Active
#define LORA 1 // LED

#ifdef ARDUINO_AVR_UNO
#define RFM95_CS 10
#define RFM95_RST 5
#define RFM95_INT  2
#elif ARDUINO_AVR_MEGA2560
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#elif __AVR_ATmega1284P__
#define RFM95_CS 4
#define RFM95_RST 3 //was 1 which was wrong
#define RFM95_INT 2
#endif

class Radio
{
    public:
    static Radio* Instance();

    void sayHello();
    void initialise(uint8_t loraID);
    void onReceive(Sensor *pSensor);
    XcisMessage xcisMessage;

    private:

    Radio();
    Radio(Radio const&){};

    static Radio* m_pInstance;
    RHMesh *manager;
    
    void setLoraDefault();
    void setLoraInitialised();
    bool checkLoraInitialised();
    int getChipID();
    String getChipID_string();

    uint8_t rfm95_cs;
    uint8_t rfm95_rst;
    uint8_t rfm95_int;
    unsigned int address;
    String node_id_default; // Default Lora ID
    unsigned int node_id_mesh_default; // Default Lora ID   
    uint8_t deviceType;
   
};
#endif