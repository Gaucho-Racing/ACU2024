
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
enum test_case {ADBMS6830, CAN, FAN, GPIO, TEENSY, CELLBAL, EXTENDEDCELLBAL, EXTRA, PRECHARGE, ADC};
test_case debug = EXTRA;

CANLine can;
short message[8] = {60000,4,0,0,0,0,0,0};
std::vector<byte> pong;

float cellVoltage[16 * TOTAL_IC];
float baltemp[16*TOTAL_IC];
float celltemp[32*TOTAL_IC];
uint16_t cell_to_mux[8] ={0b0011111111, 0b0000111111, 0b0001111111, 0b0010111111, 0b0100111111, 0b0110111111, 0b0111111111, 0b0101111111};
// uint16_t mux_temp_codes[8] = {0b0011100001, 0b0000100001, 0b0001100001, 0b0010100001, 0b0100100001, 0b0110100001, 0b0111100001, 0b0101100001}; 



// std::unordered_map<u_int8_t, u_int16_t> cell_to_mux;
unsigned long previousMillis = 0;

cell_asic IC[TOTAL_IC];

fanController fans(&Serial8);

ADC1283 acu_adc(CS_ADC, 4.096, 1000000);

bool test_bool[10] = {0,0,0,0,0,0,0,0,0,0};

uint8_t Wrpwm1[2] = { 0x00, 0x20 };
uint8_t Wrpwm2[2] = { 0x00, 0x21 };
uint8_t Wrcfgb[2] = { 0x00, 0x24 };
uint8_t Wrcfga[2] = { 0x00, 0x01 };

float getAccumulatorVoltage() {
  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
  adBms6830_read_cell_voltages(TOTAL_IC, IC);
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++) {
    for (uint8_t cell = 0; cell < CELL; cell++) {
      cellVoltage[ic * CELL + cell] = (IC[ic].cell.c_codes[cell] + 10000) * 0.000150;
    }
  }
  float accumulatorVoltage = 0;
  for (uint8_t cell = 0; cell < CELL * TOTAL_IC; cell++) {
    accumulatorVoltage += cellVoltage[cell];
  }
  return accumulatorVoltage;
}

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
  pinMode(PIN_AIR_POS, OUTPUT);
  pinMode(PIN_AIR_NEG, OUTPUT);
  pinMode(PIN_AIR_RESET, OUTPUT); 
  pinMode(PIN_PRECHG, OUTPUT);
  
}

void loop() {
  switch (debug)
  {
  case ADBMS6830:
    Serial.println("Cell Voltages: --------------------------");
    adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
    //for some reason this doesn't work, why not?
    adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);

  Serial.println("TEMPS: --------------------------");

    previousMillis = millis();
    for(int i = 0; i < 8; i++){
      IC[0].tx_cfga.gpo = cell_to_mux[i];
      
      // previousMillis = millis();
      
      adBmsWriteData(TOTAL_IC, IC, Wrcfga, Config, AA);

      // Serial.printf("Time (1): %lu\n", millis() - previousMillis);
      // previousMillis = millis();
      
      adBms6830_start_aux_voltage_measurment(TOTAL_IC, IC);
      adBms6830_read_aux_voltages(TOTAL_IC, IC);

      baltemp[i] = getVoltage(IC[0].aux.a_codes[0]);
      baltemp[i+8] = getVoltage(IC[0].aux.a_codes[5]);
      celltemp[(7-i)] = getVoltage(IC[0].aux.a_codes[3]);
      celltemp[(7-i)+8] = getVoltage(IC[0].aux.a_codes[4]);
      celltemp[(7-i)+16] = getVoltage(IC[0].aux.a_codes[1]);
      celltemp[(7-i)+24] = getVoltage(IC[0].aux.a_codes[2]);
    }
    
    // bool issue;
    // int mux_codes[4] = {6,1,4,5};
    // //print out the temperatures
    // for(int i = 0; i <TOTAL_IC; i++){
    //   issue = false;
    //   for(int mux = 0; mux < 4; mux++){
    //       for(int j = 0; j < 8; j++){
    //         if(baltemp[mux*8 + j] > 4.0){
    //           issue = true;
    //         }
    //       }
    //       if(issue) Serial.printf("Mux %d has issue \n", mux_codes[i]);
    //   }
    // }
    

    Serial.println("Bal Temp: --------------------------");
    for(int i = 0; i < TOTAL_IC; i++){
        Serial.printf("Segment %d: ", i);
        for(int j = 0; j < 16; j++){
            Serial.printf("[%3u]%5.01f; ", j, V2T(baltemp[i*16 + j]));
        }
        Serial.println();
    }
        
    Serial.println("Cell Temp: --------------------------");
    for(int j = 0; j < 32; j++){
        Serial.printf("[%5u]%5.01f; ", j, V2T(celltemp[j]));
    }
    Serial.println();
    Serial.println("-------------END-------------");
  
  break;

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
    adBms6830_start_raux_voltage_measurment(TOTAL_IC, &IC[0]);
    adBms6830_read_raux_voltages(TOTAL_IC, &IC[0]);
    delay(200);
    Serial.println();
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

  case EXTRA:
    
  break;
  case ADC:
    Serial.printf("ADC GLV Voltage: %f\n", acu_adc.readVoltage(ADC_MUX_GLV_VOLT)*4);
    Serial.printf("ADC HV Voltage: %f\n", acu_adc.readVoltage(ADC_MUX_HV_VOLT*200));
    Serial.printf("ADC HV Current: %f\n", acu_adc.readVoltage(ADC_MUX_HV_CURRENT));
    Serial.printf("ADC Shutdown Power: %f\n", acu_adc.readVoltage(ADC_MUX_SHDN_POW)*4);
    Serial.printf("ADC DCDC Current: %f\n", acu_adc.readVoltage(ADC_MUX_DCDC_CURRENT));
    Serial.printf("ADC DCDC Temp1: %f\n", V2T(acu_adc.readVoltage(ADC_MUX_DCDC_TEMP1)));
    Serial.printf("ADC DCDC Temp2: %f\n", V2T(acu_adc.readVoltage(ADC_MUX_DCDC_TEMP2)));
    Serial.printf("ADC Fan Ref: %f\n", acu_adc.readVoltage(ADC_MUX_FAN_REF)*2);
    Serial.println();
    break;
  case PRECHARGE:
  Serial.println("Precharge, AIR pins reset");
    digitalWrite(PIN_AIR_NEG, LOW);
    digitalWrite(PIN_PRECHG, LOW);


  Serial.println("Precharge Start");
  if (acu_adc.readVoltage(ADC_MUX_GLV_VOLT)*4 - acu_adc.readVoltage(ADC_MUX_SHDN_POW)*4) { // if latch is not closed
    digitalWrite(PIN_AIR_RESET, HIGH); // close latch
    delay(50); // wait for the relay to switch
    digitalWrite(PIN_AIR_RESET, LOW);
    while (acu_adc.readVoltage(ADC_MUX_GLV_VOLT)*4 - acu_adc.readVoltage(ADC_MUX_SHDN_POW)*4) {
      Serial.println("Latch not closed, error");
    }
  }

    digitalWrite(PIN_AIR_NEG, HIGH); // close AIR-
    delay(50); // wait for the relay to switch
    
    digitalWrite(PIN_PRECHG, HIGH); // close precharge relay
    delay(10); // wait for the relay to switch
    
  // send message to VDM to indicate Precharge
  // check voltage, if difference > 5V after 2 seconds throw error
  uint32_t startTime = millis();
  while (acu_adc.readVoltage(ADC_MUX_HV_VOLT)*200 < getAccumulatorVoltage() * PRECHARGE_THRESHOLD) {
    if (millis() - startTime > 3000) { // timeout, throw error
      digitalWrite(PIN_AIR_POS, LOW); // open AIR+, shouldn't be closed but just in case
      digitalWrite(PIN_AIR_NEG, LOW); // open AIR-
      digitalWrite(PIN_PRECHG, LOW); // open precharge relay, close discharge relay
      Serial.println("Precharge timeout, error");
      return;
    }
    delay(20);
  }
    digitalWrite(PIN_AIR_POS, HIGH); // clost AIR+
    delay(50); // wait for the relay to switch

  Serial.println("Precharge Done. Ready to drive. ");
  break;


  default:
    Serial.println("Uh oh u dummy u didn't set what to debug");
    break;
  }

  delay(2000);
  
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