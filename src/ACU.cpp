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

void ACU::init_config(){
  pinMode(PIN_IMD_OK, INPUT_PULLUP);  
  pinMode(PIN_AMS_OK, OUTPUT);
  pinMode(PIN_DCDC_EN, OUTPUT);
  pinMode(PIN_DCDC_SLOW, OUTPUT);
  pinMode(PIN_DCDC_ER, INPUT);
  pinMode(PIN_BSPD_CLK, OUTPUT);
  analogWriteFrequency(PIN_BSPD_CLK, 50000);
  analogWrite(PIN_BSPD_CLK, 127);
  this->ACU_ADC.begin();
}

//triage 3: owen check this
void ACU::updateGlvVoltage(){
  glv_voltage = ACU_ADC.readVoltage(ADC_MUX_GLV_VOLT) * 4;
}
void ACU::updateTsVoltage(){
  ts_voltage = ACU_ADC.readVoltage(ADC_MUX_HV_VOLT) * 200;
}
void ACU::updateTsCurrent(){
  ts_current = (ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) - 1.235) /5 /0.0032;
}
void ACU::updateShdnVolt(){
  shdn_volt = ACU_ADC.readVoltage(ADC_MUX_SHDN_POW) * 4;
}
void ACU::updateDcdcCurrent(){
  dcdc_current = (ACU_ADC.readVoltage(ADC_MUX_DCDC_CURRENT) - 2.5) / 0.09;
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
void ACU::updateRelayState(){
  relay_state = (digitalRead(PIN_AIR_NEG) << 7) + (digitalRead(PIN_AIR_POS) << 6) + (digitalRead(PIN_PRECHG) << 5);
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


//TRAIGE 3: check if the values are correct
void ACU::checkACU(){
    this->warns &= ~(WARN_LowChrg|WARN_HighCurr); //reset warnings
    //overcurrent checks
    if(this->ts_current > MAX_HV_CURRENT){
        #if DEBUG > 1
            Serial.println("HV Overcurrent detected");
        #endif
        this->errs |= ERR_OverCurr;
    }
    else if(this->ts_current > MAX_HV_CURRENT*0.8){
        this->warns |= WARN_HighCurr;
    } 

    //dcdc convertor temp regulation, slow down fan if temp is high, shut down if temp is too high
    if(max(DCDC_temp[0], DCDC_temp[1]) > MAX_DCDC_TEMP){
        #if DEBUG > 1
            Serial.println("DCDC Overtemp detected");
        #endif
        digitalWrite(PIN_DCDC_EN, LOW);
    }
    else if(max(DCDC_temp[0], DCDC_temp[1]) > MAX_DCDC_TEMP*0.9){
        digitalWrite(PIN_DCDC_SLOW, HIGH);
    } else {
        digitalWrite(PIN_DCDC_EN, HIGH);
        digitalWrite(PIN_DCDC_SLOW, LOW);
    }

    //dcdc current
    if(this->dcdc_current > MAX_DCDC_CURRENT){
        #if DEBUG > 1
            Serial.println("DCDC Overcurrent detected");
        #endif
        this->errs |= ERR_OverCurr;
    }

    //glv bat not charged
    if(this->glv_voltage < MIN_GLV_VOLT){
        #if DEBUG > 1
            Serial.println("GLV Undervolt detected");
        #endif
        this->errs |= ERR_UndrVolt;
    } 

    //fan ref voltage
    if(this->fan_Ref < MIN_FAN_REF_VOLT){
        #if DEBUG > 1
            Serial.println("Fan Ref Low detected");
        #endif
        this->errs |= ERR_UndrVolt;
    } else if(this->fan_Ref > MAX_FAN_REF_VOLT){
        #if DEBUG > 1
            Serial.println("Fan Ref High detected");
        #endif
        this->errs |= ERR_OverVolt;
    }

    //shdn voltage, should be within 0.2V of GLV
    if(abs(this->shdn_volt - this->glv_voltage) > 0.2){
        #if DEBUG > 1
            Serial.println("Shdn voltage not within 0.2V of GLV");
        #endif
        if(this->shdn_volt < this->glv_voltage)
            this->errs |= ERR_UndrVolt;
        else if(this->shdn_volt > this->glv_voltage){
            this->errs |= ERR_OverVolt;
        }
    }
}


void ACU::setMaxChrgVoltage(float voltage){
  this->max_chrg_voltage = voltage;
}
void ACU::setMaxChrgCurrent(float current){
  this->max_chrg_current = current;
}
void ACU::setMaxOutputCurrent(float current){
  this->max_output_current = current;
}
void ACU::setMaxTemp(float temp){
  this->max_temp = temp;
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

void ACU::printIso(){
  Serial.println("-------IMD--------");
  uint8_t R_iso_status = IMD.imd_gen[2];
  uint16_t hv_voltage = IMD.hv_system_voltage;
  Serial.printf("R_ISO_Corrected: R_iso_corrected %u\n R_ISO_Status:");
  Serial.print(R_iso_status, HEX);
  Serial.printf("\n ISO_Meas_Count: %u\n Status_Warnings_Alarms:", IMD.iso_meas_count);
  Serial.print(IMD.status_warnings_alarms, BIN);
  Serial.printf("\n Status_Device_Activity: %u\n HV_System: %f\n", IMD.status_device_activity, hv_voltage);
}

void ACU::setIMDHV(float voltage){
  IMD.hv_system_voltage = voltage;
}
void ACU::setRIsoCorrected(uint16_t resistance){
  IMD.R_iso_corrected = resistance;
}
void ACU::setRIsoStatus(uint8_t mesurement){
  IMD.R_iso_status = mesurement;
}
void ACU::setIsoMeasCount(uint8_t count){
  IMD.iso_meas_count = count;
}
void ACU::setStatusWarningsAlarms(uint16_t status){
  IMD.status_warnings_alarms = status;
}
void ACU::setStatusDeviceActivity(uint8_t activity){
  IMD.status_device_activity = activity;
}
