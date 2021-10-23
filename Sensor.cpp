#include "Sensor.h"
Sensor::Sensor()
{


}
void Sensor::sayHello()
{
    Serial.println("Sensor::Hello");
}
uint16_t Sensor::getVoltageBytes()
{
    uint16_t batteryVoltage = 0; //
    batteryVoltage = (analogRead(BATTERY_VOLTAGE) * 0.006325) * 100;
    //batteryVoltage = (512 * 0.006325) * 100;
    return batteryVoltage;
}