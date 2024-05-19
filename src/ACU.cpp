#include "ACU.h"


/// @brief converts thermistor voltage to temperature; TRIAGE 4: calibrate B values per thermistor
/// @param[in] voltage
/// @param[in] B
/// @return temperature in deg C
float V2T(float voltage, float B = 4390){
  float actualVoltage = (voltage+10000)* 0.000150;
  float R = actualVoltage / ((5.0 - actualVoltage) / 47e3) / 100e3;
  float T = 1.0 / ((log(R) / B) + (1.0 / 298.15));
  return T - 273.15;
}

void ACU::ACU_init(){
  this->ACU_ADC.begin();
}

void ACU::updateGlvVoltage(){
  glv_voltage = ACU_ADC.readVoltage(ADC_MUX_GLV_VOLT) * 4;
}
void ACU::updateTsVoltage(){
  ts_voltage = ACU_ADC.readVoltage(ADC_MUX_HV_VOLT);
}
void ACU::updateTsCurrent(){
  ts_current = ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT);
}
void ACU::updateShdnVolt(){
  shdn_volt = ACU_ADC.readVoltage(ADC_MUX_SHDN_POW) * 4;
}
void ACU::updateDcdcCurrent(){
  dcdc_current = ACU_ADC.readVoltage(ADC_MUX_DCDC_CURRENT);
}
void ACU::updateDcdcTemp1(){
  DCDC_temp[0] = V2T(ACU_ADC.readVoltage(ADC_MUX_DCDC_TEMP1)); 
}
void ACU::updateDcdcTemp2(){
  DCDC_temp[1] = V2T(ACU_ADC.readVoltage(ADC_MUX_DCDC_TEMP2)); //given in volts
}
void ACU::updateFanRef(){
  fan_Ref = ACU_ADC.readVoltage(ADC_MUX_FAN_REF)*2;
}

void ACU::updateAll(){
  updateGlvVoltage();
  updateTsVoltage();
  updateTsCurrent();
  updateShdnVolt();
  updateDcdcCurrent();
  updateDcdcTemp1();
  updateDcdcTemp2();
  updateFanRef();
}

//ACU can cause the following errors:
//Precharge, Undervolt Error (Glv), 
//TRIAGE 1: FINISH this
void ACU::checkACU(){
  this->errs &= ~(ERR_Prechrg|ERR_UndrVolt); //reset errors
}


uint8_t ACU::getRelayState(){
  return relay_state;
}
float ACU::getGlvVoltage(){
  return glv_voltage;
}
float ACU::getTsVoltage(){
  return ts_voltage;
}
float ACU::getTsCurrent(){
  return ts_current;
}
float ACU::getShdnVolt(){
  return shdn_volt;
}
float ACU::getDcdcCurrent(){
  return dcdc_current;
}
float ACU::getDcdcTemp1(){
  return DCDC_temp[0];
}
float ACU::getDcdcTemp2(){
  return DCDC_temp[1];
}
float ACU::getFanRef(){
  return fan_Ref;
}
