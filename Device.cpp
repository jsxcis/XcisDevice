#include "Device.h"

Device* Device::m_pInstance = NULL;

Device::Device()
{
    uid_d = 0x00000000;
    mode = 0; // Default to no activity = 0, 1 = sensing, 2 = hunting 

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

    pinMode(DEFAULT,INPUT);

    getDefaultSwitch();
        
    uid.initialise();
    pmem.initialise();
    //pmem.reset(); // remove this line - testing only
    pmem.displayPMEM();
    // Manual loraID setting.
    //Serial.println("*******Manual Lora Setting**********");
    //pmem.setLoraID(0x17); // Decimal 23
   

    Serial.println(pmem.getInitState());
    if (getLoraInitState() == false)
    {
        Serial.println("NO LORA ID: Entering hunting mode using LORAID:254");
        mode = 2;
    }
    if (getLoraInitState()== true)
    {
        Serial.println("FOUND LORA ID: Entering sensing mode");
        mode = 1;
    }
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
        case TEST_MODE: //111 SW 000
        {
            Serial.println("TEST_MODE");
            pSensor = new XcisTestMode();
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
void Device::initialise() // Dont this  is used.
{
    Serial.println("Device::initialise");
    randomSeed(analogRead(0)); // Seed the random number generator
    pinMode(STATUS,OUTPUT); 
    digitalWrite(STATUS,1);// LED off

    pinMode(SENSOR_STATUS,OUTPUT);
    digitalWrite(SENSOR_STATUS,0); // LED ON

    pinMode(DEFAULT,INPUT);

    getDefaultSwitch();
        
    uid.initialise();
    pmem.initialise();
    //pmem.reset(); // remove this line - testing only
    pmem.displayPMEM();
    // Manual loraID setting.
    //Serial.println("*******Manual Lora Setting**********");
    //pmem.setLoraID(0x17); // Decimal 23 
   

    Serial.println(pmem.getInitState());
    if (getLoraInitState() == false)
    {
        Serial.println("NO LORA ID: Entering hunting mode using LORAID:254");
        mode = 2;
    }
    if (getLoraInitState()== true)
    {
        Serial.println("FOUND LORA ID: Entering sensing mode");
        mode = 1;
    }
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
        case TEST_MODE: //111 SW 000
        {
            Serial.println("TEST_MODE");
            pSensor = new XcisTestMode();
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
}
void Device::onReceive() // called from main loop
{
     Radio::Instance()->onReceive(pSensor);
}
void Device::execute() // Called from main loop
{
    if (mode == 1)
    {
        // In sensing mode (normal)
        //Serial.println("Sensing mode");
        pSensor->execute();
    }
    else if (mode == 0)
    {
        // Passive mode
        // In sensing mode (normal)
        //Serial.println("Passive mode");
        
    }
    else if (mode == 2)
    {
        //Serial.println("Hunting mode");
        // Use the radio to send a hunting message
        //Radio::Instance()->sendID(pSensor);
        
    }
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
void Device::getDefaultSwitch()
{
    uint8_t def;
    def = digitalRead(DEFAULT);
    Serial.print("Default:");
    Serial.println(def);
    if (def == 0)
    {
        setDefaults();
    }
}
void Device::setDefaults()
{
    Serial.println("Setting Defaults");
    pmem.reset();
}
void Device::setLoraID(uint8_t loraID)
{
    Serial.print("Device::setLoraID:");
    Serial.println(loraID);
    pmem.setLoraID(loraID);
}
void Device::restart()
{
  Serial.println("Restart called");
  {
    noInterrupts();
    wdt_enable(WDTO_15MS);
    while(1);
  }
}