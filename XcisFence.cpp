// Xcis Flow Meter
#include "XcisFence.h"

SoftwareSerial sensor(SERIAL_RX, SERIAL_TX); //SDA,SCL-  RX, TX

XcisFence::XcisFence()
{
    delayStart = 0;   // start delay
    delayRunning = 0; // not finished yet
}
void XcisFence::initialise()
{
    Serial.println("XcisFence::initialise");
    delayStart = millis();   // start delay
    delayRunning = true; // not finished yet
    sensor.begin(9600);
}

void XcisFence::execute()
{
    
    if (delayRunning && ((millis() - delayStart) >= 100))// 5 secs 
    {
        delayStart +=100; // 5 secs
        readDataStream();
    }
}
void XcisFence::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[28];

    Serial.print("XcisFence::processMessage:");
    xcisMessage.dumpHex(data,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    xcisMessage.processMessage(data);
    Serial.print(" LocationID:");
    Serial.print(xcisMessage.getLocationID(),HEX);
    Serial.print(" DeviceType:");
    Serial.print(xcisMessage.getDeviceType(),HEX);
    Serial.print(" Command:");
    Serial.print(xcisMessage.getCommand(),HEX);
    xcisMessage.getPayload(recvPayload);
    Serial.print(" Payload:");
    xcisMessage.dumpHex(recvPayload,28);
    if (xcisMessage.getCommand() == SENSOR_DATA_REQUEST)
    { 
      Serial.println("Received:SENSOR_DATA_REQUEST");
    
      Serial.print("Battery value:");
      uint16_t battery = getVoltageBytes();
      Serial.println(battery,HEX);
      Serial.print("Voltage value:");
      Serial.println(value,HEX);

      xcisMessage.createVoltagePayload(SENSOR_DATA_RESPONSE, battery,value);  
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), FENCE, SENSOR_DATA_RESPONSE);
  
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
   
}

void XcisFence::readDataStream()
{
  String data = "";
  String incomingData = "";
  bool foundPacket = false;
  if (sensor.available() > 0)
  {
    Serial.println(F("readDataStream()"));
  }
  while (sensor.available() > 0) 
  {
    // read the incoming byte:
    data = sensor.readString();
    incomingData = data.substring(2,6);
    value = atol(incomingData.c_str());
    //Serial.println(value,HEX);
  }
}