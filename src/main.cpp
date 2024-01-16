#include <Arduino.h>
#include "ADBMS6822_Driver.h"

// put function declarations here:
void intrFunc();
void wakeBms();

// Object declarations 
isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  isoSPI1.begin();
  isoSPI1.setIntFunc(intrFunc);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t number = 0b1010101010101010;
  isoSPI1.beginTransaction(SPI_MODE3, 2000000);
  isoSPI1.transfer16(number);
  isoSPI1.endTransaction();
  delay(100);
}

// put function definitions here:
void intrFunc() {
  Serial.println("Interrupt!");
}

void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}