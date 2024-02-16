
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
enum test_case {VOLTAGE, CAN, FAN, GPIO, TEENSY};
test_case debug = VOLTAGE;

CANLine can;
short message[8] = {60000,4,0,0,0,0,0,0};
std::vector<byte> pong;

cell_asic IC[TOTAL_IC];

fanController fans(&Serial8);
bool test_bool[10] = {0,0,0,0,0,0,0,0,0,1};

void setup() {
  Serial.begin(115200);
  // fans.begin();
  adBms6830_init_config(TOTAL_IC, &IC[0]);
  
}

void loop() {
  switch (debug)
  {
  case VOLTAGE:
    //expected result: print out the cell voltages
    adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
    //for some reason this doesn't work, why not?
    adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);
    adBms6830_read_cell_voltages(TOTAL_IC, &IC[1]);
    break;

  case CAN:  
      //sends Precharge stuff to VDM, expects a response back of some kind
      can.send(97, message);
      can.recieve_one();
      pong = can.recieve(97);
      break;

  case FAN:
    //dunno yet
    break;

  case GPIO:
    adbms6830_write_gpio(TOTAL_IC, &IC[0], test_bool);
    //start aux voltage measurement sets all the GPIO pins to low, this is adjustable in the code
    adBms6830_start_aux_voltage_measurment(TOTAL_IC, &IC[0]);
    adBms6830_read_aux_voltages(TOTAL_IC, &IC[0]);
    break;

  case TEENSY:
    Serial.println("Teensy is probably not the issue");
    break;

  default:
    Serial.println("Uh oh u dummy u didn't set what to debug");
    delay(1000);
    break;
  }

  
  
}

void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}

