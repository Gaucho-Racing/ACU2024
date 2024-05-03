#include "FanController.h"
#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830Data.h"
#include "debug.h"


IntervalTimer canTimer;

Battery battery;
States state;
// fanController fans(&Serial8); moved under battery

bool tsActive = false;
uint8_t errors = 0b00000000;


//wrapper includes battery argument, necessary for passing as a function pointer to IntervalTimer
void dumpCANwrapper(){
  dumpCANbus(battery);
}

void test(){
  digitalToggle(13);
}

void setup() {
  Serial.begin(115200);
  // fans.begin();
  Serial.println("Init config");
  adBms6830_init_config(TOTAL_IC, battery.IC);
  Serial.println("Setup done");

  battery.can_prim.begin();
  battery.can_prim.setBaudRate(1000000);
  battery.can_chgr.begin();
  battery.can_chgr.setBaudRate(500000);
  updateAllTemps(battery);
  state = STANDBY;
  canTimer.begin(dumpCANwrapper, 1000);
}

void loop() {
  // ACU STATES
  battery.containsError = systemCheck(battery);
  digitalWrite(PIN_AMS_OK, !battery.containsError);
  if (battery.containsError)battery.state = SHUTDOWN;
  switch (battery.state)
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
    case OFFSTATE:
      offState(battery);
      break;
    default:
      battery.state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }
  // dumpCANbus(battery); //uncomment if interrupt don't work

  #if DEBUG
    debug(battery);
    delay(2000);
  #endif

}