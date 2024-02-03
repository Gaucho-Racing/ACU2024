
#include <Arduino.h>
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "adbms_main.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"

// put function declarations here:
void wakeBms();

// Object declarations 
//isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
//isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);
cell_asic IC[TOTAL_IC];
fanController fans(&Serial8);

void setup() {
  Serial.begin(115200);
  // fans.begin();
  adBms6830_init_config(TOTAL_IC, &IC[0]);
}

cell_asic test;
void loop() {
  delay(1000);
  adBmsWakeupIc(1);
  run_command(11);
  run_command(12);
}

void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}