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

FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_256> can_prim;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_chgr;
CAN_message_t msg;
uint64_t prev_mill = 0;

//changes ISR depending on whether discharging or charging
IntervalTimer dumpCAN; //consider using this in conjunction with mailbox

void setup() {
  Serial.begin(115200);
  // //D_L1 and D_L2 are debug print statements
  D_L1("Init config");
  acu.init_config();
  battery.init_config();
  D_L1("Setup done");

  
  can_prim.begin();
  can_prim.setBaudRate(1000000);
  can_chgr.begin();
  can_chgr.setBaudRate(500000);
  //mailboxSetup();

  // needs to wait a bit before good values
  
  acu.warns = 0;
  acu.errs = 0;

  // TRIAGE 1: create a syscheck function
  if(SystemCheck(true, true)){
    // state = SHUTDOWN;
    debug();
    D_L1("System check failed, shutting down");
  }
  else{
    state = STANDBY;
    D_L1("System check passed, entering standby");
  }

}

void loop() {

  // ACU STATES
    // msg.id = 0x69420;
    // msg.flags.extended = true;
    // uint16_t tsVoltage = acu.getTsVoltage() * 1000;
    // for(int i = 0; i < 8; i++)
    //   msg.buf[i] = 0;
    // can_prim.write(msg); 

  
  //Serial.println("here");
  // can_prim.events();
  // acu.prechargeDone();
  // readCANData();
  if(can_prim.read(msg)){
    if(msg.id == 0x97){
    if(state == STANDBY){
      state = PRECHARGE;
    } else {
      state = SHUTDOWN;
    
    }
    }
  }
  // sendCANData(ACU_General2);
  
   
  switch (state)
  {
    
    case STANDBY:
      standByState();

      break;
    case PRECHARGE:{
      preChargeState();

      break;
      }
//     case CHARGE:
//       chargeState();
//       break;
//     case NORMAL:
//       normalState();
//       break;
    case SHUTDOWN:
      shutdownState();
      Serial.println("Shutting down");
      break;
    default:
      state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      // delay(10000);
      break;
  }
//   // dumpCANbus(battery); //uncomment if interrupt don't work

  #if DEBUG
    if(millis() - prev_mill > 1000){
      prev_mill = millis();
      debug();
      Serial.println(state);
    }
    // delay(1000);
  #endif

}