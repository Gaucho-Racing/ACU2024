#include "ACU.h"

extern States state;

/// @brief converts thermistor voltage to temperature; TRIAGE 4: calibrate B values per thermistor
/// @param[in] voltage
/// @param[in] B
/// @return temperature in deg C
float V2T(float voltage, float B){
  float R = voltage / ((5.0 - voltage) / 47e3) / 100e3;
  float T = 1.0 / ((log(R) / B) + (1.0 / 298.15)); 
  return T - 273.15;
}

float V2T(int16_t voltage, float B){ // voltage in ADI format
  float actualVoltage = (voltage + 10000) * 0.000150;
  float R = actualVoltage / ((5.0 - actualVoltage) / 47e3) / 100e3;
  float T = 1.0 / ((log(R) / B) + (1.0 / 298.15)); 
  return T - 273.15;
}

void ACU::init_config(){
  //TRIAGE 1.5: add Fan controller init
  pinMode(PIN_IMD_OK, INPUT_PULLUP);  
  pinMode(PIN_AMS_OK, OUTPUT);
  pinMode(PIN_DCDC_EN, OUTPUT);
  pinMode(PIN_DCDC_SLOW, OUTPUT);
  pinMode(PIN_DCDC_ER, INPUT);
  pinMode(PIN_BSPD_CLK, OUTPUT);
  pinMode(PIN_AIR_POS, OUTPUT);
  pinMode(PIN_AIR_NEG, OUTPUT);
  pinMode(PIN_PRECHG, OUTPUT);
  pinMode(PIN_AIR_RESET, OUTPUT);
  analogWriteFrequency(PIN_BSPD_CLK, 50000);
  analogWrite(PIN_BSPD_CLK, 127);
  fans.begin();
  this->ACU_ADC.begin();
  cur_ref = ACU_ADC.readVoltageTot(ADC_MUX_HV_CURRENT,256);   //Zero current sensor offset
  while (abs(cur_ref - 1.235) > ERRMG_ISNS_VREF) {
    Serial.printf("Current sensor ref: %f ", cur_ref);
    D_L1("HV current too far from zero. Check hardware. ");
    delay(500);
    cur_ref = ACU_ADC.readVoltageTot(ADC_MUX_HV_CURRENT,256);
  }
}


void ACU::updateGlvVoltage(){
  glv_voltage = ACU_ADC.readVoltage(ADC_MUX_GLV_VOLT) * 4;
}
void ACU::updateTsVoltage(){
  ts_voltage = ACU_ADC.readVoltage(ADC_MUX_HV_VOLT) * 200;
}
void ACU::updateTsCurrent(){
  ts_current = (ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) - cur_ref) /5 /0.0032;
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
  relay_state = 0;
  relay_state |= (digitalRead(PIN_AIR_NEG) << 2) + (digitalRead(PIN_AIR_POS) << 1) + (digitalRead(PIN_PRECHG));
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
  updateRelayState();
}


//TRAIGE 3: check if the values are correct
void ACU::checkACU(bool startup){
  this->updateAll();
    this->warns &= ~(WARN_LowChrg|WARN_HighCurr); //reset warnings
    //overcurrent checks
    if(this->ts_current > MAX_HV_CURRENT){
        D_L1("Overcurrent detected");
        this->errs |= ERR_OverCurr;
    }
    else if(this->ts_current > MAX_HV_CURRENT*0.8){
        this->warns |= WARN_HighCurr;
    }

    //dcdc convertor temp regulation, slow down fan if temp is high, shut down if temp is too high
    if(max(DCDC_temp[0], DCDC_temp[1]) > MAX_DCDC_TEMP){
        D_L1("DCDC Overtemp detected");
        digitalWrite(PIN_DCDC_EN, LOW);
    }
    else if(max(DCDC_temp[0], DCDC_temp[1]) > MAX_DCDC_TEMP*0.9){
        digitalWrite(PIN_DCDC_SLOW, HIGH);
    } else {
      if(this->getTsVoltage() < 370)
        // digitalWrite(PIN_DCDC_EN, HIGH);
        //D_L1("HV Voltage too low, shutting down DCDC");
        digitalWrite(PIN_DCDC_SLOW, LOW);
    }

    //dcdc current
    if(this->dcdc_current > MAX_DCDC_CURRENT){
        D_L1("DCDC Overcurrent detected");
        this->errs |= ERR_OverCurr;
    }

    //glv bat not charged
    if(this->glv_voltage < MIN_GLV_VOLT){
        D_L1("GLV Undervolt detected");
        this->errs |= ERR_UndrVolt;
    } 
    
    if(this->glv_voltage > OPEN_GLV_VOLT){
        D_L1("GLV Not connected detected");
        this->errs |= ERR_UndrVolt;
    } 

    //fan ref voltage
    if(this->fan_Ref < MIN_FAN_REF_VOLT){
        D_L1("5V Low detected");
        this->errs |= ERR_UndrVolt;
    } else if(this->fan_Ref > MAX_FAN_REF_VOLT){
        D_L1("Fan Ref High detected");
        this->errs |= ERR_OverVolt;
    }

    //shdn voltage, should be close to GLV
    if(abs(this->shdn_volt - this->glv_voltage) > ERRMG_GLV_SDC && !startup && state == NORMAL){
      Serial.println(abs(this->shdn_volt - this->glv_voltage));
        D_L1("Shdn voltage not close enough of GLV");
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
  updateRelayState();
  return relay_state;
}

float ACU::getTsVoltage(bool update){
  if (update) updateTsVoltage();
  return ts_voltage;
}
float ACU::getTsCurrent(bool update){
  if (update) updateTsCurrent();
  return ts_current;
}
float ACU::getShdnVolt(bool update){
  if (update) updateShdnVolt();
  return shdn_volt;
}

float ACU::getGlvVoltage(bool update){
  if (update) updateGlvVoltage();
  return glv_voltage;
}

float ACU::getDcdcCurrent(bool update){
  if (update) updateDcdcCurrent();
  return dcdc_current;
}
float ACU::getDcdcTemp1(bool update){
  if (update) updateDcdcTemp1();
  return DCDC_temp[0];
}
float ACU::getDcdcTemp2(bool update){
  if (update) updateDcdcTemp2();
  return DCDC_temp[1];
}
float ACU::getFanRef(bool update){
  if (update) updateFanRef();
  return fan_Ref;
}

void ACU::printIso(){
  Serial.println("-------IMD--------");
  
  Serial.printf("R_ISO_Corrected: R_iso_corrected %u\n R_ISO_Status:", this->IMD.R_iso_corrected);
  Serial.print(this->IMD.R_iso_status, HEX);
  Serial.printf("\n ISO_Meas_Count: %u\n Status_Warnings_Alarms:", this->IMD.iso_meas_count);
  Serial.print(this->IMD.status_warnings_alarms, BIN);
  Serial.printf("\n Status_Device_Activity: %u\n HV_System: %f\n", this->IMD.status_device_activity, this->IMD.hv_system_voltage  );
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

// ========== Function: setRelayState ==========
// drives relays, wait for them to switch, and check output pins' states
// Inputs: 
//   relayState: | Reserved | Reserved | Reserved | Reserved | Reserved | AIR- | AIR+ | Pre_charge
// Returns: 1 if successful, 0 if not successful
bool ACU::setRelayState(uint8_t relayState) {
  uint8_t diff = getRelayState() ^ relayState;
  digitalWrite(PIN_AIR_NEG, relayState & 0b100);
  digitalWrite(PIN_AIR_POS, relayState & 0b010);
  digitalWrite(PIN_PRECHG,  relayState & 0b001);
  delay((diff & 0b110) ? DELAY_AIR_SW : DELAY_PCHG_SW); // wait for relay to switch
  if (getRelayState() == relayState) {
    return true;
  }
  else {
    errs |= ERR_Teensy;
    return false;
  }
}

// ========== Function: resetLatch ==========
// attemps to reset (close) the latch relay
void ACU::resetLatch() {
  digitalWrite(PIN_AIR_RESET, HIGH);
  delay(10);
  digitalWrite(PIN_AIR_RESET, LOW);
}