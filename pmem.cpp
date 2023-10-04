// UID Class
#include "PMEM.h"

//We create two fucntions for writing and reading data from the EEPROM

template <class T> int EEPROM_writeAnything(int ee, const T& value)

{

    const byte* p = (const byte*)(const void*)&value;

    unsigned int i;

    for (i = 0; i < sizeof(value); i++)

          EEPROM.write(ee++, *p++);

    return i;

}



template <class T> int EEPROM_readAnything(int ee, T& value)

{

    byte* p = (byte*)(void*)&value;

    unsigned int i;

    for (i = 0; i < sizeof(value); i++)

          *p++ = EEPROM.read(ee++);

    return i;

}



PMEM::PMEM()
{
    addr = 0;

}
void PMEM::initialise()
{
    Serial.println("PMEM::initialisePMEM");

   
}
void PMEM::reset()
{
    Serial.println("PMEM::reset()");

    lorastate ls;
    ls.loraID = 0xFE;
    ls.initFlag = 0x00;
    //EEPROM.write(addr,ls.loraID);
    //EEPROM.write(addr + sizeof(ls.loraID),ls.initFlag);
    EEPROM_writeAnything(0,ls);
}
void PMEM::displayPMEM()
{ 
    lorastate ls;
    EEPROM_readAnything(addr,ls);
    Serial.print("displayPMEM:");
    Serial.print(ls.loraID);
    Serial.print(",");
    Serial.println(ls.initFlag);
}
void PMEM::setLoraID(uint8_t loraID)
{
    Serial.print("PMEM::setLoraID() to:");
    Serial.println(loraID);
    lorastate ls;
    ls.loraID = loraID;
    ls.initFlag = 0x01;
    //EEPROM.write(addr,ls.loraID);
    //EEPROM.write(addr + sizeof(ls.loraID),ls.initFlag);
    EEPROM_writeAnything(0,ls);
}
uint8_t PMEM::getLoraID()
{
    Serial.println("PMEM::getLoraID()");
    lorastate ls;
    EEPROM_readAnything(addr,ls);
    return ls.loraID;

}
bool PMEM::getInitState()
{
    Serial.println("PMEM::getInitState()");
    lorastate ls;
    EEPROM_readAnything(addr,ls);
    return ls.initFlag;
}
