#include "ADBMS.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"
#include "ACU.h"

// put function declarations here:


// ACU variables
uint16_t cellVoltage[128]; //0.1mV
float cellTemp[128][2]; // C
float balTemp[128]; // C
float maxCellTemp, maxBalTemp;

float accumVoltage, accumCurrent, tsVoltage;
float acuTemp[3]; // DC-DC converter, something, something

uint16_t fanRpm[4];
float fanVoltage[4];
float fanCurrent[4];

bool tsActive = false;
uint8_t errors = 0b00000000; // BMS error, Temperature error, voltage error


enum test_case {VOLTAGE, CAN, FAN, GPIO, TEENSY, CELLBAL, EXTRA};
test_case debug = FAN;

CANLine can;
short message[8] = {60000,4,0,0,0,0,0,0};
std::vector<byte> pong;

cell_asic IC[2];

fanController fans(&Serial8);
bool test_bool[10] = {0,0,0,0,0,0,0,0,0,1};

uint8_t Wrpwm1[2] = { 0x00, 0x20 };
uint8_t Wrpwm2[2] = { 0x00, 0x21 };
uint8_t Wrcfgb[2] = { 0x00, 0x24 };
uint8_t Wrcfga[2] = { 0x00, 0x01 };

States mockState = STANDBY; // testing purposes

void setup() {
  Serial.begin(115200);
  fans.begin();
  Serial.println("Init config");
  adBmsWakeupIc(TOTAL_IC);
  adBms6830_init_config(TOTAL_IC, &IC[0]);
  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
}

uint8_t count = 0;
uint32_t lastCountTime = 0;

void loop() {
  //Serial.println("PLEASE WORK");
  count++;
  if(count == 255){
    Serial.printf("Loop rate: %fHz\n", 1e6 / (float(micros() - lastCountTime) / 256));
    lastCountTime = micros();
  }
  switch (debug)
  {
    case VOLTAGE:
      //expected result: print out the cell voltages
      adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
      //for some reason this doesn't work, why not?
      adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);
      printVoltages(TOTAL_IC, IC, Cell);
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
      updateVoltage(cellVoltage, IC);
      for (uint8_t i = 0; i < 32; i++) {
        Serial.printf("C%u: %dmV ", i+1, cellVoltage[i]/10);
      }
      Serial.println();
      dumpCANbus(&can, cellVoltage);
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
