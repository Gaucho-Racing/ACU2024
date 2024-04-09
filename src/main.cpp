#include "ADBMS.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"
#include "ACU.h"
#include "can.h"
using namespace std;

// put function declarations here:
void wakeBms();
void printPWM(uint8_t tIC, cell_asic *IC);



// ACU variables
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


enum test_case {VOLTAGE, CAN, FAN, GPIO, TEENSY, CELLBAL, EXTRA};
test_case debug = VOLTAGE;

CANLine can;
short message[8] = {60000,4,0,0,0,0,0,0};
std::vector<byte> pong;

cell_asic IC[2];

fanController fans(&Serial8);
bool test_bool[10] = {0,0,0,0,0,0,0,0,0,1};

uint8_t Wrpwm1[2] = { 0x00, 0x20 };
uint8_t Wrpwm2[2] = { 0x00, 0x21 };
uint8_t Wrcfgb[2] = { 0x00, 0x24 };
uint8_t Wrcfga[2] = { 0x00, 0x01 };


void setup() {
  Serial.begin(115200);
  fans.begin();
  Serial.println("Init config");
  adBms6830_init_config(TOTAL_IC, &IC[0]);
  Serial.println("Setup done");
  //isoSPI1.begin();
  //isoSPI1.setIntFunc(intrFunc);
}

void loop() {


}
  

//will work for TotalIC = 1 and only to read PWM A
void printPWM(uint8_t tIC, cell_asic *IC) {
  
  Serial.println("PWM A :");
  for(int i = 0; i < RX_DATA; i++){
    Serial.printf("%u  ", IC[0].pwma.tx_data[i]);
  }
  Serial.println();
}