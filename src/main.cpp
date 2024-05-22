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
// fanController fans(&Serial8); moved under battery

bool tsActive = false;
uint8_t errors = 0b00000000;
//changes ISR depending on whether discharging or charging
IntervalTimer dumpCAN;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_prim;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_256> can_chgr;
CAN_message_t msg;

//wrapper includes battery argument, necessary for passing as a function pointer to IntervalTimer
// void primCANISR(){
//   dumpCANbus(battery);
// }

// void chgrCANISR(){
//   dumpCANbus(battery);
// }

void pinSetup(){
  pinMode(PIN_IMD_OK, INPUT_PULLUP);  
  pinMode(PIN_AMS_OK, OUTPUT);
  pinMode(PIN_DCDC_EN, OUTPUT);
  pinMode(PIN_DCDC_SLOW, OUTPUT);
  pinMode(PIN_DCDC_ER, INPUT);
  pinMode(PIN_BSPD_CLK, OUTPUT);
  analogWriteFrequency(PIN_BSPD_CLK, 50000);
  analogWrite(PIN_BSPD_CLK, 127);
}

//

// void chargerSendWrapper(){
//   sendCANData(battery, Charger_Control);
// }

//wrapper for battery interrupt

void setup() {
  Serial.begin(115200);
  // fans.begin();
  Serial.println("Init config");
  battery.init_config();
  acu.init_config();
  Serial.println("Setup done");
  pinSetup();
  
  can_prim.begin();
  can_prim.setBaudRate(1000000);
  can_chgr.begin();
  can_chgr.setBaudRate(500000);

  // needs to wait a bit before good values
  SystemCheck();
  SystemCheck();
  acu.warns = 0;
  acu.errs = 0;
  if(SystemCheck()){
    state = SHUTDOWN;
    #if DEBUG > 1
      Serial.println("System check failed, shutting down");
    #endif
  }
  else{
    state = STANDBY;
    #if DEBUG > 1
      Serial.println("System check passed, entering standby");
    #endif
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
    debug(battery);
    delay(100);
  #endif

}