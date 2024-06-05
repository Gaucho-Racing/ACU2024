#include "states.h"
void shutdownState(){
  // Open AIRS and Precharge if already not open, close Discharge
  acu.setRelayState(0);
  digitalWrite(PIN_DCDC_EN, LOW);

  acu.warns = 0;
  //errors can only be reset when shutdown
  bool temp = acu.errs & ERR_Prechrg;
  acu.errs = 0;
  if (temp) acu.errs |= ERR_Prechrg;
  bool checkPass = !SystemCheck(true);
  if (acu.getTsVoltage() < SAFE_V_TO_TURN_OFF && checkPass) { // safe to turn off if TS voltage < 60V
    D_L1("Shutdown (Safe) => Standby");
    state = STANDBY;
  }
  acu.cur_ref += (acu.ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) - acu.cur_ref) * 0.01;
  acu.dcdc_ref += (acu.ACU_ADC.readVoltage(ADC_MUX_DCDC_CURRENT) - acu.dcdc_ref) * 0.01;
  return;
}

void normalState(){
  // Serial.print("Buck bad pin: ");
  // Serial.println(analogRead(PIN_DCDC_ER) / 1024.0 * 3.3);
  if(SystemCheck()){
    Serial.println("SystemCheck failed in NORMAL state");
    state = SHUTDOWN;
    return;
  }
  //cycle maxes out at 8
  cycle++;
  cycle = cycle % 8;

  if (acu.getTsCurrent(false) > 0.5) acu.cur_LastHighTime = millis();
  if (millis() - acu.cur_LastHighTime > 10000) acu.cur_ref += (acu.ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) - acu.cur_ref) * 0.01;

  if (!digitalRead(PIN_DCDC_EN)) acu.dcdc_ref += (acu.ACU_ADC.readVoltage(ADC_MUX_DCDC_CURRENT) - acu.dcdc_ref) * 0.01;
  // if (max(acu.getTemp1(false), acu.getTemp2(false)) > MAX_DCDC_TEMP){
  //   digitalWrite(PIN_DCDC_EN, LOW);
  // }
  // else if (max(acu.getTemp1(false), acu.getTemp2(false)) > MAX_DCDC_TEMP*0.9){
  //   digitalWrite(PIN_DCDC_EN, acu.getTsVoltage(false) > 370 && !digitalRead(PIN_DCDC_ER));
  //   digitalWrite(PIN_DCDC_SLOW, HIGH);
  // }
  // else {
  //   digitalWrite(PIN_DCDC_EN, acu.getTsVoltage(false) > 370 && !digitalRead(PIN_DCDC_ER));
  // }

  return;
}

//TRIAGE 1.5: implement
uint64_t lastChargeTime = 0;
uint64_t lastDischargeTime = 0;
void chargeState(){
  acu.warns = 0;
  //every 2 seconds check if the system is still good
  if(millis() - lastChargeTime > 2000){
    //what needs to be reset
    lastChargeTime = millis();
    if(SystemCheck(true, false)){
      D_L1("CHARGE: System check failed, shutting down");
      state = SHUTDOWN;
      return;
    }
  }

  if (max(acu.getTemp1(false), acu.getTemp2(false)) > MAX_DCDC_TEMP){
    digitalWrite(PIN_DCDC_EN, LOW);
  }
  else if (max(acu.getTemp1(false), acu.getTemp2(false)) > MAX_DCDC_TEMP*0.9){
    digitalWrite(PIN_DCDC_EN, acu.getTsVoltage(false) > 370 && !digitalRead(PIN_DCDC_ER));
    digitalWrite(PIN_DCDC_SLOW, HIGH);
  }
  else {
    digitalWrite(PIN_DCDC_EN, acu.getTsVoltage(false) > 370 && !digitalRead(PIN_DCDC_ER));
  }

  return;
}

void preChargeState(){
  float Vglv, Vsdp;
  digitalWrite(PIN_DCDC_EN, LOW);
  acu.warns = 0;
  acu.errs &= ~ERR_Prechrg;
  #if DEBUG
    D_L1("State: preCharge");
  #endif

  D_L1("Precharge, AIR pins reset");
  if (!acu.setRelayState(0)) {
    D_L1("PreCharge => Shutdown");
    state = SHUTDOWN;
  }

  Vglv = acu.getGlvVoltage();
  Vsdp = acu.getShdnVolt();

  while (Vglv > OPEN_GLV_VOLT) { // 12V is not powered (defaults to max)
    D_L1("GLV not powered");
    D_L1("PreCharge => Shutdown");
    state = SHUTDOWN;
    return;
  }

  D_L1("Precharge Start");
  acu.resetLatch();
  delay(100);
  Vglv = acu.getGlvVoltage();
  Vsdp = acu.getShdnVolt();
  if (abs(Vglv - Vsdp) > ERRMG_GLV_SDC) {
    D_L1("Latch not closed, skill issue");
    delay(1000);
    D_L1("PreCharge => Shutdown");
    state = SHUTDOWN;
    return;
  }

  D_L1("systemCheck");
  if (SystemCheck(true, false)) {
    D_L1("PreCharge => Shutdown");
    state = SHUTDOWN;
    return;
  }
  D_L1("Close AIR-");
  acu.setRelayState(0b100); // close AIR-
  sendCANData(ACU_General2);
  D_L1("Close precharge relay");
  acu.setRelayState(0b101); // close precharge relay
  D_L1("Huh");
  sendCANData(ACU_General2);
    
  // check voltage, if difference > threshold after 2 seconds throw error
  uint32_t startTime = millis();
  while (acu.getTsVoltage() < battery.getTotalVoltage() * PRECHARGE_THRESHOLD) {
    Serial.println(acu.getTsVoltage(false));
    if (SystemCheck()) {
      D_L1("PreCharge (TsVoltage) => Shutdown");
      state = SHUTDOWN;
      return;
    }
    Vglv = acu.getGlvVoltage();
    Vsdp = acu.getShdnVolt();
    if(abs(Vglv - Vsdp) > ERRMG_GLV_SDC){
      Serial.printf("Vglv: %f, Vsdp: %f\n", Vglv, Vsdp);
      Serial.println("SDC voltage dropped while precharging!! Check connections");
      acu.errs |= ERR_Prechrg;
      sendCANData(ACU_General);
      D_L1("PreCharge (ERRMG_GLV_SDC) => Shutdown");
      state = SHUTDOWN;
      return;
    }
    if (millis() - startTime > 5000) { // timeout, throw error
      acu.errs |= ERR_Prechrg;
      sendCANData(ACU_General);
      state = SHUTDOWN;
      D_L1("Precharge timeout, error");
      D_L1("PreCharge (timeout) => Shutdown");
      return;
    }

    readCANData();
    dumpCANbus();
    if (state != PRECHARGE) {
      state = SHUTDOWN;
      D_L1("In Precharge (but incorrect state) => Shutdown");
      return;
    }
    D_L1(acu.getTsVoltage(false));
  }

  // delay 3 seconds, for safety
  startTime = millis();
  while (millis() - startTime < 3000) {
    if (acu.getTsVoltage() < battery.getTotalVoltage() * PRECHARGE_THRESHOLD) {
      state = SHUTDOWN;
      D_L1("Precharge (TS Threshold) => Shutdown");
      acu.errs |= ERR_Prechrg;
      return;
    }
    if (SystemCheck()) {
      D_L1("Precharge (SystemCheck) => Shutdown");
      state = SHUTDOWN;
      return;
    }
    Serial.print("Precharge TS voltage: ");
    Serial.println(acu.getTsVoltage(false));
    dumpCANbus();
  }

  acu.setRelayState(0b111); // close all relays
  sendCANData(ACU_General2);

  D_L1("Precharge Done. Ready to drive. State Normal");
  D_L1("Precharge => Normal");
  state = NORMAL;
  return;
}

//
/// @brief do nothing, in initial state wait for VDM to send start command, might need to poll CAN
void standByState(){
  digitalWrite(PIN_DCDC_EN, LOW);
  SystemCheck();
  cycle++;
  cycle = cycle % 8;
  acu.cur_ref += (acu.ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) - acu.cur_ref) * 0.01;
  acu.dcdc_ref += (acu.ACU_ADC.readVoltage(ADC_MUX_DCDC_CURRENT) - acu.dcdc_ref) * 0.01;
}

//TRIAGE 3: set a macro for fullCheck for readibility; FULL = true, PARTIAL = false
/// @brief 
/// @param fullCheck 
/// @return 1 if check failed
bool SystemCheck(bool fullCheck, bool startup){
  //D_L1("------------System Check------------");
  //D_L1("Checking ACU");
  acu.checkACU(startup);
  //D_L1("Checking Battery");
  battery.checkBattery(fullCheck);
  //D_L1("System Check Done");
  //D_L1();
  digitalWrite(PIN_AMS_OK, acu.errs == 0);
  return acu.errs != 0;
}
