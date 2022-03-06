// Xcis Flow Meter
#include "XcisBore.h"

XcisBore::XcisBore()
{
    currentValue = 0;
    // Need to override this with a test on startup to check on bore state. Use Current?
    boreRunning = 0;
    delayStart = 0;   // start delay
    delayStartPulse = 0;   // start delay
    delayRunning = 0; // not finished yet
    inputState_D20 = 0;
    lastInputState_D20 = 0;
    inputState_D18;
    lastInputState_D18;
    inputState_D19;
    lastInputState_D19;

    pulseCount = 0x00;
    accumulatedPulses = 0x0000;
    accumulatedDataToken = 0x00000000;

    duration = 2000;
}
void XcisBore::initialise()
{
    Serial.println("XcisBore::initialise");
    delayStart = millis();   // start delay
    delayStartPulse = millis();   // start delay
    delayRunning = true; // not finished yet

    pinMode(PULSE,INPUT); // Pulse input
    pinMode(CURRENT,INPUT); // Current sensor input
    pinMode(ON_RELAY, OUTPUT); //RELAY (D22) NOTE:(D22) on PCB
    pinMode(OFF_RELAY, OUTPUT); //RELAY (D23) NOTE:(D23) on PCB
    pinMode(BORE_ON_SW,INPUT); // Bore On Input
    pinMode(BORE_OFF_SW, INPUT); // Bore Off Input       
}

void XcisBore::execute()
{
    // FLOW PULSE INPUT
    inputState_D20 = digitalRead(PULSE);
    if (inputState_D20 != lastInputState_D20)
    {
        if (inputState_D20 == 1)
        {
            Serial.println("Got pulse");
            countPulses();
            readCurrentValue();
        }
    }
    lastInputState_D20 = inputState_D20;
    // BORE ON SW
    inputState_D18 = digitalRead(BORE_ON_SW);
    if (inputState_D18 != lastInputState_D18)
    {
        if (inputState_D18 == 1)
        {
            Serial.println("Got BORE ON SW");
            turnOn();
        }
    }
    lastInputState_D18 = inputState_D18;

    // BORE OFF SW
    inputState_D19 = digitalRead(BORE_OFF_SW);
    if (inputState_D19 != lastInputState_D19)
    {
        if (inputState_D19 == 1)
        {
            Serial.println("Got BORE OFF SW");
            turnOff();
        }
    }
    lastInputState_D19 = inputState_D19;


    if (delayRunning && ((millis() - delayStartPulse) >= 900000))// 15 mins 
    {
        delayStartPulse += 900000; // 15 mins - normal value
        storePulseCount();
    }
}
void XcisBore::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[26];

    Serial.print("XcisBore::processMessage:");
    xcisMessage.dumpHex(data,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    xcisMessage.processMessage(data);
    Serial.print(" LocationID:");
    Serial.print(xcisMessage.getLocationID(),HEX);
    Serial.print(" DeviceType:");
    Serial.print(xcisMessage.getDeviceType(),HEX);
    Serial.print(" Command:");
    Serial.print(xcisMessage.getCommand(),HEX);
    Serial.print(" CRC:");
    Serial.print(xcisMessage.getCRC(),HEX);
    xcisMessage.getPayload(recvPayload);
    Serial.print(" Payload:");
    xcisMessage.dumpHex(recvPayload,26);
    if (xcisMessage.getCommand() == SENSOR_DATA_REQUEST)
    { 
      Serial.println("Received:SENSOR_DATA_REQUEST");
    
      Serial.print("Battery value:");
      uint16_t battery = getVoltageBytes();
      Serial.println(battery,HEX);
      readCurrentValue();
      Serial.print("Current value:");
      Serial.println(currentValue,HEX);
      Serial.print("Bore State:");
      Serial.println(boreRunning,HEX);

      Serial.print("Pulses Integer:");
      Serial.println(accumulatedPulses);
      Serial.print("Pulses Hex:");
      Serial.println(accumulatedPulses,HEX);
      
      Serial.print("accumulatedDataToken Integer:");
      Serial.println(accumulatedDataToken);
      Serial.print("accumulatedDataToken Hex:");
      Serial.println(accumulatedDataToken,HEX);

      xcisMessage.createBorePayload(SENSOR_DATA_RESPONSE, battery, currentValue, accumulatedPulses, accumulatedDataToken, boreRunning);  
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), BORE_CONTROLLER, SENSOR_DATA_RESPONSE);
  
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
    if (xcisMessage.getCommand() == CONTROL_ON)
    {
      Serial.println("Received:CONTROL_ON");
      sensor_control_data control;
      xcisMessage.processControlPayload(control);
      Serial.println(control.value); 
      duration = control.value;
      turnOn();
    }
    if (xcisMessage.getCommand() == CONTROL_OFF)
    {
      Serial.println("Received:CONTROL_OFF");
      sensor_control_data control;
      xcisMessage.processControlPayload(control);
      Serial.println(control.value); 
      duration = control.value;
      turnOff();
    }
}
 void XcisBore::readCurrentValue()
 {
    Serial.print("XcisBore::readCurrentValue:");
    // Code to get the analog input value measuring current
    currentValue = analogRead(CURRENT); // Read current
    currentValue = 0; // Not working
    Serial.println(currentValue);
 }
void XcisBore::turnOn()
{
    Serial.println("XcisBore::turnOn:");
    // Code to switch the bore 
    digitalWrite(ON_RELAY, HIGH);         // turn the Bore Shield Relay (D22) ON
    delay(duration);
    digitalWrite(ON_RELAY, LOW);         // turn the Bore Shield Relay (D22) off
    boreRunning = 1; // Need to override this with a test on startup to check on bore state. Use Current?
}
void XcisBore::turnOff()
{
     Serial.println("XcisBore::turnOff:");
     // Code to switch the bore OFF
      digitalWrite(OFF_RELAY, HIGH);         // turn the Bore Shield Relay (D23) ON
      delay(duration);
      digitalWrite(OFF_RELAY, LOW);         // turn the Bore Shield Relay (D23) off
      boreRunning = 0;
}
void XcisBore::countPulses()
{
    pulseCount++;
}
void XcisBore::storePulseCount()
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