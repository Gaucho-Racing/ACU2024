#include "FanController.h"
#include "ACU.h"


Battery battery;
States state;
fanController fans(&Serial8);

float accumVoltage, accumCurrent, tsVoltage;
float acuTemp[3]; // DC-DC converter, something, something

uint16_t fanRpm[4];
float fanVoltage[4];
float fanCurrent[4];

bool tsActive = false;
uint8_t errors = 0b00000000;



void setup() {
  Serial.begin(115200);
  // fans.begin();
  Serial.println("Init config");

  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
  state = STANDBY;

}

void loop() {
  // ACU STATES  
  systemCheckOk = systemCheck(battery, state);
  switch (state)
  {
    case STANDBY:
      standByState(battery, state);
      break;
    case PRECHARGE:
      preChargeState(battery, state);
      break;
    case CHARGE:
      chargeState(battery, state);
      break;
    case NORMAL:
      normalState(battery, state);
      break;
    case SHUTDOWN:
      shutdownState(battery, state, systemCheckOk, tsActive);
      break;
    default:
      state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }

  delay(500);
  
}