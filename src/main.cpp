#include <Arduino.h>
#include "ADBMS.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World!");
}

void loop() {
  Serial.println("I love Analog Devices");
  delay(1000);
}
