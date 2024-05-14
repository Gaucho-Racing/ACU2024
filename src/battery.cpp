#include "battery.h"

uint16_t mux_temp_codes[8] = {0b0011100001, 0b0000100001, 0b0001100001, 0b0010100001, 0b0100100001, 0b0110100001, 0b0111100001, 0b0101100001}; 

void init_config(Battery &battery){

    return;
}

void updateVoltage(Battery &battery){
  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
  adBms6830_read_cell_voltages(TOTAL_IC, battery.IC);
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++) {
    for (uint8_t cell = 0; cell < CELL; cell++) {
      battery.cellVoltage[ic * CELL + cell] = (battery.IC[ic].cell.c_codes[cell] + 10000) * 3 / 2;
    }
  }
}
bool checkVoltage(Battery &battery){

    return false;
}

/// @brief converts thermistor voltage to temperature; TODO: calibrate B values per thermistor
/// @param[in] voltage
/// @param[in] B
/// @return temperature in deg C
float V2T(float voltage, float B = 4390){
  float actualVoltage = (voltage+10000)* 0.000150;
  float R = actualVoltage / ((5.0 - actualVoltage) / 47e3) / 100e3;
  float T = 1.0 / ((log(R) / B) + (1.0 / 298.15));
  return T - 273.15;
}

void updateTemp(Battery &battery, uint8_t cycle){
  //write the mux to the gpio
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    battery.IC[ic].tx_cfga.gpo = mux_temp_codes[cycle];
  }

  adBms6830_start_aux_voltage_measurment(TOTAL_IC, battery.IC);
  adBms6830_read_aux_voltages(TOTAL_IC, battery.IC);
  
  //parse the data and store it in the battery struct
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    //all values are subtracted by one to account for indexing from 0
    //gpio 3: mux1, temp 0
    battery.cellTemp[ic*32 + (7-cycle)] = V2T(battery.IC[ic].aux.a_codes[3]);
    //gpio 4: mux 2, temp 8
    battery.cellTemp[ic*32 + (7-cycle) + 8] = V2T(battery.IC[ic].aux.a_codes[4]);
    //gpio 5: mux 3, bal 0
    battery.balTemp[ic*16 + cycle] = V2T(battery.IC[ic].aux.a_codes[5]);
    //gpio 0: mux 4, bal 0
    battery.balTemp[ic*16 + cycle + 8] = V2T(battery.IC[ic].aux.a_codes[0]);
    //gpio 1: mux 5, temp 16
    battery.cellTemp[ic*32 + (7-cycle) + 16] = V2T(battery.IC[ic].aux.a_codes[1]);
    //gpio 2: mux 6, temp 24
    battery.cellTemp[ic*32 + (7-cycle) + 24] = V2T(battery.IC[ic].aux.a_codes[2]);
  }
  //increment the cycle
}
bool checkTemp(Battery &battery){

    return false;
}
void updateAllTemps(Battery &battery){
    //8 input mux, 8 cycles
  for(uint8_t i = 0; i < 8; i++){
    for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    battery.IC[ic].tx_cfga.gpo = mux_temp_codes[i];
    }
    // adBmsWriteData(TOTAL_IC, battery.IC, WRCFGA, Config, AA);
    adBms6830_start_aux_voltage_measurment(TOTAL_IC, battery.IC);
    adBms6830_read_aux_voltages(TOTAL_IC, battery.IC);
    for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
      //all values are subtracted by one to account for indexing from 0
      //gpio 3: mux1, temp 0
      battery.cellTemp[ic*32 + (7-i)] = V2T(battery.IC[ic].aux.a_codes[3]);
      //gpio 4: mux 2, temp 8
      battery.cellTemp[ic*32 + (7-i) + 8] = V2T(battery.IC[ic].aux.a_codes[4]);
      //gpio 5: mux 3, bal 0
      battery.balTemp[ic*16 + i] = V2T(battery.IC[ic].aux.a_codes[5]);
      //gpio 0: mux 4, bal 0
      battery.balTemp[ic*16 + i + 8] = V2T(battery.IC[ic].aux.a_codes[0]);
      //gpio 1: mux 5, temp 16
      battery.cellTemp[ic*32 + (7-i) + 16] = V2T(battery.IC[ic].aux.a_codes[1]);
      //gpio 2: mux 6, temp 24
      battery.cellTemp[ic*32 + (7-i) + 24] = V2T(battery.IC[ic].aux.a_codes[2]);
    }
  }
    return;
}

uint8_t condenseVoltage(uint16_t voltage){

    return 0;
}
uint8_t condenseTemperature(float temperature){

    return 0;
}
uint8_t calcCharge(Battery &battery){

    return 0;
}

void cell_Balancing(Battery &battery){

    return;
}

/*

//this slowly updates the temperatures of the cells, one mux at a time
/// @brief updates temperatures
/// @param[in] battery
/// @return None
void updateTemps(Battery &battery){
  
}

/// @brief updates all temperatures, using same structure as updateTemps but using a for loop instead of cycles
/// @param[in] battery
/// @return None
void updateAllTemps(Battery &battery){
  
}


/// @brief converts uint16_t voltage --> uint8_t voltage
/// @param[in] voltage uint16_t
/// @return uint8_t voltage converted
uint8_t condenseVoltage(uint16_t voltage) {
  //voltage = constrain(voltage, 20000, 45500);
  return (voltage / 100 + (voltage % 100 > 49));// - 200; // uncomment these when connecting to cells
}

/// @brief converts float temp --> uint8_t temp
/// @param[in] temperature float
/// @return uint8_t temperature converted
uint8_t condenseTemperature(float temperature) {
  return (uint8_t)((temperature - 10) * 4); // 10C~73.75C --> 0~255
}
*/