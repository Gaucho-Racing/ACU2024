#include "battery.h"

uint16_t mux_temp_codes[8] = {0b0011100001, 0b0000100001, 0b0001100001, 0b0010100001, 0b0100100001, 0b0110100001, 0b0111100001, 0b0101100001}; 

void Battery::updateVoltage(){
  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
  adBms6830_read_cell_voltages(TOTAL_IC, this->IC);
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++) {
    for (uint8_t cell = 0; cell < CELL; cell++) {
      this->cellVoltage[ic * CELL + cell] = (this->IC[ic].cell.c_codes[cell] + 10000) * 3 / 2;
    }
  }
}
/// @brief check Voltage and update min/max values; update errs 
/// @param battery 
/// @param errs 
void Battery::checkVoltage(uint8_t &errs){
  //if first check, set extremes to first cell
  
  if(this->minVolt == -1) this->minVolt = this->cellVoltage[0];
  bool isOK = true;
  //iterate though cellVoltage
  for (int i = 0 ; i < TOTAL_IC*16; i++){

    if (this->minVolt > this->cellVoltage[i]) this->minVolt = this->cellVoltage[i];
    if (this->cellVoltage[i] > OV_THRESHOLD){
      errs |= ERR_OverVolt;
    }
    if (this->cellVoltage[i] < UV_THRESHOLD){
      errs |= ERR_UndrVolt;
    }
  }
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

/// @brief updates the temperature of the battery, one mux code at a time, based on the cycle
/// @param[in] battery
/// @param[in] cycle
/// @return None
void Battery::updateTemp(uint8_t cycle){
  if(cycle > 7){
    Serial.println("Invalid cycle");
    return;
  }
  //write the mux to the gpio
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    this->IC[ic].tx_cfga.gpo = mux_temp_codes[cycle];
  }

  adBms6830_start_aux_voltage_measurment(TOTAL_IC, this->IC);
  adBms6830_read_aux_voltages(TOTAL_IC, this->IC);
  
  //parse the data and store it in the battery struct
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    //all values are subtracted by one to account for indexing from 0
    //gpio 3: mux1, temp 0
    this->cellTemp[ic*32 + (7-cycle)] = V2T(this->IC[ic].aux.a_codes[3]);
    //gpio 4: mux 2, temp 8
    this->cellTemp[ic*32 + (7-cycle) + 8] = V2T(this->IC[ic].aux.a_codes[4]);
    //gpio 5: mux 3, bal 0
    this->balTemp[ic*16 + cycle] = V2T(this->IC[ic].aux.a_codes[5]);
    //gpio 0: mux 4, bal 0
    this->balTemp[ic*16 + cycle + 8] = V2T(this->IC[ic].aux.a_codes[0]);
    //gpio 1: mux 5, temp 16
    this->cellTemp[ic*32 + (7-cycle) + 16] = V2T(this->IC[ic].aux.a_codes[1]);
    //gpio 2: mux 6, temp 24
    this->cellTemp[ic*32 + (7-cycle) + 24] = V2T(this->IC[ic].aux.a_codes[2]);
  }
  //increment the cycle
}

/// @brief 
/// @param battery 
/// @param errs 
void Battery::checkTemp(uint8_t &errs){
  if(this->maxBalTemp == -1) this->maxBalTemp = this->balTemp[0];
  if(this->maxCellTemp == -1) this->maxCellTemp = this->cellTemp[0];
  for(int i = 0; i < TOTAL_IC*16; i++){
    if (this->maxBalTemp < this->balTemp[i]) this->maxBalTemp = this->balTemp[i];
    // if (battery.minCellVo > battery.cellTemp[i]) battery.maxBalTemp = battery.balTemp[i];
    //check Bal Temp;
    if (this->balTemp[i] > MAX_BAL_TEMP){
      errs |= ERR_OverTemp;
    }
    if (this->balTemp[i] < MIN_BAL_TEMP){
      errs |= ERR_UndrTemp;
    }
  }
  for(int i = 0; i < TOTAL_IC*2*16; i++){
    if (this->maxCellTemp < this->cellTemp[i]) this->maxCellTemp = this->cellTemp[i];
    // if (battery.minCellVo > battery.cellTemp[i]) battery.maxBalTemp = battery.balTemp[i];
    //check Bal Temp;
    if (this->cellTemp[i] > MAX_BAL_TEMP){
      errs |= ERR_OverTemp;
    }
    if (this->cellTemp[i] < MIN_BAL_TEMP){
      errs |= ERR_UndrTemp;
    }
  }
}

/// @brief updates all temperatures, using same structure as updateTemps but using a for loop instead of cycles
/// @param[in] battery
/// @return None
void Battery::updateAllTemps(){
    //8 input mux, 8 cycles
  for(uint8_t i = 0; i < 8; i++){
    for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    this->IC[ic].tx_cfga.gpo = mux_temp_codes[i];
    }
    // adBmsWriteData(TOTAL_IC, battery.IC, WRCFGA, Config, AA);
    adBms6830_start_aux_voltage_measurment(TOTAL_IC, this->IC);
    adBms6830_read_aux_voltages(TOTAL_IC, this->IC);
    for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
      //all values are subtracted by one to account for indexing from 0
      //gpio 3: mux1, temp 0
      this->cellTemp[ic*32 + (7-i)] = V2T(this->IC[ic].aux.a_codes[3]);
      //gpio 4: mux 2, temp 8
      this->cellTemp[ic*32 + (7-i) + 8] = V2T(this->IC[ic].aux.a_codes[4]);
      //gpio 5: mux 3, bal 0
      this->balTemp[ic*16 + i] = V2T(this->IC[ic].aux.a_codes[5]);
      //gpio 0: mux 4, bal 0
      this->balTemp[ic*16 + i + 8] = V2T(this->IC[ic].aux.a_codes[0]);
      //gpio 1: mux 5, temp 16
      this->cellTemp[ic*32 + (7-i) + 16] = V2T(this->IC[ic].aux.a_codes[1]);
      //gpio 2: mux 6, temp 24
      this->cellTemp[ic*32 + (7-i) + 24] = V2T(this->IC[ic].aux.a_codes[2]);
    }
  }
    return;
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

uint8_t Battery::calcCharge(){

    return 0;
}

void Battery::cell_Balancing(){
  uint16_t toDischarge = 0;

  //check new voltage to find min cell temp
  this->updateVoltage();
  if(!(this->checkVoltage())){

    return;
  } 

  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    toDischarge = 0;
    
  }
  //figure out which cells to discharge
  for(int ic = 0; ic < TOTAL_IC; ic++){
    for(int cell = 0; cell < CELL; cell++){
      //diff between the minimum cell voltage and the current cell is 20mV discharge
      if(this->cellVoltage[ic*CELL + cell]-this->minVolt > 200){
        toDischarge |= 1 << cell;
      }
    }
    this->IC[ic].tx_cfgb.dcc = toDischarge;
  }
  
  
}