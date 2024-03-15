
#include "FanController.h"
#include "ACU.h"


// put function declarations here:
void wakeBms();

Battery battery;
States state;
bool systemCheckOk;
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
  fans.begin();
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
      standByState(state, systemCheckOk);
      break;
    case PRECHARGE:
      preChargeState(state, systemCheckOk);
      break;
    case CHARGE:
      chargeState(state, systemCheckOk);
      break;
    case NORMAL:
      normalState(state, systemCheckOk);
      break;
    case SHUTDOWN:
      shutdownState(state, systemCheckOk);
      break;
    default:
      state = SHUTDOWN;
      Serial.println("Uh oh u dummy, u've entered a non-existent state");
      break;
  }

  delay(500);
  
}
void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}