
#include "FanController.h"
#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830Data.h"


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
  adBms6830_init_config(TOTAL_IC, IC);
  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
  //counts the number of seconds since polling can

  for(int i = 0; i < 10; i++){
    if(battery.can.charger_can_recieve() == true){
      state = CHARGE;
      return;
    } else if (battery.can.vdm_can_recieve() == true){
      state = PRECHARGE;
      return;
    }
    Serial.println("Waiting for can");
    delay(1000);
  }
  Serial.println("CAN not connected");
  state = SHUTDOWN;
}

void loop() {
  // ACU STATES
  battery.containsError = systemCheck(battery, state);
  switch (state)
  {
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
    case OFFSTATE:
      offState(battery, state);
      break;
    default:
      state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }
  //TODO: Uncomment when 
  delay(500);
  
}