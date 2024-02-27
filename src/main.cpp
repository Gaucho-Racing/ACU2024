
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"
#include "ACU.h"
#include "can.cpp"
$include "main.cpp"
using namespace std;

// put function declarations here:
void wakeBms();
void printPWM(uint8_t tIC, cell_asic *IC);

// Object declarations 
//isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
//isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);
enum test_case {VOLTAGE, CAN, FAN, GPIO, TEENSY, CELLBAL, EXTRA};
test_case debug = CELLBAL;

CANLine can;
short message[8] = {60000,4,0,0,0,0,0,0};
std::vector<byte> pong;

#define TOTAL_IC 2
cell_asic IC[TOTAL_IC];

fanController fans(&Serial8);
bool test_bool[10] = {0,0,0,0,0,0,0,0,0,1};

uint8_t Wrpwm1[2] = { 0x00, 0x20 };
uint8_t Wrpwm2[2] = { 0x00, 0x21 };
uint8_t Wrcfgb[2] = { 0x00, 0x24 };
uint8_t Wrcfga[2] = { 0x00, 0x01 };

States mockState = FIRST; // testing purposes

void setup() {
  Serial.begin(115200);
  fans.begin();
  Serial.println("Init config");
  adBms6830_init_config(TOTAL_IC, &IC[0]);
  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
}

void loop() {
  switch (debug)
  {
  case VOLTAGE:
    //expected result: print out the cell voltages
    adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
    //for some reason this doesn't work, why not?
    adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);
    // adBms6830_read_cell_voltages(TOTAL_IC, &IC[1]);
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

  case CELLBAL:
    //test discharge first
    Serial.println("Set duty cycle");
    SetPwmDutyCycle(TOTAL_IC, &IC[0], PWM_100_0_PCT);
    Serial.println("Create PWMs");
    adBms6830CreatePwma(TOTAL_IC, &IC[0]);
    adBms6830CreatePwmb(TOTAL_IC, &IC[0]);
    Serial.println("Write data");
    adBmsWriteData(TOTAL_IC, &IC[0], Wrpwm1, Pwm, AA);
    adBmsWriteData(TOTAL_IC, &IC[0], Wrpwm2, Pwm, BB);
    printPWM(TOTAL_IC, &IC[0]);
    

    break;
  default:
    Serial.println("Uh oh u dummy u didn't set what to debug");
    break;
  }
  
  // ACU STATES
  switch (mockState)
  {
    case STANDBY:
      standByState();
      break;
    case PRECHARGE:
      preChargeState();
      break;
    case CHARGE:
      chargeState();
      break;
    case NORMAL:
      normalState();
      break;
    case SHUTDOWN:
      shutdownState();
      break;
    default:
      mockState = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }

  delay(1000);
  
}
void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}
//will work for TotalIC = 1 and only to read PWM A
void printPWM(uint8_t tIC, cell_asic *IC) {
  
  Serial.println("PWM A :");
  for(int i = 0; i < RX_DATA; i++){
    Serial.printf("%u  ", IC[0].pwma.tx_data[i]);
  }
  Serial.println();
}