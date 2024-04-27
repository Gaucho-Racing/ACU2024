#include "FanController.h"
#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830Data.h"

#define DEBUG 1

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
  adBms6830_init_config(TOTAL_IC, battery.IC);
  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);

  battery.can_prim.begin();
  battery.can_prim.setBaudRate(1000000);
  battery.can_chgr.begin();
  battery.can_chgr.setBaudRate(500000);
  state = STANDBY;
}

void loop() {
  // ACU STATES
  // battery.containsError = systemCheck(battery);
  // digitalWrite(PIN_AMS_OK, !battery.containsError);
  // if (battery.containsError)battery.state = SHUTDOWN;
  // switch (battery.state)
  // {
  //   case STANDBY:
  //     standByState(battery);
  //     break;
  //   case PRECHARGE:
  //     preChargeState(battery);
  //     break;
  //   case CHARGE:
  //     chargeState(battery);
  //     break;
  //   case NORMAL:
  //     normalState(battery);
  //     break;
  //   case SHUTDOWN:
  //     shutdownState(battery);
  //     break;
  //   case OFFSTATE:
  //     offState(battery);
  //     break;
  //   default:
  //     battery.state = SHUTDOWN;
  //     Serial.println("Uh oh u dummy, u've entered a non-existent state");
  //     break;
  // }
  // dumpCANbus(battery);
  updateTemps(battery);
  for(int i = 0; i < TOTAL_IC*16; i++){
    Serial.printf("[% 3u]%5.01f; ", i, battery.balTemp[i]);
    if (i % 8 == 7)Serial.write('\n');
  }
  #if DEBUG
   delay(2000);
  #endif
  
}