// XcisTestMode
#include "XcisTestMode.h"

XcisTestMode::XcisTestMode()
{
    currentValue = 0;
    // Need to override this with a test on startup to check on bore state. Use Current?
    boreState = 0;
    boreStatus = 0;
    delayStart = 0;   // start delay
    delayStartPulse = 0;   // start delay
    delayRunning = 0; // not finished yet
    inputState_D20 = 0;
    lastInputState_D20 = 1; // active low
    inputState_D18 = 0;
    lastInputState_D18 = 1; // active low
    inputState_D19 = 0;
    lastInputState_D19 = 1; // active low

    pulseCount = 0x00;
    accumulatedPulses = 0x0000;
    accumulatedDataToken = 0x00000000;

    duration = 2000;
}
void XcisTestMode::initialise()
{
    Serial.println("XcisTestMode::initialise");
    delayStart = millis();   // start delay
    delayStartPulse = millis();   // start delay
    delayRunning = true; // not finished yet

    pinMode(PULSE,INPUT); // Pulse input
    pinMode(CURRENT,INPUT); // Current sensor input
    pinMode(ON_RELAY, OUTPUT); //RELAY (D22) NOTE:(D22) on PCB
    pinMode(OFF_RELAY, OUTPUT); //RELAY (D23) NOTE:(D23) on PCB
    pinMode(BORE_ON_SW,INPUT); // Bore On Input
    pinMode(BORE_OFF_SW, INPUT); // Bore Off Input
    pinMode(BORE_OFF_LED,OUTPUT);
    pinMode(BORE_ON_LED,OUTPUT);

    digitalWrite(BORE_ON_LED,1); // LED OFF
    digitalWrite(BORE_OFF_LED,1); // LED OFF
    pulseCount = 0x00;
    currentValue = 0;
    boreState = 0;
    boreStatus = 0;
}

void XcisTestMode::execute()
{
    // FLOW PULSE INPUT
    inputState_D20 = digitalRead(PULSE);
    if (inputState_D20 != lastInputState_D20)
    {
        if (inputState_D20 == 1)
        {
            Serial.println("Got pulse on D20");
        }
    }
    lastInputState_D20 = inputState_D20;
    // BORE ON SW
    inputState_D18 = digitalRead(BORE_ON_SW);
    if (inputState_D18 != lastInputState_D18)
    {
        if (inputState_D18 == 1)
        {
            Serial.println("Got BORE ON SW D18");
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
            Serial.println("Got BORE OFF SW D19");
            turnOff();
        }
    }
    lastInputState_D19 = inputState_D19;
    sequenceLeds();
    sequenceRelays();
}
void XcisTestMode::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[26];

    Serial.print("XcisTestMode::processMessage:");
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
 void XcisTestMode::readCurrentValue()
 {
    Serial.print("XcisTestMode::readCurrentValue:");
    // Code to get the analog input value measuring current
    float peakVoltage = 0;
    float voltageVirtualValue = 0;
    int rawValue = 0;
    for (int i = 0; i < 5; i++)
    {
        peakVoltage += analogRead(CURRENT);   //read peak voltage
        delay(1);
    }
    peakVoltage = peakVoltage / 5; 
    voltageVirtualValue = peakVoltage * 0.707;
    rawValue = analogRead(A4);
    Serial.println(rawValue);
    Serial.println(voltageVirtualValue);
    currentValue = voltageVirtualValue * 1000;
    //currentValue = analogRead(CURRENT); // Read current
    //currentValue = 100; // Not working
    Serial.println(currentValue);

 }
void XcisTestMode::turnOn()
{
    Serial.println("XcisTestMode::turnOn:");
    // Code to switch the bore 
    digitalWrite(ON_RELAY, HIGH);         // turn the Bore Shield Relay (D22) ON
    delay(duration);
    digitalWrite(ON_RELAY, LOW);         // turn the Bore Shield Relay (D22) off
    boreState = 1; // Need to override this with a test on startup to check on bore state. Use Current?
    //digitalWrite(BORE_ON_LED,0); // LED ON
    //digitalWrite(BORE_OFF_LED,1); // LED OFF
}
void XcisTestMode::turnOff()
{
     Serial.println("XcisTestMode::turnOff:");
     // Code to switch the bore OFF
    digitalWrite(OFF_RELAY, HIGH);         // turn the Bore Shield Relay (D23) ON
    delay(duration);
    digitalWrite(OFF_RELAY, LOW);         // turn the Bore Shield Relay (D23) off
    boreState = 0;
    //digitalWrite(BORE_ON_LED,1); // LED OFF
    //digitalWrite(BORE_OFF_LED,0); // LED O
}
void XcisTestMode::sequenceRelays()
{
    turnOn();
    turnOff();
}
void XcisTestMode::sequenceLeds()
{
    // All off 
    digitalWrite(BORE_ON_LED,1); // LED OFF
    digitalWrite(BORE_OFF_LED,1); // LED OFF
    digitalWrite(LORA_LED,1); // LED
    digitalWrite(STATUS_LED,1); // STATUS LED - DEVICE INITIIALISED
    digitalWrite(SENSOR_STATUS_LED, 1); // STATUS LED - DEVICE ACTIVE - PWR ON

    // Start sequence
    
    delay(1000);
    digitalWrite(BORE_ON_LED,0); // LED OFF
    delay(1000);
    digitalWrite(BORE_ON_LED,1); // LED OFF
    delay(1000);
    digitalWrite(BORE_OFF_LED,0); // LED OFF
    delay(1000);
    digitalWrite(BORE_OFF_LED,1); // LED OFF
    delay(1000);
    digitalWrite(LORA_LED,0); // LED OFF
    delay(1000);
    digitalWrite(LORA_LED,1); // LED
    delay(1000);
    digitalWrite(STATUS_LED,0); // STATUS LED - DEVICE INITIIALISED
    delay(1000);
    digitalWrite(STATUS_LED,1); // STATUS LED - DEVICE INITIIALISED
    delay(1000);
    digitalWrite(SENSOR_STATUS_LED, 0); // STATUS LED - DEVICE ACTIVE - PWR ON
    delay(1000);
    digitalWrite(SENSOR_STATUS_LED, 1); // STATUS LED - DEVICE ACTIVE - PWR ON
    delay(1000);

    // All off 
    digitalWrite(BORE_ON_LED,1); // LED OFF
    digitalWrite(BORE_OFF_LED,1); // LED OFF
    digitalWrite(LORA_LED,1); // LED
    digitalWrite(STATUS_LED,1); // STATUS LED - DEVICE INITIIALISED
    digitalWrite(SENSOR_STATUS_LED, 1); // STATUS LED - DEVICE ACTIVE - PWR ON
    
}