#include <Arduino.h>
#include <SPI.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World!");
}

void loop() {
  Serial.println("I wanna die!");
  delay(1000);
}
