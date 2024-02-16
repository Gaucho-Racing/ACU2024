
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "ACU.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"
#include "can.cpp"

// put function declarations here:
void wakeBms();

// Object declarations 
//isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
//isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);

CANLine can;
cell_asic IC[TOTAL_IC];
fanController fans(&Serial8);
bool test_bool[10] = {0,0,0,0,0,0,0,0,0,1};

void setup() {
  Serial.begin(115200);
  // fans.begin();
  adBms6830_init_config(TOTAL_IC, &IC[0]);
  
}
std::vector<byte> pong;
cell_asic test;
void loop() {

  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
  //for some reason this doesn't work, why not?
  adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);
  adBms6830_read_cell_voltages(TOTAL_IC, &IC[1]);
  
}

void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}

