#include "states.h"
void shutdownState(){
  acu.clearPrecharge();
  acu.setShutdown(true);
  dumpCANbus();
  #if DEBUG
    Serial.println("State: Shutdown");
  #endif
  acu.warns = 0;
  //errors can only be reset when shutdown
  acu.errs = 0;
  // Open AIRS and Precharge if already not open, close Discharge

  delayMicroseconds(1);
  digitalWrite(PIN_AIR_RESET, LOW);
  digitalWrite(PIN_PRECHG, LOW);
  digitalWrite(PIN_AIR_NEG, LOW);
  digitalWrite(PIN_AIR_POS, LOW);
  delay(10);

  acu.updateRelayState();
  SystemCheck(true);
  if (acu.getRelayState() != 0) {
    acu.errs |= ERR_Teensy; // Teensy error, output not working
    //TRIAGE 2: lockable loop, might be an issue
    while(acu.errs & ERR_Teensy){
      delayMicroseconds(1);
      digitalWrite(PIN_AIR_RESET, LOW);
      digitalWrite(PIN_PRECHG, LOW);
      digitalWrite(PIN_AIR_NEG, LOW);
      digitalWrite(PIN_AIR_POS, LOW);
      acu.updateRelayState();
    }
  }

  if (acu.getGlvVoltage() < SAFE_V_TO_TURN_OFF) { // safe to turn off if TS voltage < 50V
    state = STANDBY;
    acu.setShutdown(false);
  }
  return;
}

//may newed more??
void normalState(){
  if(SystemCheck()){
    state = SHUTDOWN;
    return;
  };
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

void preChargeReset(){
    digitalWrite(PIN_AIR_RESET, HIGH); 
    delay(50);
    digitalWrite(PIN_AIR_RESET, LOW);
    digitalWrite(PIN_AIR_POS, LOW); // open AIR+, shouldn't be closed but just in case
    digitalWrite(PIN_AIR_NEG, LOW); // open AIR-
    digitalWrite(PIN_PRECHG, LOW); // open precharge relay, close discharge relay
}

//REVIEW BEFORE USE

float Vglv, Vsdp;

void preChargeState(){
  acu.warns = 0;
  #if DEBUG
    // Serial.println("State: preCharge");
  #endif

    Serial.println("Precharge, AIR pins reset");
  digitalWrite(PIN_AIR_POS, LOW);
  digitalWrite(PIN_AIR_NEG, LOW);
  digitalWrite(PIN_PRECHG, LOW);

  Vglv = acu.ACU_ADC.readVoltage(ADC_MUX_GLV_VOLT)*4;
  Vsdp = acu.ACU_ADC.readVoltage(ADC_MUX_SHDN_POW)*4;

  while (Vglv > 16.37) { // 12V is not powered
    Serial.println("GLV not powered");
    Vglv = acu.ACU_ADC.readVoltage(ADC_MUX_GLV_VOLT)*4;
    delay(1000);
  }

  Serial.println("Precharge Start");
  while (Vglv - Vsdp > 0.2) {
    Serial.println("Latch not closed");
    digitalWrite(PIN_AIR_RESET, HIGH); // close latch
    delay(50); // wait for the relay to switch
    digitalWrite(PIN_AIR_RESET, LOW);
    delay(1950);
    Vglv = acu.ACU_ADC.readVoltage(ADC_MUX_GLV_VOLT)*4;
    Vsdp = acu.ACU_ADC.readVoltage(ADC_MUX_SHDN_POW)*4;
  }

    digitalWrite(PIN_AIR_NEG, HIGH); // close AIR-
    delay(100); // wait for the relay to switch
    
    digitalWrite(PIN_PRECHG, HIGH); // close precharge relay
    delay(20); // wait for the relay to switch

    delay(5000); // minimum precharge time
    
  // TODO: send message to VDM to indicate Precharge
  // check voltage, if difference > 5V after 2 seconds throw error
  delay(10000);
  uint32_t startTime = millis();
  while (acu.ACU_ADC.readVoltage(ADC_MUX_HV_VOLT)*200 < battery.getTotalVoltage() * PRECHARGE_THRESHOLD) {
    if (millis() - startTime > 5000) { // timeout, throw error
      digitalWrite(PIN_AIR_POS, LOW); // open AIR+, shouldn't be closed but just in case
      digitalWrite(PIN_AIR_NEG, LOW); // open AIR-
      digitalWrite(PIN_PRECHG, LOW); // open precharge relay, close discharge relay
      Serial.println("Precharge timeout, error");
      delay(2000);
      return;
    }
    delay(100);
  }
    digitalWrite(PIN_AIR_POS, HIGH); // clost AIR+
    delay(50); // wait for the relay to switch

  Serial.println("Precharge Done. Ready to drive. ");
  // if (abs(acu.getGlvVoltage() - acu.getShdnVolt()) > 1) { // if latch is not closed, TRIAGE 2: 100 is probably in some sort of other units that need to be figured out
  //   digitalWrite(PIN_AIR_RESET, HIGH); // close latch
  //   delay(50); // wait for the relay to switch
  //   digitalWrite(PIN_AIR_RESET, LOW);
  //   //TRIAGE 1: fix later
  //   acu.updateAll();
  //   acu.updateShdnVolt();
  //   Serial.println("----------------------------------------");
  //   Serial.println(acu.getShdnVolt());
  //   Serial.println("----------------------------------------");

  //   //battery.containsError = systemCheck(battery); //TRIAGE 2: figure out what to do here
  //   //TRIAGE 1: Make the 1 into a macro
  //   if (abs(acu.getGlvVoltage() - acu.getShdnVolt()) > 1) { //TRIAGE 2: ditto if statement condition above
  //     // D_L1("PRECHARGE: GLV voltage not equal to shutdown voltage, shutting down.");
  //     acu.errs |= ERR_UndrVolt; // Shutdown circuit is not closed
  //     state = SHUTDOWN;
  //     return;
  //   }
  // }
  // if (!(acu.getRelayState() & AIR_NEG)) { // if AIR- isn't closed
  //   digitalWrite(PIN_AIR_NEG, HIGH); // close AIR-
  //   delay(50); // wait for the relay to switch
  //   //battery.containsError = systemCheck(battery);
  //   acu.updateAll();
  //   Serial.println("PRECHARGE1");
  //   if (!(acu.getRelayState() & AIR_NEG)) {
  //     D_L1("PRECHARGE: AIR- not closed, shutting down.");
  //     acu.errs |= ERR_Teensy; // Teensy error, output pin not working
  //     digitalWrite(PIN_AIR_NEG, LOW);
  //     acu.updateAll();
  //     state = SHUTDOWN;
  //     return;
  //   }
  // }
  // if (!(acu.getRelayState() & PRE_CHARGE)) { // if precharge relay isn't closed
  //   digitalWrite(PIN_PRECHG, HIGH); // close precharge relay
  //   delay(10); // wait for the relay to switch
  //   acu.updateAll();
  //   Serial.println("PRECHARGE2");
    
  //   if (!(acu.getRelayState() & PRE_CHARGE)) {
  //     D_L1("PRECHARGE:  not closed, shutting down.");
  //     acu.errs |= ERR_Teensy; // Teensy error, output pin not working
  //     digitalWrite(PIN_PRECHG, LOW);
  //     digitalWrite(PIN_AIR_NEG, LOW);
  //     acu.updateAll();
  //     state = SHUTDOWN;
  //     return;
  //   }
  // }

  // //TRIAGE 1: continue new update
  // // send message to VDM to indicate Precharge
  // acu.getRelayState();
  // sendCANData(ACU_General2);
  // // check voltage, if difference > 5V after 2 seconds throw error
  // uint32_t startTime = millis();
  //   Serial.println("PRECHARGE3");
  // while (acu.getTsVoltage() < battery.getTotalVoltage() * PRECHARGE_THRESHOLD){
  //   // if(SystemCheck()){
  //   //   state = SHUTDOWN;
  //   //   return;
  //   // }
  //   if (millis() - startTime > 3000) { // timeout, throw error
      
      
  //     acu.errs |= ERR_Prechrg;
  //     // battery.containsError = systemCheck(battery);
  //     state = SHUTDOWN;
  //     return;
  //   }
  //   dumpCANbus();
  //   delay(20);
  // }
  // if (!(acu.errs & 0b01000000)) { // if AIR+ isn't closed
  //   digitalWrite(PIN_AIR_POS, HIGH); // clost AIR+
  //   delay(50); // wait for the relay to switch
  //   if(SystemCheck()){
  //     state = SHUTDOWN;
  //     return;
  //   }
  //   if (!(acu.getRelayState() & 0b01000000)) {
  //     acu.errs |= ERR_Teensy; // Teensy error, output pin not working
  //     digitalWrite(PIN_PRECHG, LOW);
  //     digitalWrite(PIN_AIR_NEG, LOW);
  //     digitalWrite(PIN_AIR_POS, LOW);
  //     acu.updateAll();
  //     state = SHUTDOWN;
  //     return;
  //   }
  //   delay(100);
  // }
  // #if DEBUG > 1
  //   Serial.println("Precharge Done. Ready to drive. ");
  // #endif
  // cycle = 0;
  // acu.updateRelayState();
  // acu.prechargeDone();
  // sendCANData(ACU_General2);
  // state = NORMAL;

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
bool SystemCheck(bool fullCheck = false, bool startup = false){
  D_L1("-----------------System Check------------");
  D_L1("Checking ACU");
  acu.checkACU(startup);
  D_L1("Checking Battery");
  battery.checkBattery(fullCheck);
  D_L1("System Check Done");
  D_L1();
  // return acu.errs != 0; true if there are errors, false if there are no errors
  //TRIAGE 1: remove b4 production
  return acu.errs != 0;
}
