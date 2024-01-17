
#include <Arduino.h>
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "adbms_main.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"

// put function declarations here:
void intrFunc();
void wakeBms();

// Object declarations 
isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  isoSPI1.begin();
  isoSPI1.setIntFunc(intrFunc);

}

cell_asic test;
void loop() {
  // put your main code here, to run repeatedly:
  //uint16_t number = 0b1010101010101010;
  //isoSPI1.beginTransaction(SPI_MODE3, 2000000);
  //isoSPI1.transfer16(number);
  //isoSPI1.endTransaction();
  delay(100);
  //adbms_main();
  adBmsWakeupIc(1);
  //adBms6830_cell_openwire_test(1,);
  //printOpenWireTestResult(1, &test, AvgCell);
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