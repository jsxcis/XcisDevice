// Xcis Flow Meter
#include "XcisWeather.h"

SoftwareSerial usonic_weather(SERIAL_RX, SERIAL_TX); //SDA,SCL-  RX, TX
Adafruit_BME280 bme; // Create an instance of the BME280 sensor

XcisWeather::XcisWeather()
{
    delayStart = 0;   // start delay
    delayRunning = 0; // not finished yet
}
void XcisWeather::initialise()
{
    Serial.println("XcisWeather::initialise new2");
    delayStart = millis();   // start delay
    delayRunning = true; // not finished yet
  
    bool bmeStatus = bme.begin(BME280_ADDRESS);
    if (!bmeStatus) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1); // Halt if sensor not found
    }
    Serial.println("BME280 sensor initialized successfully.");
}
void XcisWeather::execute()
{
    
    if (delayRunning && ((millis() - delayStart) >= 1000))// 100 ms  
    {
        delayStart +=1000; // 100 ms
        // Send hunting message - looking for a gateway
        //readDataStream();
    }
}
void XcisWeather::execute(int mode)
{
    if (delayRunning && ((millis() - delayStart) >= 10000))// 10 secs 
    {
        delayStart +=10000; // 10 secs
        // Send hunting message - looking for a gateway
        Serial.println("Sending hunting message");
    }
}
void XcisWeather::processMessage(uint8_t *data , uint8_t *responseData)
{
    uint8_t recvPayload[28];


    Serial.print("XcisWeather::processMessage:");
    
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
    
 
      // Read the sensor data
      Serial.print("Temperature = ");
      uint16_t temperature = getTemperatureBytes();
      Serial.println(temperature,HEX);  
      Serial.print("Pressure = ");
      uint16_t pressure = getPressureBytes();
     
      Serial.println(pressure,HEX);
      Serial.print("Humidity = ");
      uint16_t humidity = getHumidityBytes();
      Serial.println(humidity,HEX);
      Serial.print("Altitude = ");
      uint16_t altitude = getAltitudeBytes();
      Serial.println(altitude,HEX);

      // Create the response payload
      xcisMessage.createWeatherPayload(SENSOR_DATA_RESPONSE, battery, temperature, humidity, pressure, altitude); 
      // Create the full message
      // Note: The location ID is set in the xcisMessage object, so we use    
      xcisMessage.createMessage(responseData,xcisMessage.getLocationID(), WEATHER_SENSOR, SENSOR_DATA_RESPONSE);
      
      Serial.print("Response:");
      xcisMessage.dumpHex(responseData,XCIS_RH_MESH_MAX_MESSAGE_LEN);
    }
    if (xcisMessage.getCommand() == SET_SENSOR_LORAID)
    {
      Serial.println("XcisWeather::processMessage:SET_SENSOR_LORAID");
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

void XcisWeather::readDataStream()
{
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVEL_PRESSURE_HPA));
  Serial.println(" m");
  
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  
  Serial.println();
 

}