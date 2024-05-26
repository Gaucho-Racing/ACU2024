#include "FanController.h"
#include "ACU_data.h"
#include "ACU.h"
#include "states.h"
#include "battery.h"
#include <FlexCAN_T4.h>
#include "debug.h"

Battery battery;
ACU acu;
States state;
uint8_t cycle = 0;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_prim;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_256> can_chgr;
CAN_message_t msg;

//changes ISR depending on whether discharging or charging
IntervalTimer dumpCAN; //consider using this in conjunction with mailbox

void setup() {
  Serial.begin(115200);
  //D_L1 and D_L2 are debug print statements
  D_L1("Init config");
  Serial.println("Init config");
  battery.init_config();
  acu.init_config();
  Serial.println("Setup done");
  
  can_prim.begin();
  can_prim.setBaudRate(1000000);
  can_chgr.begin();
  can_chgr.setBaudRate(500000);
  mailboxSetup();

  // needs to wait a bit before good values
  SystemCheck();
  SystemCheck();
  acu.warns = 0;
  acu.errs = 0;
  if(SystemCheck()){
    state = SHUTDOWN;
    D_L1("System check failed, shutting down");
  }
  else{
    state = STANDBY;
    D_L1("System check passed, entering standby");
  }

}

void loop() {
  // ACU STATES
  can_prim.events();
  switch (state)
  {
    case STANDBY:
      standByState();
      break;
    case PRECHARGE:
      preChargeState();
      break;
    case CHARGE:
      chargeState();
      break;
    case NORMAL:
      normalState();
      break;
    case SHUTDOWN:
      shutdownState();
      break;
    default:
      state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }
  // dumpCANbus(battery); //uncomment if interrupt don't work
cycle++;
  #if DEBUG
    debug();
    delay(100);
  #endif

}