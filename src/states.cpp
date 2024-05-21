#include "states.h"
void shutdownState(){
  dumpCANbus();
  #if DEBUG
    Serial.println("State: Shutdown");
  #endif
  acu.warns = 0;
  //errors can only be reset when shutdown
  acu.errs = 0;
  // Open AIRS and Precharge if already not open, close Discharge
  digitalWrite(PIN_PRECHG, LOW);
  digitalWrite(PIN_AIR_NEG, LOW);
  digitalWrite(PIN_AIR_POS, LOW);
  delay(10);

  acu.updateRelayState();
  SystemCheck(true);
  if (acu.getRelayState() != 0) {
    acu.errs |= ERR_Teensy; // Teensy error, output not working
    while(acu.errs & ERR_Teensy){
      digitalWrite(PIN_PRECHG, LOW);
      digitalWrite(PIN_AIR_NEG, LOW);
      digitalWrite(PIN_AIR_POS, LOW);
      acu.updateRelayState();
    }
  }

  if (acu.getGlvVoltage() < SAFE_V_TO_TURN_OFF) { // safe to turn off if TS voltage < 50V
    state = STANDBY;
  }
  return;
}

//may newed more??
void normalState(){
  if(SystemCheck()){
    state = SHUTDOWN;
    return;
  };
  // TRIAGE 1: CAN?
  //cycle maxes out at 8
  cycle++;
  cycle = cycle % 9;
  dumpCANbus();
  return;
}

//TRIAGE 1.5: implement
void chargeState(){
  acu.warns = 0;
  return;
}

//REVIEW BEFORE USE
void preChargeState(){
  acu.warns = 0;
  #if DEBUG
    Serial.println("State: preCharge");
  #endif
  if (acu.getGlvVoltage() - acu.getShdnVolt() > 100) { // if latch is not closed, TRIAGE 2: 100 is probably in some sort of other units that need to be figured out
    digitalWrite(PIN_AIR_RESET, HIGH); // close latch
    delay(50); // wait for the relay to switch
    digitalWrite(PIN_AIR_RESET, LOW);
    //battery.containsError = systemCheck(battery); //TRIAGE 2: figure out what to do here
    if (acu.getGlvVoltage() - acu.getShdnVolt() > 100) { //TRIAGE 2: ditto if statement condition above
      acu.errs |= ERR_UndrVolt; // Shutdown circuit is not closed
      return;
    }
  }
  if (!(acu.getRelayState() & AIR_NEG)) { // if AIR- isn't closed
    digitalWrite(PIN_AIR_NEG, HIGH); // close AIR-
    delay(50); // wait for the relay to switch
    //battery.containsError = systemCheck(battery);
    if (!(acu.getRelayState() & AIR_NEG)) {
      acu.errs |= ERR_Teensy; // Teensy error, output pin not working
      digitalWrite(PIN_AIR_NEG, LOW);
      return;
    }
  }
  if (!(acu.getRelayState() & PRE_CHARGE)) { // if precharge relay isn't closed
    digitalWrite(PIN_PRECHG, HIGH); // close precharge relay
    delay(10); // wait for the relay to switch
    
    if (!(acu.getRelayState() & PRE_CHARGE)) {
      acu.errs |= ERR_Teensy; // Teensy error, output pin not working
      digitalWrite(PIN_PRECHG, LOW);
      digitalWrite(PIN_AIR_NEG, LOW);
      return;
    }
  }

  //TRIAGE 1: continue new update
  // send message to VDM to indicate Precharge
  sendCANData(ACU_General2);
  // check voltage, if difference > 5V after 2 seconds throw error
  uint32_t startTime = millis();
  while (acu.getTsVoltage() < battery.getTotalVoltage() * PRECHARGE_THRESHOLD) {
    SystemCheck();
    if(acu.errs){
      state = SHUTDOWN;
      return;
    }
    if (millis() - startTime > 3000) { // timeout, throw error
      digitalWrite(PIN_AIR_POS, LOW); // open AIR+, shouldn't be closed but just in case
      digitalWrite(PIN_AIR_NEG, LOW); // open AIR-
      digitalWrite(PIN_PRECHG, LOW); // open precharge relay, close discharge relay
      acu.errs |= ERR_Prechrg;
      // battery.containsError = systemCheck(battery);
      return;
    }
    dumpCANbus();
    delay(20);
  }
  if (!(acu.errs & 0b01000000)) { // if AIR+ isn't closed
    digitalWrite(PIN_AIR_POS, HIGH); // clost AIR+
    delay(50); // wait for the relay to switch
    SystemCheck();
    if(acu.errs){
      state = SHUTDOWN;
      return;
    }
    if (!(acu.getRelayState() & 0b01000000)) {
      acu.errs |= ERR_Teensy; // Teensy error, output pin not working
      digitalWrite(PIN_PRECHG, LOW);
      digitalWrite(PIN_AIR_NEG, LOW);
      digitalWrite(PIN_AIR_POS, LOW);
      return;
    }
  }
  #if DEBUG > 1
    Serial.println("Precharge Done. Ready to drive. ");
  #endif
  cycle = 0;
  state = NORMAL;

  return;
}

//
/// @brief do nothing, in initial state wait for VDM to send start command, might need to poll CAN
void standByState(){
  acu.warns = 0;
  battery.disable_Mux();
  for(int i = 0; i < 30; i++){
    readCANData();
  }
}

//TRIAGE 3: set a macro for fullCheck for readibility; FULL = true, PARTIAL = false
/// @brief 
/// @param fullCheck 
bool SystemCheck(bool fullCheck = false){
  acu.checkACU();
  battery.checkBattery(fullCheck);
  return acu.errs != 0;
}
/*

/// @brief standby, send data --> VDM
/// @param[in] battery
/// @return N/A
void standByState(Battery &battery){
  //STUB
  #if DEBUG
    Serial.println("State: Standby");
  #endif
    for (int i = 0; i < 30; i++){
      
      switch (readCANData(battery))
      {
      case 0:
        //normal operations
        //send can start precharge
        return;
      case 1:
        //charging
        battery.state = CHARGE;
        //send charge parameters then acknowledgement ping
        sendCANData(battery, Charger_Control);
        battery.prevMillis = millis();

        break;
      case -1:
        delay(100);
        break;
      default:
        break;
      }
    }
}


bool systemCheck(Battery &battery) {
  // read stuff on ADC1283 chip
  battery.warns = 0; // clear warnings
  battery.errs &= ~(ERR_OverCurr|ERR_OverVolt|ERR_UndrVolt|ERR_OverTemp|ERR_UndrTemp); // clear all error bit that are checked in sysCheck

  battery.ts_voltage = battery.ACU_ADC.readVoltage(ADC_MUX_HV_VOLT) * 150;
  battery.accumCurrent = (battery.ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) - battery.accumCurrentZero) * 6250;
  if (battery.accumCurrent > 6300) {
    battery.warns |= WARN_HighCurr;
    if (battery.accumCurrent > 13500) {
      battery.errs |= ERR_OverCurr;
    }
  }
  if (V2T(battery.ACU_ADC.readVoltage(ADC_MUX_DCDC_TEMP1), 3380) > 60) {
    if (V2T(battery.ACU_ADC.readVoltage(ADC_MUX_DCDC_TEMP1), 3380) > 70) {
      digitalWrite(PIN_DCDC_EN, LOW);
    }
    digitalWrite(PIN_DCDC_SLOW, HIGH);
  }
  // TODO: ACU temperatures, DC/DC current

  // check relay states by reading on the output pins (should work?)
  battery.relay_state = (digitalRead(PIN_AIR_NEG) << 7) + (digitalRead(PIN_AIR_POS) << 6) + (digitalRead(PIN_PRECHG) << 5);
  
  // every 10 cycles recheck Voltage while in charge
  if(battery.chargeCycle>0 && battery.state == CHARGE){
  } else {
    updateVoltage(battery);
  }
  if (battery.temp_cycle == 0) updateTemps(battery);

  if(battery.maxBalTemp == -1) battery.maxBalTemp = battery.balTemp[0];
  if(battery.maxCellTemp == -1) battery.maxCellTemp = battery.cellTemp[0];
  if(battery.minVolt == -1) battery.minVolt = battery.cellVoltage[0];
  // if(battery.minCellTemp == -1) battery.minCellTemp = battery.cellTemp[0];


  //check CellTemp:
  for (int i = 0; i < TOTAL_IC * 16 * 2; i++){
    if (battery.maxCellTemp < battery.cellTemp[i]) battery.maxCellTemp = battery.cellTemp[i];
    if (battery.state == CHARGE){
      if (battery.cellTemp[i] > MAX_CHR_TEMP) {
        battery.errs |= ERR_OverTemp;
      }
      if (battery.cellTemp[i] < MIN_CHR_TEMP) {
        battery.errs |= ERR_UndrTemp;
      }
    }else{
      if (battery.cellTemp[i] > MAX_DIS_TEMP) {
        battery.errs |= ERR_OverTemp;
      }
      if (battery.cellTemp[i] < MIN_DIS_TEMP){
        battery.errs |= ERR_UndrTemp;
      }
    }
  }

  if(digitalRead(PIN_DCDC_ER) == HIGH || digitalRead(PIN_IMD_OK) == LOW){
    battery.errs |= ERR_BMS;
  }
  
  return battery.errs != 0;
}



/// @brief shutDown, send errors --> VDM
/// @param[in] battery
/// @return N/A
void shutdownState(Battery &battery){
  #if DEBUG
    Serial.println("State: Shutdown");
  #endif
  // Open AIRS and Precharge if already not open, close Discharge
  digitalWrite(PIN_PRECHG, LOW);
  digitalWrite(PIN_AIR_NEG, LOW);
  digitalWrite(PIN_AIR_POS, LOW);
  delay(10);
  battery.containsError = systemCheck(battery);
  if (battery.relay_state != 0) {
    battery.errs |= ERR_Teensy; // Teensy error, output not working
  }
  else if (battery.ts_voltage < 5000) { // safe to turn off if TS voltage < 50V
    battery.state = OFFSTATE;
  }
  for(int ic = 0; ic < TOTAL_IC; ic++){
    battery.IC[ic].tx_cfgb.dcc = 0x0; // disable discharge
    battery.IC[ic].tx_cfga.gpo &= ~(1<<10); // disable mux
  }
  
}

/// @brief timeout checks, system checks, batt data --> VDM
/// @param[in] battery
/// @return N/A
void normalState(Battery &battery){ // ready to drive
 #if DEBUG
    Serial.println("State: Normal");
  #endif
  digitalWrite(PIN_DCDC_EN, HIGH);
  // control fans & pump --> TODO
}

/// @brief req charge, system checks
/// @param[in] battery
/// @return TBD
void chargeState(Battery &battery){
   #ifdef DEBUG
    Serial.println("State: Charge");
  #endif
  //if next chargeCycle is 0 and Charging, get ready for cell measurement by turing off discharge
  if(battery.chargeCycle >= 9 && battery.state == CHARGE){
    battery.chargeCycle = 0;
    for(int ic = 0; ic < TOTAL_IC; ic++){
      battery.IC[ic].tx_cfgb.dcc = 0x0;
    }
  }
  adBms6830_write_read_config(TOTAL_IC, battery.IC);

  // sendMsg if time 0.5 s reached --> TODO
  // if charge full --> send to standby
  if (millis() - battery.prevMillis > 500) {
    battery.prevMillis = millis();
    sendCANData(battery, Charger_Control);
  }
}

/// @brief error --> VDM if timeout --> (NORMAL/SHUTDOWN)
/// @param[in] battery
/// @return TBD
void preChargeState(Battery &battery){
  #ifdef DEBUG
    Serial.println("State: preCharge");
  #endif
  if (battery.glv_voltage - battery.sdc_voltage > 100) { // if latch is not closed
    digitalWrite(PIN_AIR_RESET, HIGH); // close latch
    delay(50); // wait for the relay to switch
    digitalWrite(PIN_AIR_RESET, LOW);
    battery.containsError = systemCheck(battery);
    if (battery.glv_voltage - battery.sdc_voltage > 100) {
      battery.errs |= ERR_UndrVolt; // Shutdown circuit is not closed
      return;
    }
  }
  if (!(battery.relay_state & 0b10000000)) { // if AIR- isn't closed
    digitalWrite(PIN_AIR_NEG, HIGH); // close AIR-
    delay(50); // wait for the relay to switch
    battery.containsError = systemCheck(battery);
    if (!(battery.relay_state & 0b10000000)) {
      battery.errs |= ERR_Teensy; // Teensy error, output pin not working
      digitalWrite(PIN_AIR_NEG, LOW);
      return;
    }
  }
  if (!(battery.relay_state & 0b00100000)) { // if precharge relay isn't closed
    digitalWrite(PIN_PRECHG, HIGH); // close precharge relay
    delay(10); // wait for the relay to switch
    battery.containsError = systemCheck(battery);
    if (!(battery.relay_state & 0b00100000)) {
      battery.errs |= ERR_Teensy; // Teensy error, output pin not working
      digitalWrite(PIN_PRECHG, LOW);
      digitalWrite(PIN_AIR_NEG, LOW);
      return;
    }
  }
  // send message to VDM to indicate Precharge
  sendCANData(battery, ACU_General2);
  // check voltage, if difference > 5V after 2 seconds throw error
  uint32_t startTime = millis();
  while (battery.ts_voltage < getAccumulatorVoltage(battery) - PRECHARGE_THRESHOLD) {
    battery.containsError = systemCheck(battery);
    if (millis() - startTime > 2000) { // timeout, throw error
      digitalWrite(PIN_AIR_POS, LOW); // open AIR+, shouldn't be closed but just in case
      digitalWrite(PIN_AIR_NEG, LOW); // open AIR-
      digitalWrite(PIN_PRECHG, LOW); // open precharge relay, close discharge relay
      battery.errs |= ERR_Prechrg;
      battery.containsError = systemCheck(battery);
      return;
    }
    dumpCANbus(battery);
    delay(20);
  }
  if (!(battery.relay_state & 0b01000000)) { // if AIR+ isn't closed
    digitalWrite(PIN_AIR_POS, HIGH); // clost AIR+
    delay(50); // wait for the relay to switch
    battery.containsError = systemCheck(battery);
    if (!(battery.relay_state & 0b01000000)) {
      battery.errs |= ERR_Teensy; // Teensy error, output pin not working
      digitalWrite(PIN_PRECHG, LOW);
      digitalWrite(PIN_AIR_NEG, LOW);
      digitalWrite(PIN_AIR_POS, LOW);
      return;
    }
  }
  Serial.println("Precharge Done. Ready to drive. ");
  battery.state = NORMAL;
}



*/    