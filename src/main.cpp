
#include <Arduino.h>
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "adbms_main.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"

// put function declarations here:
void intrFunc();
void wakeBms();

// Object declarations 
//isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
//isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);
#define TOTAL_IC 1
cell_asic IC[TOTAL_IC];
fanController fans(&Serial8);

void setup() {
  // put your setup code here, to run once:
  //set_arm_clock(24000000);
  Serial.begin(115200);
  fans.begin();
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
  adBms6830_init_config(TOTAL_IC, &IC[0]);
}

cell_asic test;
void loop() {
  Serial.println("PLEASE WORK");
  uint8_t number = 45 + sin(millis()/10000.0) * 25;
  fans.writeRegister(0, number);
  Serial.println(fans.readRegister(0x00) * 50);
  //Serial.println(fans.writeRegister(0, number) ? "Write fan success" : "Write fan failed");
  Serial.println(fans.readRegister(0x18) * 50); // read rpm
  Serial.println(fans.readRegister(0x14) * 0.06132665832290363); // read voltage, will add register definitions later
  //SPI.beginTransaction(SPISettings(SPI_MODE3, MSBFIRST, 1000000));
  // put your main code here, to run repeatedly:
  //uint16_t number = 0b1010101010101010;
  //isoSPI1.beginTransaction(SPI_MODE3, 2000000);
  //isoSPI1.transfer16(number);
  //isoSPI1.endTransaction();
  //adbms_main();
  //adBmsWakeupIc(1);
  // run_command(3);
  // run_command(4);
  delay(100);
  // run_command(21);
  //run_command(11);
  //run_command(12);
  // for (uint8_t i = 0; i < 20; i++) {
  //   run_command(i);
  //   //delay(1000);
  // }
  //delay(1000);
  // run_command(4);
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