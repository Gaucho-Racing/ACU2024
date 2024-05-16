
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"
#include "can.cpp"
#include "ADC1283.h"
#include <unordered_map>

// put function declarations here:
void wakeBms();
void printPWM(uint8_t tIC, cell_asic *IC);
float V2T(float voltage, float B = 4390);

// Object declarations 
//isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
//isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);
enum test_case {VOLTAGE, CAN, FAN, GPIO, TEENSY, CELLBAL, THERMAL, EXTENDEDCELLBAL, EXTRA, ADC};
test_case debug = VOLTAGE;

CANLine can;
short message[8] = {60000,4,0,0,0,0,0,0};
std::vector<byte> pong;

float temp[8][2];
uint16_t cell_to_mux[8] ={0b0011111111, 0b0000111111, 0b0001111111, 0b0010111111, 0b0100111111, 0b0110111111, 0b0111111111, 0b0101111111};
// std::unordered_map<u_int8_t, u_int16_t> cell_to_mux;
unsigned long previousMillis = 0;

cell_asic IC[TOTAL_IC];

fanController fans(&Serial8);

ADC1283 acu_adc;

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
  acu_adc.begin();
  //TODO: 
  acu_adc.setCsPin(10);
  acu_adc.setFsck(13);
  acu_adc.setVref(5.0);
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
      IC[0].tx_cfga.gpo = cell_to_mux[i];
      
      // previousMillis = millis();
      
      adBmsWriteData(TOTAL_IC, IC, Wrcfga, Config, AA);

      // Serial.printf("Time (1): %lu\n", millis() - previousMillis);
      // previousMillis = millis();
      
      adBms6830_start_aux_voltage_measurment(TOTAL_IC, IC);
      adBms6830_read_aux_voltages(TOTAL_IC, IC);
      temp[i][0] = getVoltage(IC[0].aux.a_codes[0]);
      temp[i][1] = getVoltage(IC[0].aux.a_codes[5]);
    }
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
  case ADC:
    Serial.printf("ADC GLV Voltage: %f\n", acu_adc.readVoltage(ADC_MUX_GLV_VOLT));
    Serial.printf("ADC HV Voltage: %f\n", acu_adc.readVoltage(ADC_MUX_HV_VOLT));
    Serial.printf("ADC HV Current: %f\n", acu_adc.readVoltage(ADC_MUX_HV_CURRENT));
    Serial.printf("ADC Shutdown Power: %f\n", acu_adc.readVoltage(ADC_MUX_SHDN_POW))*4;
    Serial.printf("ADC DCDC Current: %f\n", acu_adc.readVoltage(ADC_MUX_DCDC_CURRENT));
    Serial.printf("ADC DCDC Temp1: %f\n", V2T(acu_adc.readVoltage(ADC_MUX_DCDC_TEMP1)));
    Serial.printf("ADC DCDC Temp2: %f\n", V2T(acu_adc.readVoltage(ADC_MUX_DCDC_TEMP2)));
    Serial.printf("ADC Fan Ref: %f\n", acu_adc.readVoltage(ADC_MUX_FAN_REF)*2);
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