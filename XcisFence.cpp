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
    //sensor.begin(9600);
    pinMode(ADC_POSITIVE,INPUT); // Fence sensor input
    pinMode(ADC_NEGATIVE,INPUT); // Fence sensor input
    adc_pos = 0;
    adc_neg = 0;
    value = 0;

    pos_readIndex = 0;              // the index of the current reading
    pos_total = 0;                  // the running total
    pos_average = 0;                // the average
    pos_fenceVoltage = 0;

    neg_readIndex = 0;              // the index of the current reading
    neg_total = 0;                  // the running total
    neg_average = 0;                // the average
    neg_fenceVoltage = 0;
    // initialize all the readings to 0:
    for (int thisReading = 0; thisReading < numReadings; thisReading++)
    {
      pos_readings[thisReading] = 0;
      neg_readings[thisReading] = 0;
    }
}

void XcisFence::execute()
{
    // Old code for serial voltage converter
    //if (delayRunning && ((millis() - delayStart) >= 100))// 5 secs 
    //{
        //delayStart +=100; // 5 secs
        //readDataStream();
    //}

    // New code for XCIS fence device
    readPositive();
    readNegative();
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
      Serial.print(pos_fenceVoltage);
      Serial.print(",");
      Serial.print(neg_fenceVoltage);
      Serial.print(",");
      Serial.println(value);

      xcisMessage.createVoltagePayload(SENSOR_DATA_RESPONSE, battery,pos_fenceVoltage,neg_fenceVoltage);  
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), FENCE, SENSOR_DATA_RESPONSE);
  
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
   
}
double XcisFence::readFenceInput(readType toRead)
{
    max_value = 0;
    for (int counter = 0; counter < 20000; counter++ )
    {
      adc_pos = 0;
      adc_neg = 0;
      if (toRead == POS)
      {
        adc_pos = analogRead(ADC_POSITIVE);
        if (adc_pos > max_value)
        {
          max_value = adc_pos;
        }
      }
      else if (toRead == NEG)
      {
        adc_neg = analogRead(ADC_NEGATIVE);
        if (adc_neg > max_value)
        {
          max_value = adc_neg;
        }
      }     
    }
    //Serial.print("max_value:");
    return max_value;
}
void XcisFence::readPositive()
{
   // subtract the last reading:
  pos_total = pos_total - pos_readings[pos_readIndex];
  // read from the sensor:
  
  pos_readings[pos_readIndex] = readFenceInput(POS);
 
  // add the reading to the total:
  pos_total = pos_total + pos_readings[pos_readIndex];
  // advance to the next position in the array:
  pos_readIndex = pos_readIndex + 1;

  // if we're at the end of the array...
  if (pos_readIndex >= numReadings) {
    // ...wrap around to the beginning:
    pos_readIndex = 0;
  }
  // calculate the average:
  pos_average = pos_total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.print("Positive Average:");
  //Serial.println(pos_average);

  pos_fenceVoltage = 12000 * (pos_average/1023);
  //Serial.print("Positive Voltage:");
  //Serial.println(pos_fenceVoltage);
}
void XcisFence::readNegative()
{
    // subtract the last reading:
  neg_total = neg_total - neg_readings[pos_readIndex];
  // read from the sensor:
  
  neg_readings[neg_readIndex] = readFenceInput(NEG);
 
  // add the reading to the total:
  neg_total = neg_total + neg_readings[neg_readIndex];
  // advance to the next position in the array:
  neg_readIndex = neg_readIndex + 1;

  // if we're at the end of the array...
  if (neg_readIndex >= numReadings) {
    // ...wrap around to the beginning:
    neg_readIndex = 0;
  }

  // calculate the average:
  neg_average = neg_total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.print("Negative Average:");
  //Serial.println(neg_average);

  neg_fenceVoltage = 12000 * (neg_average/1023);
  //Serial.print("Negative Voltage:");
  //Serial.println(neg_fenceVoltage);
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
long XcisFence::readVcc() 
{
    long result;
    // Read 1.1V reference against AVcc
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);

    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA,ADSC));
    result = ADCL;
    result |= ADCH<<8;
    result = 1125300L / result; // Back-calculate AVcc in mV
    Serial.println(result);
    return result;
}