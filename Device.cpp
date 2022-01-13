#include "Device.h"

Device* Device::m_pInstance = NULL;

Device::Device()
{
    uid_d = 0x00000000;

}
Device* Device::Instance()
{
  if (!m_pInstance)
  {
      m_pInstance = new Device;
  }
  return m_pInstance;
}
void Device::sayHello()
{
    Serial.println("Device::Hello");
}
void Device::initialise(String board)
{
    Serial.println("Device::initialise:" + board);
    randomSeed(analogRead(0)); // Seed the random number generator
    pinMode(STATUS,OUTPUT); 
    digitalWrite(STATUS,1);// LED off

    pinMode(SENSOR_STATUS,OUTPUT);
    digitalWrite(SENSOR_STATUS,0); // LED ON
    
    uid.initialise();
    pmem.initialise();
    pmem.reset(); // remove this line - testing only
    pmem.setLoraID(0x18);
   

    Serial.println(pmem.getInitState());
    uid_d = uid.readID();
    deviceType = readDIPSwitches();
   
    switch(deviceType)
    {
        case TANK: // 000 SW 111
        {
            Serial.println("DeviceConfiguration=TANK");
            pSensor = new XcisTank();
            break;
        }
        case TROUGH: // 001 SW 011
        {
            Serial.println("DeviceConfiguration=TROUGH");
             pSensor = new XcisTrough();
             break;
        }
        case BORE_CONTROLLER: //010 SW 101
        {
            Serial.println("DeviceConfiguration=BORE_CONTROLLER");
            pSensor = new XcisBore();
            break;
        }
        case WEATHER_SENSOR: //011
        {
            Serial.println("DeviceConfiguration=WEATHER_SENSOR");
             // CAUTION NOT IMPLEMENTED
             break;
        }
        case RAIN_GAUGE: //100 SW 110
        {
            Serial.println("DeviceConfiguration=RAIN_GAUGE");
            pSensor = new XcisRainGauge();
            break;
        }
        case FENCE: //101 SW 010
        {
            Serial.println("DeviceConfiguration=FENCE");
            pSensor = new XcisFence();
            break;
        }
        case FLOW_METER: //110 SW 100
        {
            Serial.println("DeviceConfiguration=FLOW_METER");
            pSensor = new XcisFlowMeter();
            break;
        }
        default:
        {
            Serial.println("DeviceConfiguration=UNKNOWN");
             // CAUTION NOT IMPLEMENTED
            break;
        }
    }
    pSensor->initialise();
    Radio::Instance()->initialise(pmem.getLoraID());
    digitalWrite(STATUS,0);// LED ON
    Serial.print(F("Ready"));
    Serial.print(" Board:");
    Serial.println(board);  
}
void Device::onReceive()
{
     Radio::Instance()->onReceive(pSensor);
}
void Device::execute()
{
    pSensor->execute();
}
uint8_t Device::readDIPSwitches()
{
    uint8_t switches[3] = {0,0,0};
    uint8_t result = 0;
  
    pinMode(SW1,INPUT); 
    pinMode(SW2,INPUT); 
    pinMode(SW3,INPUT);
    switches[0] = digitalRead(SW1);
    switches[1] = digitalRead(SW2);
    switches[2] = digitalRead(SW3); 
    Serial.print("Switch Settings:");
    for (int i = 0; i< 3; i++)
    {
        //Serial.print(switches[i]);
        result |= (switches[i] & 0x01) << i;
    } 
    Serial.println(result,HEX);
    return result;
}