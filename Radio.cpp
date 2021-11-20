// Lora Radio
#include "Radio.h"

Radio* Radio::m_pInstance = NULL;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

//RHMesh manager(rf95, 4);

Radio::Radio()
{
    address = 0; // For EEPROM
    

}
Radio* Radio::Instance()
{
  if (!m_pInstance)
  {
      m_pInstance = new Radio;
  }
  return m_pInstance;
}
void Radio::sayHello()
{
    Serial.println("Radio::Hello");
}
void Radio::initialise(uint8_t loraID)
{
    Serial.print("Radio::initialise with loraID:");
    Serial.println(loraID);

    pinMode(LORA,OUTPUT); 
    digitalWrite(LORA,1);//  LED off
    
    pinMode(RFM95_RST, OUTPUT); // LORA RESET
    digitalWrite(RFM95_RST, 1);
    // manual reset
    digitalWrite(RFM95_RST, 0);
    delay(10);
    digitalWrite(RFM95_RST, 1);
    delay(10);
    manager = new RHMesh(rf95, loraID);
    if (!manager->init())
    {
      Serial.println(F("Radio init failed"));
      digitalWrite(LORA,1);//  LED OFF
    }
    else
    {
      Serial.println("Radio initialised");
      digitalWrite(LORA,0);//  LED OFF
    }
}
void Radio::onReceive(Sensor *pSensor)
{
  uint8_t buf[32] = {0};
  uint8_t len = sizeof(buf);
  uint8_t from;
  uint8_t responseData[32];

  
  if (manager->recvfromAck(buf, &len, &from))
  {
    // Assume message is for me.
    digitalWrite(LORA,1); 
    Serial.print("onReceive()");
    //Serial.println((char*)buf);
    xcisMessage.dumpHex(buf,sizeof(buf));
    xcisMessage.processMessage(buf);
    Serial.print("Radio::onReceive() Command:");
    Serial.println(xcisMessage.getCommand(),HEX);
    if (xcisMessage.getCommand() == STATUS_REQUEST)
    { 
      // This is a broadcast message to check for new devices
      Serial.println("Device Received:STATUS_REQUEST");
      // Check if initialised
      // if not - return STATUS_NEW + UID + DEVICE_TYPE
      if (Device::Instance()->getLoraInitState() == false)
      {
        Serial.println("Device not initialised!! - sending device data");
        xcisMessage.createStatusPayload(STATUS_RESPONSE, Device::Instance()->getUID(),Device::Instance()->getDeviceType() );
        xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), Device::Instance()->getDeviceType(), STATUS_RESPONSE);
        manager->sendtoWait(responseData, sizeof(responseData), from);
        Serial.print("Response:");
        xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
      }
    }
    else
    {

      pSensor->processMessage(buf,responseData);
      manager->sendtoWait(responseData, sizeof(responseData), from);

    }
    digitalWrite(LORA,0);
  }
  return;
}
void Radio::setLoraDefault()
{
     EEPROM.write(address, 1);
}
void Radio::setLoraInitialised()
{
  EEPROM.write(address, 2);
}
bool Radio::checkLoraInitialised()
{
  // read a byte from the current address of the EEPROM
  byte value  = 0;
  value = EEPROM.read(address);

  //Serial.print(address);
  //Serial.print("\t");
  //Serial.print(value, DEC);
  //Serial.println();

  if (value == 1)
  {
    return false;
  }
  else if (value == 2)
  {
    return true;
  }
}
int Radio::getChipID()
{
  // Code to get the Chip ID
  int chipID = 2222;

  return chipID;
}
String Radio::getChipID_string()
{
  String chipID = "2222";
  // Code to get the Chip ID
  return chipID;
}
