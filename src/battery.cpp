#include "battery.h"
#include "can.h"
uint16_t mux_temp_codes[8] = {0b0011111111, 0b0000111111, 0b0001111111, 0b0010111111, 0b0100111111, 0b0110111111, 0b0111111111, 0b0101111111};
; 

void Battery::init_config(){
  adBmsSpiInit();
  adBms6830_init_config(TOTAL_IC, this->IC);
  for(int i = 0; i < 4; i++){
    adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
    adBms6830_read_cell_voltages(TOTAL_IC, this->IC); 
    if((this->IC[0].cell.c_codes[0] + 10000) * 0.000150 > 1.75) break;
    delay(100);
  }
  this->cell_OT_Threshold = MAX_CELL_TEMP;
  this->cell_UT_Threshold = MIN_CELL_TEMP;
  Serial.printf("cell_OT_Threshold: %5.03f, cell_UT_Threshold: %5.03f\n", this->cell_OT_Threshold, this->cell_UT_Threshold);
  this->updateVoltage();
  this->updateAllTemps();
  // this->resetDischarge(); would be nice to have
}

/// @brief updates the voltage of the battery
void Battery::updateVoltage(){
  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC, &readCANData);
  adBms6830_read_cell_voltages(TOTAL_IC, this->IC);
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++) {
    if (this->IC[ic].cccrc.cell_pec) {
      //D_pf("Segment %u PEC error\n", ic);
      for (uint8_t cell = 0; cell < CELL; cell++) {
        this->cellVoltErr[ic * CELL + cell]++;
      }
    }
    else {
      for (uint8_t cell = 0; cell < CELL; cell++) {
        this->cellVoltage[ic * CELL + cell] = (this->IC[ic].cell.c_codes[cell] + 10000) * 0.000150;
      }
    }
  }
}


///Triage 2: optimize this
/// @brief updates the temperature of the battery, one mux code at a time, based on the cycle
/// @param[in] battery
/// @param[in] cycle
/// @return None
void Battery::updateTemp(){
  if(cycle > 7){
    //Serial.println("Invalid cycle");
    return;
  }
  //write the mux to the gpio
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    this->IC[ic].tx_cfga.gpo = mux_temp_codes[cycle];
  }

  adBms6830_start_aux_voltage_measurment(TOTAL_IC, this->IC, &readCANData);
  adBms6830_read_aux_voltages(TOTAL_IC, this->IC);
  
  //parse the data and store it in the battery struct
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++) {
    if (this->IC[ic].cccrc.aux_pec) {
      //gpio 3: mux1, temp 0
      this->cellTempErr[ic*16 + (7-cycle)/2]++;
      //gpio 4: mux 2, temp 8
      this->cellTempErr[ic*16 + (7-cycle)/2 + 4]++;
      //gpio 5: mux 3, bal 0
      this->cellTempErr[ic*16 + cycle]++;
      //gpio 0: mux 4, bal 0
      this->cellTempErr[ic*16 + cycle + 8]++;
      //gpio 1: mux 5, temp 16
      this->cellTempErr[ic*16 + (7-cycle)/2 + 8]++;
      //gpio 2: mux 6, temp 24
      this->cellTempErr[ic*16 + (7-cycle)/2 + 12]++;
    }
    else {
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
      if (this->IC[ic].cccrc.aux_pec) {
        //gpio 3: mux1, temp 0
        this->cellTempErr[ic*16 + (7-i)/2]++;
        //gpio 4: mux 2, temp 8
        this->cellTempErr[ic*16 + (7-i)/2 + 4]++;
        //gpio 5: mux 3, bal 0
        this->cellTempErr[ic*16 + i]++;
        //gpio 0: mux 4, bal 0
        this->cellTempErr[ic*16 + i + 8]++;
        //gpio 1: mux 5, temp 16
        this->cellTempErr[ic*16 + (7-i)/2 + 8]++;
        //gpio 2: mux 6, temp 24
        this->cellTempErr[ic*16 + (7-i)/2 + 12]++;
      }
      else {
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
  }
    return;
}

//TRIAGE 2: Add warnings
/// @brief check the voltage of the battery, update min/max values, and update errs
void Battery::checkVoltage(){
  //if first check, set extremes to first cell
  
  this->minCellVolt = this->cellVoltage[0];
  this->maxCellVolt = this->cellVoltage[0];
  // bool isOK = true;
  //iterate though cellVoltage
  this->batVoltage = 0;
  for (int i = 0 ; i < TOTAL_IC*16; i++){
    if (this->maxCellVolt < this->cellVoltage[i]) this->maxCellVolt = this->cellVoltage[i];
    if (this->minCellVolt > this->cellVoltage[i]) this->minCellVolt = this->cellVoltage[i];
    if (this->cellVoltage[i] > OV_THRESHOLD){
      this->cellVoltErr[i]++;
      D_pf("Cell %u OverVolt Err: %5.03fV, x%u\n", i, this->cellVoltage[i], this->cellVoltErr[i]);
      if (this->cellVoltErr[i] >= ERRMG_CELL_VOLT_ERR) {
        acu.errs |= ERR_OverVolt;
        this->cellVoltErr[i] = ERRMG_CELL_VOLT_ERR;
      }
    }
    else {
      this->cellVoltErr[i] = 0;
    }
    if (this->cellVoltage[i] < UV_THRESHOLD){
      this->cellVoltErr[i]++;
      if (this->cellVoltage[i] > 1.6) {
        D_pf("Cell %u UndrVolt Err: %5.03fV, x%u\n", i, this->cellVoltage[i], this->cellVoltErr[i]);
      }
      if (this->cellVoltErr[i] >= ERRMG_CELL_VOLT_ERR) {
        acu.errs |= ERR_UndrVolt;
        this->cellVoltErr[i] = ERRMG_CELL_VOLT_ERR;
      }
    }
    else {
      this->cellVoltErr[i] = 0;
    }
    this->batVoltage += this->cellVoltage[i];
  }
}

//TRIAGE 2: Add warnings
/// @brief compares bal and cell temps to max/min values, updates errs
/// NAN values are discarded, they are likely due to a disconnection
void Battery::checkTemp(){
  this->maxBalTemp = this->balTemp[0];
  this->maxCellTemp = this->cellTemp[0];
  for(int i = 0; i < TOTAL_IC*16; i++){
    if (this->maxBalTemp < this->balTemp[i]) this->maxBalTemp = this->balTemp[i];
    // if (battery.minCellVo > battery.cellTemp[i]) battery.maxBalTemp = battery.balTemp[i];
    //check Bal Temp;
    // if(this->balTemp[i] == NAN || this->cellTemp[i] == NAN){
    //   D_L1("Nan temp");
    // }
    // else{
    //   if (this->balTemp[i] > MAX_BAL_TEMP){
    //     D_L1("Bal OverTemp Err");
    //     this->cellTempErr[i]++;
    //     if (this->cellTempErr[i] > ERRMG_CELL_TEMP_ERR) acu.errs |= ERR_OverTemp;
    //   }
    //   else if (this->cellTempErr[i] < MIN_BAL_TEMP){
    //     D_L1("Bal UnderTemp Err");
    //     this->cellTempErr[i]++;
    //     if (this->cellTempErr[i] > ERRMG_CELL_TEMP_ERR) acu.errs |= ERR_UndrTemp;
    //   }
    //   else {
    //     this->cellTempErr[i] = 0;
    //   }
    // }
  }
  for(uint16_t i = 0; i < TOTAL_IC*2*16; i++){
    if(this->balTemp[i] == NAN || this->cellTemp[i] == NAN){
      D_L1("Nan temp pt. 2");
    } else {
      if (this->maxCellTemp < this->cellTemp[i]) this->maxCellTemp = this->cellTemp[i];
      // if (battery.minCellVo > battery.cellTemp[i]) battery.maxBalTemp = battery.balTemp[i];
      //check Cell Temp;
      if (this->cellTemp[i] > MAX_CELL_TEMP){
        this->cellTempErr[i >> 1]++;
        //Serial.printf("Cell %d overtemp: %f°C, celErr: %u\n", i >> 1, this->cellTemp[i], this->cellTempErr[i >> 1]);
        if (this->cellTempErr[i >> 1] >= ERRMG_CELL_TEMP_ERR) {
          acu.errs |= ERR_OverTemp;
          this->cellTempErr[i >> 1] = ERRMG_CELL_TEMP_ERR;
        }
      }
      else if (this->cellTemp[i] < MIN_CELL_TEMP){
        this->cellTempErr[i >> 1]++;
        //Serial.printf("Cell %d undrtemp: %f°C, celErr: %u\n", i >> 1, this->cellTemp[i], this->cellTempErr[i >> 1]);
        if (this->cellTempErr[i >> 1] >= ERRMG_CELL_TEMP_ERR) {
          acu.errs |= ERR_UndrTemp;
          this->cellTempErr[i >> 1] = ERRMG_CELL_TEMP_ERR;
        }
      }
      else {
        this->cellTempErr[i >> 1] = 0;
      }
    }
  }
}

/// @brief checks fuses by turning on discharge for a cell based on cycle 
/// then examining if the voltage changes drastically
void Battery::checkFuse(){
  //D_L1("------------Checking fuses------------- \n");
  for(int ic = 0; ic < TOTAL_IC; ic++){
    this->IC[ic].tx_cfgb.dcc = 0;
    this->IC[ic].tx_cfgb.dcc |= 1 << cycle;
    adBmsWakeupIc(TOTAL_IC);
    adBmsWriteData(TOTAL_IC, this->IC, WRCFGB, Config, BB);
  }
  this->updateVoltage();
  this->checkVoltage();
  //D_L1("------------Fuse Checked------------- \n");
  resetDischarge();
}

/// @brief does the same as checkFuse but immediately
void Battery::checkAllFuse(){
  for(int i = 0; i < 8; i++){
    for(int ic = 0; ic < TOTAL_IC; ic++){
      this->IC[ic].tx_cfgb.dcc = 0;
      this->IC[ic].tx_cfgb.dcc |= 1 << i;
    }
    adBmsWakeupIc(TOTAL_IC);
    adBmsWriteData(TOTAL_IC, this->IC, WRCFGB, Config, BB);
    this->updateVoltage();
    this->checkVoltage();
  }
  resetDischarge();
}

/// @brief finds lowest cell voltage and discharges the other cells to match, within 20mV
void Battery::cell_Balancing(){
  D_L2("Balancing cells");
  //turn off balancing to prepare for cell voltage readings
  for(int ic = 0; ic < TOTAL_IC; ic++){
    this->IC[ic].tx_cfgb.dcc = 0;
  }
  uint16_t toDischarge = 0;

  // //check new voltage to find min cell temp  
  float threshold = (this->minCellVolt + this->maxCellVolt)/2;
  D_L1(threshold);

  //figure out which cells to discharge
  Serial.printf("\nDischarging Stuff: ************************** \n");
  for(int ic = 0; ic < TOTAL_IC; ic++){
    Serial.printf("Segment %d: ", ic);
    for(int cell = 0; cell < CELL; cell++){
      if((this->cellVoltage[ic*CELL + cell] > threshold || this->cellVoltage[ic*CELL + cell] - minCellVolt > 0.02) && this->balTemp[ic*CELL + cell] < 70){
        toDischarge |= 1 << cell;
        Serial.print("1 ");
      }
      else{
        Serial.print("0 ");
      }
    }
    Serial.println();
    
    this->IC[ic].tx_cfgb.dcc = toDischarge;
    toDischarge = 0;
  }
  Serial.printf("Discharge Print Done:  ************************** \n");
  adBmsWakeupIc(TOTAL_IC);
  adBmsWriteData(TOTAL_IC, this->IC, WRCFGA, Config, AA);
  adBmsWriteData(TOTAL_IC, this->IC, WRCFGB, Config, BB);
  
}

/// @brief disables the mux, used in Standby
void Battery::disable_Mux(){
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    this->IC[ic].tx_cfga.gpo = 0b1000000000;
    
  }
  adBmsWakeupIc(TOTAL_IC);
  adBmsWriteData(TOTAL_IC, this->IC, WRCFGA, Config, AA);
}

float Battery::getCellVoltage(uint8_t index){
  
  return this->cellVoltage[index];
}
float Battery::getCellTemp(uint8_t index){
  return this->cellTemp[index];

}
float Battery::getBalTemp(uint8_t index){
  return this->balTemp[index];
}

/// @brief sums all cell voltages, each cell voltage might be off by +- 10mV
float Battery::getTotalVoltage(){
  float totVoltage = 0;
  for(int i = 0; i < TOTAL_IC*16; i++){
    totVoltage += this->cellVoltage[i];
  }
  return totVoltage;
}

void Battery::checkBattery(bool fullCheck){
  if (fullCheck) {
    this->updateAllTemps();
    this->checkAllFuse();
  } else {
    this->updateTemp();
    this->checkFuse();
  }
  this->checkTemp();
  this->updateVoltage();
  this->checkVoltage();
}

/// @brief converts float voltage --> uint8_t voltage
/// @param[in] voltage float
/// @return uint8_t voltage converted
uint8_t condenseVoltage(float voltage) {
  return (voltage - 2) * 100;
}

/// @brief converts float temp --> uint8_t temp
/// @param[in] temperature float
/// @return uint8_t temperature converted
uint8_t condenseTemperature(float temp1, float temp2) {
  temp1 = (abs(temp1 - 61.411102) < 0.00001) ? 10 : temp1;
  temp2 = (abs(temp2 - 61.411102) < 0.00001) ? 10 : temp2;
  return (uint8_t)((max(temp1, temp2) - 10) * 4); // 10C~73.75C --> 0~255
}


/// @brief calculates state of charge and applies filter
float Battery::updateSOC() {
  float cellOpenVoltage = getTotalVoltage() + acu.getTsCurrent(false) * CELL_INT_RESISTANCE * TOTAL_IC * 16;
  float zeroChargeVolt = TOTAL_IC * 16 * CELL_EMPTY_VOLTAGE;
  float fullChargeVolt = TOTAL_IC * 16 * CELL_FULL_VOLTAGE;
  batSOC += (map(cellOpenVoltage, zeroChargeVolt, fullChargeVolt, 0, 255) - batSOC) * 0.1;
  return batSOC;
}

void Battery::resetDischarge(){
  for(int ic = 0; ic < TOTAL_IC; ic++){
    this->IC[ic].tx_cfgb.dcc = 0;
  }
  adBmsWakeupIc(TOTAL_IC);
  adBmsWriteData(TOTAL_IC, this->IC, WRCFGB, Config, BB);
}