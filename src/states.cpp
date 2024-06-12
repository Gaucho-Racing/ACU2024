#include "states.h"
void shutdownState(){
  // Open AIRS and Precharge if already not open, close Discharge
  digitalWrite(PIN_DCDC_EN, LOW);
  acu.setRelayState(0);

  //indicates to battery to stop charging, should fall on deaf ears if not charging
  sendCANData(Charger_Control);
  battery.resetDischarge();
  
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
  if(SystemCheck() || abs(acu.getTsVoltage(false) < SAFE_V_TO_TURN_OFF)){
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

  if (acu.getTemp1(false) > MAX_DCDC_TEMP){
    digitalWrite(PIN_DCDC_EN, LOW);
  }
  else {
    if (!digitalRead(PIN_DCDC_EN) && acu.getGlvVoltage(false) < 11 && acu.getTemp1(false) < MAX_DCDC_TEMP - 5){
      digitalWrite(PIN_DCDC_EN, acu.getTsVoltage(false) > 370 && !digitalRead(PIN_DCDC_ER));
    }
    else if (digitalRead(PIN_DCDC_EN) && acu.getGlvVoltage(false) > 13.5) {
      digitalWrite(PIN_DCDC_EN, LOW);
    }
  }

  return;
}

//TRIAGE 1.5: implement
uint32_t lastChargeTime = 0;
uint32_t lastDischargeTime = 0;
uint32_t lastSendTime = 0;
uint32_t lastCaliTime = 0;

void chargeState(){
  acu.warns = 0;
  //every 2 seconds check if the system is still good
  acu.checkACU();
  if(millis() - lastChargeTime >= 2000){
    battery.resetDischarge();
    //what needs to be reset
    lastChargeTime = millis();
    if(SystemCheck(true, false)){
      D_L1("CHARGE: System check failed, shutting down");
      state = SHUTDOWN;
      return;
    }
    //doesn't need to check Voltage b/c done in the sysCheck
    battery.cell_Balancing();
  }

  //every 0.99 seconds send charger "ping"
  if(millis() - lastSendTime > 990){
    lastSendTime = millis();
    if(battery.maxCellVolt > 4.15){
      battery.max_chrg_current = map(battery.maxCellVolt, OV_THRESHOLD-0.06, OV_THRESHOLD-0.01, acu.max_chrg_current, 0);
      battery.max_chrg_current = constrain(battery.max_chrg_current, 0, acu.max_chrg_current);
    } else {
      battery.max_chrg_current = acu.max_chrg_current;
    }
    battery.max_chrg_voltage = acu.max_chrg_voltage;
    sendCANData(Charger_Control);
  }

  //if no CAN data for 5 seconds, shut down
  if(millis() - acu.getLastChrgRecieveTime() > 5000){
    D_L1("CHARGE: Charger CAN timeout, shutting down");
    state = SHUTDOWN;
    return;
  }

  // re-measure current sensor ref every 5 minutes
  if (millis() - lastCaliTime > 300000) {
    lastCaliTime = millis();
    state = NORMAL; // turn off charger
    sendCANData(Charger_Control);
    delay(1000);
    acu.cur_ref = acu.ACU_ADC.readVoltageTot(ADC_MUX_HV_CURRENT, 1024);
    state = CHARGE; // turn charger back on
    sendCANData(Charger_Control);
  }

  //if done charging shut down
  // if(battery.getTotalVoltage() > CHARGER_VOLTAGE){
  //   D_L1("CHARGE: Done charging, shutting down");
  //   state = SHUTDOWN;
  //   sendCANData(Charger_Control);
  //   return;
  // }
  

  if (acu.getTemp1(false) > MAX_DCDC_TEMP && digitalRead(PIN_DCDC_EN)){
    digitalWrite(PIN_DCDC_EN, LOW);
  }
  else if ((acu.getTemp1(false) < MAX_DCDC_TEMP - 5 && !digitalRead(PIN_DCDC_EN))) {
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
    Serial.print("Precharging... "); Serial.println(acu.getTsVoltage(false));
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
      //state = SHUTDOWN;
      //D_L1("In Precharge (but incorrect state) => Shutdown");
      D_L1(state);
      //return;
    }
  }
   
  // delay 3 seconds, for safety
  startTime = millis();
  CAN_message_t msg;
  bool goToCharge = false; // change this to false on final build
  while (millis() - startTime < 3000) {
    acu.checkACU(false);
    if (can_chgr.read(msg)) {
      if (msg.id == Charger_Data) {
        goToCharge = true;
      }
    }

    if (acu.getTsVoltage() < battery.getTotalVoltage() * PRECHARGE_THRESHOLD) {
      state = SHUTDOWN;
      D_L1("Precharge (TS Threshold) => Shutdown");
      acu.errs |= ERR_Prechrg;
      return;
    }
    // if (SystemCheck()) {
    //   D_L1("Precharge (SystemCheck) => Shutdown");
    //   state = SHUTDOWN;
    //   return;
    // }
    Serial.print("waiting... "); Serial.println(acu.getTsVoltage(false));
    dumpCANbus();
    delay(50);
  }

  acu.setRelayState(0b111); // close all relays
  sendCANData(ACU_General2);

  D_L1("Precharge Done. Ready to drive. State Normal");
  state = goToCharge ? CHARGE : NORMAL;
  acu.cur_ref = acu.ACU_ADC.readVoltageTot(ADC_MUX_HV_CURRENT, 1024);
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
