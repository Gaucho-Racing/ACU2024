#include "states.h"
void shutdownState(){
  // Open AIRS and Precharge if already not open, close Discharge
  acu.setRelayState(0);

  //dumpCANbus();////////////////////////////////////////FIX
  acu.warns = 0;
  //errors can only be reset when shutdown
  bool temp = acu.errs & ERR_Prechrg;
  acu.errs = 0;
  if (temp) acu.errs |= ERR_Prechrg;
  bool checkPass = !SystemCheck(true);
  if (acu.getTsVoltage() < SAFE_V_TO_TURN_OFF && checkPass) { // safe to turn off if TS voltage < 60V
    state = STANDBY;
  }
  return;
}

void normalState(){
  if(SystemCheck()){
    state = SHUTDOWN;
    return;
  }
  //cycle maxes out at 8
  cycle++;
  cycle = cycle % 8;

  if (acu.getTsCurrent(false) > 0.5) acu.cur_LastHighTime = millis();
  if (millis() - acu.cur_LastHighTime > 10000) acu.cur_ref += (acu.ACU_ADC.readVoltageTot(ADC_MUX_HV_CURRENT, 256) - acu.cur_ref) * 0.01;

  return;
}

//TRIAGE 1.5: implement
void chargeState(){
  acu.warns = 0;
  return;
}

float Vglv, Vsdp;

void preChargeState(){
  acu.warns = 0;
  acu.errs &= ~ERR_Prechrg;
  #if DEBUG
    D_L1("State: preCharge");
  #endif

  D_L1("Precharge, AIR pins reset");
  if (!acu.setRelayState(0)) {
    state = SHUTDOWN;
  }

  Vglv = acu.getGlvVoltage();
  Vsdp = acu.getShdnVolt();

  while (Vglv > OPEN_GLV_VOLT) { // 12V is not powered (defaults to max)
    D_L1("GLV not powered");
    state = SHUTDOWN;
    return;
  }

  D_L1("Precharge Start");
  acu.resetLatch();
  delay(100);
  Vglv = acu.getGlvVoltage();
  Vsdp = acu.getShdnVolt();
  if (abs(Vglv - Vsdp) > ERRMG_GLV_SDC) {
    D_L1("Latch not closed");
    state = SHUTDOWN;
    return;
  }

  acu.setRelayState(0b100); // close AIR-
  sendCANData(ACU_General2);
  acu.setRelayState(0b101); // close precharge relay
  sendCANData(ACU_General2);
  if (acu.errs) {
    state = SHUTDOWN;
    return;
  }
    
  // check voltage, if difference > threshold after 2 seconds throw error
  uint32_t startTime = millis();
  while (acu.getTsVoltage() < battery.getTotalVoltage() * PRECHARGE_THRESHOLD) {
    Serial.println(acu.getTsVoltage());
    Vglv = acu.getGlvVoltage();
    Vsdp = acu.getShdnVolt();
    if(abs(Vglv - Vsdp) > ERRMG_GLV_SDC){
      Serial.printf("Vglv: %f, Vsdp: %f\n", Vglv, Vsdp);
      D_L1("SDC voltage dropped while precharging!! Check connections");
      acu.errs |= ERR_Prechrg;
      sendCANData(ACU_General);
      state = SHUTDOWN;
      return;
    }
    if (millis() - startTime > 5000) { // timeout, throw error
      acu.errs |= ERR_Prechrg;
      sendCANData(ACU_General);
      state = SHUTDOWN;
      D_L1("Precharge timeout, error");
      return;
    }

    readCANData();
    sendCANData(ACU_General);
    sendCANData(ACU_General2);
    if (state != PRECHARGE) {
      state = SHUTDOWN;
      return;
    }
    D_L1(acu.getTsVoltage(false));
  }
  acu.setRelayState(0b111); // close all relays
  sendCANData(ACU_General2);

  D_L1("Precharge Done. Ready to drive. State Normal");
  state = NORMAL;
  return;
}

//
/// @brief do nothing, in initial state wait for VDM to send start command, might need to poll CAN
void standByState(){
  acu.warns = 0;
  //battery.disable_Mux();
  SystemCheck();
  cycle++;
  cycle = cycle % 8;
  acu.cur_ref += (acu.ACU_ADC.readVoltageTot(ADC_MUX_HV_CURRENT, 256) - acu.cur_ref) * 0.01;
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
  // return acu.errs != 0; true if there are errors, false if there are no errors
  //TRIAGE 1: remove b4 production
  digitalWrite(PIN_AMS_OK, acu.errs == 0);
  return acu.errs != 0;
}
