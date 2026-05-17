// Xcis Flow Meter
#include "XcisWindSensor.h"

// Global variables for the wind sensor

volatile bool IsSampleRequired; // this is set true every 2.5s. Get wind speed 
volatile unsigned int TimerCount; // used to determine 2.5sec timer count 
volatile unsigned long Rotations; // cup rotation counter used in interrupt routine 
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in isr 

void isr_rotation()
{
    // This is called every time the wind sensor rotation pin goes low
    // Increment the rotation count
    if((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact. 
    Rotations++; 
    ContactBounceTime = millis(); 
  } 
}
isr_timer()
{
    TimerCount++; 
  
    if(TimerCount == 6) {
      IsSampleRequired = true; 
      TimerCount = 0; 
  } 
}
// Convert MPH to Knots 
float getKnots(float speed) { 
  return speed * 0.868976; 
} 

XcisWindSensor::XcisWindSensor()
{
    delayStart = 0;   // start delay
    delayRunning = 0; // not finished yet
}
void XcisWindSensor::initialise()
{
    Serial.println("XcisWindSensor::initialise");
    delayStart = millis();   // start delay
    delayRunning = true; // not finished yet

    lastWindDirectionValue = 0; 
  
    IsSampleRequired = false; 
  
    TimerCount = 0; 
    Rotations = 0; // Set Rotations to 0 ready for calculations 
  
    pinMode(WindSensorPin, INPUT); 

    //@@ Ooops, typo in this line from the original script
    //attachInterrupt(digitalPinToInterrupt(WindSensorPin), rotation, FALLING); 
    attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING); 
  
    // Setup the timer interupt 
    Timer1.initialize(500000);// Timer interrupt every 2.5 seconds 
    Timer1.attachInterrupt(isr_timer);
    
    Serial.println("Wind sensor initialized successfully.");
}
void XcisWindSensor::execute()
{
     getWindDirection();

     // Only update the display if change greater than 5 degrees. 
    if(abs(windCalDirection - lastWindDirectionValue) > 5) { 
      lastWindDirectionValue = windCalDirection; 
   } 

    if(IsSampleRequired) { 
      // convert to mp/h using the formula V=P(2.25/T) 
      // V = P(2.25/2.5) = P * 0.9 in mph
      // where P is the number of rotations in 2.5 seconds

      windSpeed = 1.6 * (Rotations * 0.9); // Convert to kph

      Rotations = 0; // Reset count for next sample 
    
      IsSampleRequired = false; 

      Serial.print(windSpeed); Serial.print(",");
      Serial.print(windCalDirection);  Serial.print(",");
      Serial.print(windCompassDirection);  Serial.print(",");
    
      Serial.println();
    
    }
}
void XcisWindSensor::execute(int mode)
{
    if (delayRunning && ((millis() - delayStart) >= 10000))// 10 secs 
    {
        delayStart +=10000; // 10 secs
        // Send hunting message - looking for a gateway
        Serial.println("Sending hunting message");
    }
}
void XcisWindSensor::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[28];


    Serial.print("XcisWindSensor::processMessage:");
    
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
      // Wind sensor data
      
      // Example values for wind speed and direction
      // These would be replaced with actual sensor readings
      //windSpeed = 30.0; // Example wind speed in mph  
      //windCalDirection = 246; // Example wind direction in degrees
      //windCompassDirection = "SE"; // Example wind direction as compass points


      //float windSpeed; // speed miles per hour
      Serial.print("Wind Speed:");
      Serial.println(windSpeed);
      Serial.print("Wind Speed Hex:");
      Serial.println((uint16_t)(windSpeed * 100),HEX);

      //int windCalDirection; // converted value with offset applied 
      Serial.print("Wind Direction:");    
      Serial.println(windCalDirection);
      Serial.print("Wind Direction Hex:");
      Serial.println(windCalDirection,HEX);

      //String windCompassDirection; // wind direction as compass points 
      Serial.print("Wind Compass Direction :0x");      
      Serial.print((uint16_t)windCompassDirection.c_str(),HEX);
      Serial.print(" Wind Compass Direction String:");
      Serial.print(windCompassDirection);
      Serial.print(" Wind Compass Direction Hex:");
      // Print each character in the string as hex
      Serial.print("0x");
      for (size_t i = 0; i < windCompassDirection.length(); i++)
      {
          Serial.print((uint16_t)windCompassDirection[i],HEX);
          if (i < windCompassDirection.length() - 1)
          {
              Serial.print(",");
          }
      }
      Serial.println();

       // WindSensor payload
      //typedef struct{
      //  uint16_t battery;
        //uint16_t windSpeed; // in 0.1 m/s
        //uint16_t windCalDirection; // in degrees
        //uint16_t windCompassDirection; // A string representation of the direction 
      //} wind_Sensor;
    
  
      // Create the response payload
      xcisMessage.createWindSensorPayload(SENSOR_DATA_RESPONSE, battery, windSpeed * 100, windCalDirection, (uint16_t)windCompassDirection.c_str());

      // Create the full message
      // Note: The location ID is set in the xcisMessage object, so we use    
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), WIND_SENSOR, SENSOR_DATA_RESPONSE);
      
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
    if (xcisMessage.getCommand() == SET_SENSOR_LORAID)
    {
      Serial.println("XcisWindSensor::processMessage:SET_SENSOR_LORAID");
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

// Get Wind Direction
void XcisWindSensor::getWindDirection() { 

  vaneValue = analogRead(WindVanePin); 
  windDirection = map(vaneValue, 0, 1023, 0, 360); 
  windCalDirection = windDirection + VaneOffset; 
  
  if(windCalDirection > 360) windCalDirection = windCalDirection - 360; 
  if(windCalDirection < 0) windCalDirection = windCalDirection + 360; 


  // get the compass direction for convenience , not sent in the message
  // This is a simple mapping of degrees to compass points
  if (windCalDirection < 22) windCompassDirection = "N";
  else if (windCalDirection < 67) windCompassDirection = "NE";
  else if (windCalDirection < 112) windCompassDirection = "E";
  else if (windCalDirection < 157) windCompassDirection = "SE";
  else if (windCalDirection < 212) windCompassDirection = "S";
  else if (windCalDirection < 247) windCompassDirection = "SW";
  else if (windCalDirection < 292) windCompassDirection = "W";
  else if (windCalDirection < 337) windCompassDirection = "NW";
  else windCompassDirection = "N";  

} 

