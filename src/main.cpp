#include "ADBMS.h"
#include "FanController.h"
#include "ACU.h"


Battery battery;
States state;


void setup() {
  Serial.begin(115200);
  fans.begin();
  Serial.println("Init config");

  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
}

void loop() {
  // ACU STATES
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

  delay(500);
  
}