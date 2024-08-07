#include "FanController.h"
#include "ACU_data.h"
#include "ACU.h"
#include "states.h"
#include "battery.h"
#include <FlexCAN_T4.h>
#include "debug.h"

Battery battery;
ACU acu;
States state = STANDBY;
uint8_t cycle = 0;  //Counter for ADI temp readings

FlexCAN_T4<CAN3, RX_SIZE_1024, TX_SIZE_1024> can_prim;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_chgr;
CAN_message_t msg;
uint64_t prev_mill = 0;

//changes ISR depending on whether discharging or charging
IntervalTimer dumpCAN; //consider using this in conjunction with mailbox
uint64_t lastTime = 0;
void setup() {
  // //D_L1 and D_L2 are debug print statements
  Serial.begin(1000000);
  
  
  D_L1("can_chgr begin");
  can_chgr.begin();
  can_chgr.setBaudRate(500000); 

  D_L1("can_prim begin");
  can_prim.begin();
  can_prim.setBaudRate(1000000);

  delay(1000); // required so that the CAN bus can initialize properly

  
  D_L1("Init config");
  acu.init_config();
  battery.init_config();
  D_L1("Setup done");
  Serial.println("CAN interfaces initialized successfully");

  //mailboxSetup();

  // needs to wait a bit before good values
  acu.warns = 0;
  acu.errs = 0;

  if(SystemCheck(true, true)){
    state = SHUTDOWN;
     D_L1("State => Shutdown");
    debug();
    D_L1("System check failed, shutting down (But not rly cuz its commented out)");
  }
  else{
    state = STANDBY;
    D_L1("System check passed, entering standby");
  }

}


void loop() {
  switch (state)
  {
    case STANDBY:
      standByState();
      break;

    case PRECHARGE:
      preChargeState();
      break;

    case CHARGE:
      chargeState();  //TODO
      break;

    case NORMAL:
      normalState();
      break;

    case SHUTDOWN:
      shutdownState();
      break;

    default:
      state = SHUTDOWN;
      D_L1("Uh oh u dummy, u've entered a non-existent state");
      // delay(10000);
      break;
  }
  
  readCANData();
  dumpCANbus(); //uncomment if interrupt don't work

  float maxTemp = max(acu.getTemp1(false), acu.getTemp2(false));
  if (maxTemp < 30) {
    acu.fans.writeRegister(FAN_SPD4_addr, 0);
  }
  else {
    maxTemp = constrain(maxTemp, 30, 60);
    acu.fans.writeRegister(FAN_SPD4_addr, map(maxTemp, 30, 60, 1500, 3500));
  }

  #ifdef DEBUG
    if(millis() - prev_mill > 500){
      prev_mill = millis();
      debug();
    }
  #endif

}