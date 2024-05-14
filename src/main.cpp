#include "FanController.h"
#include "ACU_data.h"
#include "ACU.h"
#include "states.h"
#include "battery.h"
#include <FlexCAN_T4.h>
#include "debug.h"





Battery battery;
States state;
// fanController fans(&Serial8); moved under battery

bool tsActive = false;
uint8_t errors = 0b00000000;
//changes ISR depending on whether discharging or charging
IntervalTimer dumpCAN;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_prim;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_256> can_chgr;
CAN_message_t msg;

//wrapper includes battery argument, necessary for passing as a function pointer to IntervalTimer
void primCANISR(){
  dumpCANbus(battery);
}

void chgrCANISR(){
  dumpCANbus(battery);
}

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
// void chargerSendWrapper(){
//   sendCANData(battery, Charger_Control);
// }

//wrapper for battery interrupt

void setup() {
  Serial.begin(115200);
  // fans.begin();
  Serial.println("Init config");
  init_config(battery);
  adBms6830_init_config(TOTAL_IC, battery.IC);
  Serial.println("Setup done");
  pinSetup();
  
  can_prim.begin();
  can_prim.setBaudRate(1000000);
  can_chgr.begin();
  can_chgr.setBaudRate(500000);

  state = STANDBY;

}

void loop() {
  // ACU STATES
  battery.containsError = systemCheck(battery);
  digitalWrite(PIN_AMS_OK, !battery.containsError);
  if (battery.containsError)state = SHUTDOWN;
  switch (state)
  {
    case STANDBY:
      standByState(battery);
      break;
    case PRECHARGE:
      preChargeState(battery);
      break;
    case CHARGE:
      chargeState(battery);
      break;
    case NORMAL:
      normalState(battery);
      break;
    case SHUTDOWN:
      shutdownState(battery);
      break;
    default:
      battery.state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }
  // dumpCANbus(battery); //uncomment if interrupt don't work

  #if DEBUG
    debug(battery);
    delay(100);
  #endif

}