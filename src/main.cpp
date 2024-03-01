
#include "ADBMS.h"
#include "FanController.h"
#include "ACU.h"


// put function declarations here:
void wakeBms();

Battery battery;
States state;
fanController fans(&Serial8);

float cellVoltage[128];
float cellTemp[128][2];
float balTemp[128];
float maxCellTemp, maxBalTemp;

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
void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}