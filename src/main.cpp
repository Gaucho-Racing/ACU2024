#include "ADBMS.h"
#include "FanController.h"
#include "ACU.h"


Battery battery;
States state;


void setup() {
  Serial.begin(115200);
  // fans.begin();
  Serial.println("Init config");

  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
}

void loop() {
  // ACU STATES
  systemCheck(battery);
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
      state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }

  delay(500);
  
}