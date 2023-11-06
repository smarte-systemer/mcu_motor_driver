#include <Arduino.h>

uint8_t binaryPins []= {2,3,4,5};


void setup() 
{
  Serial.begin(115200);
  for(uint8_t i = 0; i<sizeof(binaryPins); i++)
  {
    pinMode(binaryPins[i], OUTPUT);
    digitalWrite(binaryPins[i], HIGH);
    delay(200);
    digitalWrite(binaryPins[i], LOW);
  }

}

void decToBinaryPins(uint8_t dec, uint8_t delayInterval=0)
{
  for(uint8_t bitNumber = 0; bitNumber<sizeof(binaryPins); bitNumber++)
  {
    digitalWrite(binaryPins[bitNumber], bitRead(dec, bitNumber));
    delay(delayInterval*1000);
  }
}

void loop() 
{
  decToBinaryPins(0b0011);//Example three decimal
  for (int i= 0; i<5; i++)
  {
    decToBinaryPins(i);
    delay(1000);
  }   
}

