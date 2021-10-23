#include "Device.h"

Device::Device()
{


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
    digitalWrite(STATUS,0);// LED off
    
    uid.initialise();
    uid.readUIDManufacturer();
    uid.readDeviceCode();
    uid.readStatusReg();
    uid.readID();
    switch(readDIPSwitches())
    {
        case TANK: // 000
        {
            Serial.println("DeviceConfiguration=TANK");
            break;
        }
        case TROUGH: // 001
        {
            Serial.println("DeviceConfiguration=TROUGH");
             break;
        }
        case BORE_CONTROLLER: //010
        {
            Serial.println("DeviceConfiguration=BORE_CONTROLLER");
             break;
        }
        case WEATHER_SENSOR: //011
        {
            Serial.println("DeviceConfiguration=WEATHER_SENSOR");
             break;
        }
        case RAIN_GAUGE: //100
        {
            Serial.println("DeviceConfiguration=RAIN_GAUGE");
            pSensor = new XcisRainGauge();
            break;
        }
        case FENCE: //101
        {
            Serial.println("DeviceConfiguration=FENCE");
            break;
        }
        case FLOW_METER: //110
        {
            Serial.println("DeviceConfiguration=FLOW_METER");
            pSensor = new XcisFlowMeter();
            break;
        }
        default:
        {
            Serial.println("DeviceConfiguration=UNKNOWN");
            break;
        }
    }
    /*
    deviceType = 1;

    if (deviceType == 1)
    {
        Serial.println("Initialising RainGauge");
        pSensor = new XcisRainGauge();
    }
    else if (deviceType == 2)
    {
        pSensor = new XcisFlowMeter();
    }
    else
    {
        pSensor = NULL;
    }
    */
    //Sensor *pSensor = getSensor();
    pSensor->initialise();
    Radio::Instance()->initialise();
    digitalWrite(STATUS,1);// LED 
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