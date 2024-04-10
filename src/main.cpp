
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"
#include "can.cpp"
#include <unordered_map>

// put function declarations here:
void wakeBms();
void printPWM(uint8_t tIC, cell_asic *IC);
float V2T(float voltage, float B = 4390);

// Object declarations 
//isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
//isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);
enum test_case {VOLTAGE, CAN, FAN, GPIO, TEENSY, CELLBAL, THERMAL, EXTENDEDCELLBAL, EXTRA};
test_case debug = THERMAL;
enum test_case {VOLTAGE, CAN, FAN, GPIO, TEENSY, CELLBAL, THERMAL, EXTRA};
test_case debug = CELLBAL;
CANLine can;
short message[8] = {60000,4,0,0,0,0,0,0};
std::vector<byte> pong;
float temp[8][2];
uint16_t cell_to_mux[8];
// std::unordered_map<u_int8_t, u_int16_t> cell_to_mux;
unsigned long previousMillis = 0;

cell_asic IC[TOTAL_IC];

fanController fans(&Serial8);
bool test_bool[10] = {0,0,0,0,0,0,0,0,0,0};

uint8_t Wrpwm1[2] = { 0x00, 0x20 };
uint8_t Wrpwm2[2] = { 0x00, 0x21 };
uint8_t Wrcfgb[2] = { 0x00, 0x24 };
uint8_t Wrcfga[2] = { 0x00, 0x01 };

void setup() {
  Serial.begin(115200);
  fans.begin();
  adBmsSpiInit();
  Serial.println("Init config");
  adBms6830_init_config(TOTAL_IC, &IC[0]);
  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);

  cell_to_mux[1] = 0b0000100001;
  cell_to_mux[2] = 0b0001100001;
  cell_to_mux[3] = 0b0010100001;
  cell_to_mux[0] = 0b0011100001;
  cell_to_mux[4] = 0b0100100001;
  cell_to_mux[7] = 0b0101100001;
  cell_to_mux[5] = 0b0110100001;
  cell_to_mux[6] = 0b0111100001;

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

  case THERMAL:
    previousMillis = millis();
    for(int i = 0; i < 8; i++){
      IC[1].tx_cfga.gpo = cell_to_mux[i];
      
      // previousMillis = millis();
      
      adBmsWriteData(TOTAL_IC, IC, Wrcfga, Config, AA);

      // Serial.printf("Time (1): %lu\n", millis() - previousMillis);
      // previousMillis = millis();
      
      adBms6830_start_aux_voltage_measurment(TOTAL_IC, IC);
      adBms6830_read_aux_voltages(TOTAL_IC, IC);
      temp[i][0] = getVoltage(IC[1].aux.a_codes[0]);
      temp[i][1] = getVoltage(IC[1].aux.a_codes[5]);
    }
    Serial.printf("Time: %lu\n", millis() - previousMillis);
    //print out the temperatures
    for(int i = 0; i < 8; i++){
      Serial.printf("Cell %d: %f, Cell %d: %f\n", i+1, V2T(temp[i][1]), 8+i+1, V2T(temp[i][0]));
    }

    break;

  case TEENSY:
    Serial.println("Teensy is probably not the issue");
    break;

  case CELLBAL:
    //random non-cell related command to keep from going into extended
    adBms6830_read_s_voltages(TOTAL_IC, IC);
    
  case EXTENDEDCELLBAL: 
    //test discharge first
    Serial.println("Set duty cycle");
    SetPwmDutyCycle(TOTAL_IC, IC, PWM_100_0_PCT);
    Serial.println("Create PWMs");
    adBms6830CreatePwma(TOTAL_IC, IC);
    adBms6830CreatePwmb(TOTAL_IC, IC);
    Serial.println("Write data");
    adBmsWriteData(TOTAL_IC, IC, Wrpwm1, Pwm, AA);
    adBmsWriteData(TOTAL_IC, IC, Wrpwm2, Pwm, BB);
    printPWM(TOTAL_IC, IC);
    // IC[0].tx_cfgb.dcc = 0xFFFF;
    // run_command(22);
    adBms6830_write_read_config(TOTAL_IC, IC);


    delay(1000);
    break;
  default:
    Serial.println("Uh oh u dummy u didn't set what to debug");
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

float V2T(float voltage, float B = 4390){
  float R = voltage / ((5.0 - voltage) / 47e3) / 100e3;
  float T = 1.0 / ((log(R) / B) + (1.0 / 298.15));
  return T - 273.15;
}