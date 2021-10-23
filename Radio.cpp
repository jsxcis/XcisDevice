// Lora Radio
#include "Radio.h"


Radio* Radio::m_pInstance = NULL;

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHMesh manager(rf95, 4);

Radio::Radio()
{
    address = 0; // For EEPROM
    
    node_id_default = "04"; // Default Lora ID
    node_id_mesh_default = 4; // Default Lora ID

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
void Radio::initialise()
{
    Serial.println("Radio::initialise");
 
    pinMode(LORA,OUTPUT); 
    digitalWrite(LORA,0);// Green LED off
    
    pinMode(RFM95_RST, OUTPUT); // LORA RESET
    digitalWrite(RFM95_RST, 1);
    // manual reset
    digitalWrite(RFM95_RST, 0);
    delay(10);
    digitalWrite(RFM95_RST, 1);
    delay(10);

    if (!manager.init())
    {
      Serial.println(F("Radio init failed"));
    }
    else
    {
      Serial.println("Radio initialised");
    }
    
    digitalWrite(LORA,0);// Green LED OFF
}
void Radio::onReceive(Sensor *pSensor)
{
  uint8_t buf[32] = {0};
  uint8_t len = sizeof(buf);
  uint8_t from;
  uint8_t responseData[32];
  
  if (manager.recvfromAck(buf, &len, &from))
  {
    // Assume message is for me.
    digitalWrite(LORA,1); 
    Serial.print("onReceive()");
    //Serial.println((char*)buf);
    xcisMessage.dumpHex(buf,sizeof(buf));
    
    pSensor->processMessage(buf,responseData);
  
    manager.sendtoWait(responseData, sizeof(responseData), from);   
    
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
