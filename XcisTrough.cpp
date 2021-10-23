// Xcis Flow Meter
#include "XcisTrough.h"

SoftwareSerial usonic(SERIAL_RX, SERIAL_TX); //SDA,SCL-  RX, TX

XcisTrough::XcisTrough()
{
    delayStart = 0;   // start delay
    delayRunning = 0; // not finished yet
}
void XcisTrough::initialise()
{
    Serial.println("XcisTrough::initialise");
    delayStart = millis();   // start delay
    delayRunning = true; // not finished yet
    usonic.begin(9600);
}

void XcisTrough::execute()
{
    
    if (delayRunning && ((millis() - delayStart) >= 100))// 5 secs 
    {
        delayStart +=100; // 5 secs
        readDataStream();
    }
}
void XcisTrough::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[28];


    Serial.print("XcisTrough::processMessage:");
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
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), TROUGH, SENSOR_DATA_RESPONSE);
  
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
}

void XcisTrough::readDataStream()
{
  unsigned char data[4]={};
  unsigned char incomingByte = 0;
  int byteCount = 0;
  bool foundPacket = false;
  while (usonic.available() > 0) 
  {
    // read the incoming byte:
    incomingByte = usonic.read();
    if (incomingByte == 0xFF)
    {
      //Serial.println("Found start");
      data[0] = incomingByte;
      byteCount++;
      while (usonic.available() > 0) {
        data[byteCount] = usonic.read();
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
  usonic.flush();

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