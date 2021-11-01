// UID Class
#include "UID.h"

UID::UID()
{


}
void UID::initialise()
{
    Serial.println("UID::initialiseUID");

    pinMode(DATAOUT, OUTPUT);
    pinMode(DATAIN, INPUT);
    pinMode(SPICLOCK,OUTPUT);
    pinMode(SLAVESELECT,OUTPUT); 
    digitalWrite(SLAVESELECT,HIGH); //disable device
    SPI.begin();
}
void UID::readUIDManufacturer()
{
    unsigned int addr;
    byte one;
    byte two;
    byte three;
    byte four;
    Serial.println("Read Manufacturer");
    digitalWrite(SLAVESELECT, LOW);

    /* transmit read id command - jedec does not require 3 dummy bytes */
    SPI.transfer(READ);
    delay(10);
  
    SPI.transfer(0xFA);
    delay(10);
    one = SPI.transfer(0x00);
    Serial.println(one,HEX);
    digitalWrite(SLAVESELECT, HIGH);

    delay(10);
 
    digitalWrite(SLAVESELECT, HIGH);
}
void UID::readDeviceCode()
{
    unsigned int addr;
    byte one;
    byte two;
    byte three;
    byte four;
    Serial.println("Read Device Code");
    digitalWrite(SLAVESELECT, LOW);

    /* transmit read id command - jedec does not require 3 dummy bytes */
    SPI.transfer(READ);
    delay(10);
    
    SPI.transfer(0xFB);
    delay(10);
    one = SPI.transfer(0x00);
    Serial.println(one,HEX);
    digitalWrite(SLAVESELECT, HIGH);

    delay(10);
    
    digitalWrite(SLAVESELECT, HIGH);
}
void UID::readStatusReg()
{
  byte resp;
  digitalWrite(SLAVESELECT, LOW);
  SPI.transfer(RDSR); // returns one byte
  delay(10);

  resp = SPI.transfer(0x00);
  Serial.println("Status Register");
  Serial.print("bit 0 - WIP\t");
  Serial.println(bitRead(resp, 0));
  Serial.print("bit 1 - WEL\t");
  Serial.println(bitRead(resp, 1));
  Serial.print("bit 2 - BP0\t");
  Serial.println(bitRead(resp, 2));
  Serial.print("bit 3 - BP1\t");
  Serial.println(bitRead(resp, 3));
  Serial.print("bit 4 -\t");
  Serial.println(bitRead(resp, 4));
  Serial.print("bit 5 -\t");
  Serial.println(bitRead(resp, 5));
  Serial.print("bit 6 -\t");
  Serial.println(bitRead(resp, 6));
  Serial.print("bit 7 -\t");
  Serial.println(bitRead(resp, 7));
  digitalWrite(SLAVESELECT, HIGH);
}

uint32_t UID::readID()
{
  unsigned int addr;
  unsigned int one;
  unsigned int two;
  unsigned int three;
  unsigned int four;
  unsigned int five;
  unsigned int six;
  uint8_t data[8];
  uint32_t result;

  Serial.println("Read Serial number memory map");
 
  digitalWrite(SLAVESELECT, LOW);

  SPI.transfer(READ);
  delay(10);  
  SPI.transfer(0xFA);
  delay(10);

  one = SPI.transfer(0x00);
  data[0] = one;
  //Serial.println(one,HEX);
  
  two = SPI.transfer(0x00);
  data[1] = two;
  //Serial.println(two,HEX);
  
  three = SPI.transfer(0x00);
  data[2] = three;
  //Serial.println(three,HEX);
  
  four = SPI.transfer(0x00);
  data[3] = four;
  //Serial.println(four,HEX);
  
  five = SPI.transfer(0x00);
  //Serial.println(five,HEX);
  data[4] = five;
  
  six = SPI.transfer(0x00);
  //Serial.println(six,HEX);
  data[5] = six;

  result = (uint32_t)data[2] << 24 |
           (uint32_t)data[3] << 16 |
           (uint32_t)data[4] << 8 |
           (uint32_t)data[5];

  Serial.println(result,HEX);
    
  digitalWrite(SLAVESELECT, HIGH);
  return result;

}
