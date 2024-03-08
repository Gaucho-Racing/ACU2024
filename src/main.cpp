
#include "FanController.h"
#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830Data.h"


Battery battery;
States state;

#define TOTAL_IC 2
cell_asic IC[TOTAL_IC];

void setup() {
  Serial.begin(115200);
  Serial.println("Init config");
  adBms6830_init_config(TOTAL_IC, IC);
  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
  state = STANDBY;
  state = STANDBY;
}

void loop() {
  // ACU STATES
  switch (state)
  {
    case STANDBY:
      standByState();
      updateVoltage(battery.cellVoltage, IC);
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