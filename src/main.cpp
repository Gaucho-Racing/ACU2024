#include "main.h"

void setup() {
  Serial.begin(115200);
  //initialize SPI:
  SPI.begin();
  
  //end SPI:
  SPI.end();
}

void loop() {
  Serial.println("I wanna die!");
  delay(1000);
}
