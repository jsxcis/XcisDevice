// Xcis Flow Meter
#include "XcisTank.h"

SoftwareSerial usonic_tank(SERIAL_RX, SERIAL_TX); //SDA,SCL-  RX, TX

XcisTank::XcisTank()
{
    delayStart = 0;   // start delay
    delayRunning = 0; // not finished yet
}
void XcisTank::initialise()
{
    Serial.println("XcisTank::initialise");
    delayStart = millis();   // start delay
    delayRunning = true; // not finished yet
    usonic_tank.begin(9600);
}
void XcisTank::execute()
{
    
    if (delayRunning && ((millis() - delayStart) >= 100))// 5 secs 
    {
        delayStart +=100; // 5 secs
        readDataStream();
    }
}
void XcisTank::execute(int mode)
{
    if (delayRunning && ((millis() - delayStart) >= 10000))// 10 secs 
    {
        delayStart +=10000; // 10 secs
        // Send hunting message - looking for a gateway
        Serial.println("Sending hunting message");
    }
}
void XcisTank::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[28];


    Serial.print("XcisTank::processMessage:");
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
    
      Serial.print("Voltage value:");
      uint16_t battery = getVoltageBytes();
      Serial.println(battery,HEX);
      Serial.print("Distance value:");
      Serial.println(distance,HEX);

      xcisMessage.createDistancePayload(SENSOR_DATA_RESPONSE, battery,distance);  
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), TANK, SENSOR_DATA_RESPONSE);
  
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
}

void XcisTank::readDataStream()
{
  unsigned char data[4]={};
  unsigned char incomingByte = 0;
  int byteCount = 0;
  bool foundPacket = false;
  while (usonic_tank.available() > 0) 
  {
    // read the incoming byte:
    incomingByte = usonic_tank.read();
    if (incomingByte == 0xFF)
    {
      //Serial.println("Found start");
      data[0] = incomingByte;
      byteCount++;
      while (usonic_tank.available() > 0) {
        data[byteCount] = usonic_tank.read();
        byteCount++;
        if (byteCount > 3)
        {
          foundPacket = true;
          break;
        }
      }
    }
    if (foundPacket)
        break;
  }
  usonic_tank.flush();

  if(data[0]==0xff)
  {
    int sum;
    sum=(data[0]+data[1]+data[2])&0x00FF;
    if(sum==data[3])
    {
        distance=(data[1]<<8)+data[2];
    }
    else 
    {
      //Serial.println(F("ERROR"));
    }
  }
}