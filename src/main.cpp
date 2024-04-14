
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
  //counts the number of seconds since polling can
  uint8_t can_count = 0;
  while(battery.can.charger_can_recieve() == false && battery.can.vdm_can_recieve() == false){
    Serial.println("Waiting for can");
    delay(1000);
    //after 10 seconds shutdown
    if (can_count > 10){
      Serial.println("CAN not connected");
      break;
    }
  }
  if (can_count > 10){
    state = SHUTDOWN;
  }
  else{
    state = STANDBY;
  }
}

void loop() {
  // ACU STATES
  battery.containsError = systemCheck(battery, state);
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
      shutdownState(battery, state);
      break;
    default:
      state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }

  delay(500);
  
}