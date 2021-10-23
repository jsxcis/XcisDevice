// Xcis Flow Meter
#include "XcisRainGauge.h"

XcisRainGauge::XcisRainGauge()
{
    delayStart = 0;   // start delay
    delayStartPulse = 0;   // start delay
    delayRunning = 0; // not finished yet
    inputState = 0;
    lastInputState = 0;
    pulseCount = 0x00;
    accumulatedPulses = 0x0000;
    accumulatedDataToken = 0x000000;

}
void XcisRainGauge::initialise()
{
    Serial.println("XcisRainGauge::initialise");
    delayStart = millis();   // start delay
    delayStartPulse = millis();   // start delay
    delayRunning = true; // not finished yet
    pinMode(PULSE,INPUT); // Pulse input
}

void XcisRainGauge::execute()
{
    
    inputState = digitalRead(PULSE);
    if (inputState != lastInputState)
    {
        if (inputState == 1)
        {
            Serial.println("Got pulse");
            countPulses();
        }
    }
    lastInputState = inputState;
    if (delayRunning && ((millis() - delayStartPulse) >= 900000))// 15 mins 
    {
        delayStartPulse += 900000; // 15 mins - normal value
        storePulseCount();
    }
}
void XcisRainGauge::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[28];
    //uint8_t responseData[32];

    Serial.print("XcisRainGauge::processMessage:");
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
      //uint16_t battery = 0;
      Serial.println(battery,HEX);
     
      Serial.print("Pulses Integer:");
      Serial.println(accumulatedPulses);
      Serial.print("Pulses Hex:");
      Serial.println(accumulatedPulses,HEX);
      
      Serial.print("accumulatedDataToken Integer:");
      Serial.println(accumulatedDataToken);
      Serial.print("accumulatedDataToken Hex:");
      Serial.println(accumulatedDataToken,HEX);
     
      xcisMessage.createPulseCounterPayload(SENSOR_DATA_RESPONSE, battery,accumulatedPulses, accumulatedDataToken);  
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), RAIN_GAUGE, SENSOR_DATA_RESPONSE);
  
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
}
void XcisRainGauge::countPulses()
{
    pulseCount++;
}
void XcisRainGauge::storePulseCount()
{
    //pulseCount = 25; // Temporary for testing, remove
    accumulatedPulses = pulseCount;
    //accumulatedDataToken = now();
    accumulatedDataToken = random(10000000,99999999);
    pulseCount = 0;
    Serial.print("Storing pulses:");
    Serial.print(accumulatedPulses);
    Serial.print(",");
    Serial.println(accumulatedDataToken);
}