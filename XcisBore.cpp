// Xcis Flow Meter
#include "XcisBore.h"

XcisBore::XcisBore()
{
    currentValue = 0;
    // Need to override this with a test on startup to check on bore state. Use Current?
    local_boreState = 0;
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

    pulseCheckCounter = 0;
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
    pinMode(BORE_OFF_LED,OUTPUT);
    pinMode(BORE_ON_LED,OUTPUT);

    digitalWrite(BORE_ON_LED,1); // LED OFF
    digitalWrite(BORE_OFF_LED,1); // LED OFF
    pulseCount = 0x00;
    currentValue = 0;
    local_boreState = 0;
    boreStatus = 0;
    pulseCheckCounter = 0;
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
            pulseCheckCounter = 0;
            countPulses();
        }
    }
    pulseCheckCounter++;
    if (pulseCheckCounter > 5000)
    {
        // No pulse received for a while - store what I have and reset
        Serial.println("RESETTING PULSE COUNT DUE TO INACTIVITY");
        pulseCount = 0;
        pulseCheckCounter = 0;
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
    readCurrentValue();
    calculateStatus();
    displayStatus();
    
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
      Serial.print("Bore Status:");
      Serial.println(boreStatus,HEX);

      Serial.print("Pulses Integer:");
      Serial.println(accumulatedPulses);
      Serial.print("Pulses Hex:");
      Serial.println(accumulatedPulses,HEX);
      
      Serial.print("accumulatedDataToken Integer:");
      Serial.println(accumulatedDataToken);
      Serial.print("accumulatedDataToken Hex:");
      Serial.println(accumulatedDataToken,HEX);

      xcisMessage.createBorePayload(SENSOR_DATA_RESPONSE, battery, currentValue, accumulatedPulses, accumulatedDataToken, boreStatus);  
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
    if (xcisMessage.getCommand() == SET_SENSOR_LORAID)
    {
      Serial.println("XcisBore::processMessage:SET_SENSOR_LORAID");
      sensor_update_loraID_request update;
      uint32_t myUid;
      xcisMessage.processUpdatePayload(update);
      Serial.println(update.newLoraID,HEX);
      Serial.println(update.deviceUID,HEX);
      myUid =  Device::Instance()->getUID();
      Serial.println(myUid,HEX);
      if (myUid == update.deviceUID)
      {
        Serial.println("UID Match");
        Device::Instance()->setLoraID(update.newLoraID);
        // Need to restart the sensor to reset the radio
        Device::Instance()->initialise();
      }
      else
      {
        return;
      }
    } 
}
 void XcisBore::readCurrentValue()
 {
  
    // Code to get the analog input value measuring current
    float peakVoltage = 0;
    float voltageVirtualValue = 0;
    float ACCurrtntValue = 0;
    float powerFactor = 1.0;
    float acVoltage = 240;
    int rawValue = 0;
    int powerValue = 0;
    for (int i = 0; i < 5; i++)
    {
        peakVoltage += analogRead(CURRENT);   //read peak voltage                                                                      
        delay(1);
    }
    peakVoltage = peakVoltage / 5;
    voltageVirtualValue = peakVoltage * 0.707;
    voltageVirtualValue = (voltageVirtualValue / 1024 * VREF ) / 2; 
    ACCurrtntValue = voltageVirtualValue * ACTectionRange;
    float actualPower = 0;
    actualPower = powerFactor * ACCurrtntValue * acVoltage;
    powerValue = actualPower;
    currentValue = powerValue;
 }
 void XcisBore::readCurrentValue_debug()
 {
    Serial.println("XcisBore::readCurrentValue:");
    // Code to get the analog input value measuring current
    float peakVoltage = 0;
    float voltageVirtualValue = 0;
    float ACCurrtntValue = 0;
    float powerFactor = 1.0;
    float acVoltage = 240;
    int rawValue = 0;
    int powerValue = 0;
    for (int i = 0; i < 5; i++)
    {
        peakVoltage += analogRead(CURRENT);   //read peak voltage                                                                      
        delay(1);
    }
    peakVoltage = peakVoltage / 5;
    Serial.print("peakVoltage:");
    Serial.println(peakVoltage); 
    voltageVirtualValue = peakVoltage * 0.707;
    Serial.print("voltageVirtualValueRaw:");
    Serial.println(voltageVirtualValue);
    voltageVirtualValue = (voltageVirtualValue / 1024 * VREF ) / 2; 
    ACCurrtntValue = voltageVirtualValue * ACTectionRange;
    float actualPower = 0;
    actualPower = powerFactor * ACCurrtntValue * acVoltage;
    Serial.print("ACCurrtntValue:");
    Serial.println(ACCurrtntValue);
    Serial.print("actualPower:");
    Serial.println(actualPower);
    powerValue = actualPower;
    currentValue = powerValue;
    //currentValue = voltageVirtualValue * 1000;
    Serial.print("CurrentValue Decimal:");
    Serial.println(currentValue);

 }
void XcisBore::turnOn()
{
    Serial.println("XcisBore::turnOn:");
    // Code to switch the bore 
    digitalWrite(ON_RELAY, HIGH);         // turn the Bore Shield Relay (D22) ON
    delay(duration);
    digitalWrite(ON_RELAY, LOW);         // turn the Bore Shield Relay (D22) off
    local_boreState = 1; // THIS IS ON STATE
    boreStatus = 5; // STARTING STATUS
    //digitalWrite(BORE_ON_LED,0); // LED ON
    //digitalWrite(BORE_OFF_LED,1); // LED OFF
}
void XcisBore::turnOff()
{
     Serial.println("XcisBore::turnOff:");
     // Code to switch the bore OFF
    digitalWrite(OFF_RELAY, HIGH);         // turn the Bore Shield Relay (D23) ON
    delay(duration);
    digitalWrite(OFF_RELAY, LOW);         // turn the Bore Shield Relay (D23) off
    local_boreState = 0; // THIS IS STOPPED STATE
    boreStatus = 6; // STOPPING STATUS
    //digitalWrite(BORE_ON_LED,1); // LED OFF
    //digitalWrite(BORE_OFF_LED,0); // LED O
}
void XcisBore::countPulses()
{
    pulseCount++;
    Serial.print("Bore Status:");
    Serial.println(boreStatus);
    Serial.print("PulseCount:");
    Serial.println(pulseCount);
    Serial.print("CurrentValue:");
    Serial.println(currentValue);
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
// if the bore is running 0 = off, 1= running, 2 = run_err_current, 3 = run_err_flow, 4 = run_err
void XcisBore::calculateStatus()
{
    if (/*(local_boreState == 0) && */((currentValue < CURRENT_THRESHOLD) && (pulseCount == 0)))  // STOPPED
    {
        boreStatus = 0;
        // Could be running but an input fault - so leave local_boreState at 0 or 1, set only by inbound command
    }
    if (/*(local_boreState == 0) && */ ((currentValue > CURRENT_THRESHOLD) || (pulseCount > 0)))  // JUST STARTED STATE BY LOCAL CONTROL
    {
        boreStatus = 1; 
    }
 
    if (/*(local_boreState == 1) &&*/ ((currentValue < CURRENT_THRESHOLD) && (pulseCount > 0)))  // RUNNING OK CURRENT_ERR 101
    {
        boreStatus = 2; 
    }
    if (/*(local_boreState == 1) &&*/ ((currentValue > CURRENT_THRESHOLD) && (pulseCount == 0)))  // RUNNING OK FLOW_ERR 110
    {
        boreStatus = 3;
    }
}
void XcisBore::displayStatus()
{

    if (boreStatus == 0) // STOPPED
    {
        digitalWrite(BORE_ON_LED,1); // LED OFF
        digitalWrite(BORE_OFF_LED,0); // LED ON
    }
    else if(boreStatus == 1) // RUNNING OK
    {
        digitalWrite(BORE_ON_LED,0); // LED ON
        digitalWrite(BORE_OFF_LED,1); // LED OFF
    }
    else if(boreStatus == 2) // RUNNING WITH CURRENT ERR
    {
        digitalWrite(BORE_ON_LED,0); // LED ON
        digitalWrite(BORE_OFF_LED,0); // LED  ON
    }
    else if(boreStatus == 3) // RUNNING with FLOW ERR
    {
        digitalWrite(BORE_ON_LED,0); // LED ON
        digitalWrite(BORE_OFF_LED,0); // LED ON
    }
    else if(boreStatus == 4) // RUNNING with GENERAL ERR (should flash) current and flow not working
    {
        digitalWrite(BORE_ON_LED,0); // LED ON 
        digitalWrite(BORE_OFF_LED,0); // LED ON
    }

}