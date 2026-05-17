#ifndef XcisWeather_h
#define XcisWeather_h
#include "Sensor.h"
#include "Arduino.h"
#include <XcisMessage.h>
#include <SoftwareSerial.h>
#include "Device.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define SERIAL_RX 20 // CHIP PIN 26 GREEN
#define SERIAL_TX 21 // CHIP PIN 27 BLUE
#define SENSOR_PWR 23

#define SEALEVEL_PRESSURE_HPA (1013.25) // Adjust this value based on your location
#define BME280_ADDRESS 0x76 // Default I2C address for BME280



class XcisWeather : public Sensor {
    public:
        XcisWeather();
        void initialise();
        void execute();
        void execute(int mode);
        void processMessage(uint8_t *data, uint8_t *responseData);
        uint16_t getTemperatureBytes() {
            return (uint16_t)(bme.readTemperature() * 100); // Convert to centi-degrees
        }   
        uint16_t getPressureBytes() {
            return (uint16_t)(bme.readPressure() / 100.0F); // Convert to hPa
        }

        uint16_t getHumidityBytes() {
            return (uint16_t)(bme.readHumidity() * 100); // Convert to centi-percent
        }
        uint16_t getAltitudeBytes() {
            return (uint16_t)(bme.readAltitude(SEALEVEL_PRESSURE_HPA)); // Already metres
        }
        


    private:
        void readDataStream();
        uint16_t distance;

        unsigned long delayStart; // the time the delay started

        bool delayRunning; // true if still waiting for delay to finish
        Adafruit_BME280 bme; // Create an instance of the BME280 sensor

         
        //XcisMessage xcisMessage;

};
#endif