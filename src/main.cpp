
#include "ADBMS6822_Driver.h"
#include "ADBMS.h"
#include "ACU.h"
#include "adBms_Application.h"
#include "serialPrintResult.h"
#include "FanController.h"
#include "ACU_data.h"
#include "can.cpp"

// put function declarations here:
void wakeBms();

// Object declarations 
//isoSPI isoSPI1(&SPI, 10, 8, 7, 9, 5, 6, 4, 3, 2);
//isoSPI isoSPI2(&SPI1, 0, 25, 24, 33, 29, 28, 30, 31, 32);

CANLine can;
cell_asic IC[TOTAL_IC];
fanController fans(&Serial8);
States state;
bool systemCheckOK = false;
bool test_bool[10] = {0,0,0,0,0,0,0,0,0,1};

//there are 3 segments of 8 IC's
std::vector<std::vector<float>> cellVoltage(TOTAL_IC, std::vector<float>(16, 0));
//24 IC's with measuring 16 sets of cells each, each cell has a 2 temperatures 
std::vector<std::vector<std::pair<float,float>>> cellTemp(TOTAL_IC,std::vector<std::pair<float,float>> (16,std::make_pair(0,0)));
//HV current
float current = 0;
bool openWireTestResult = false;

void setup() {
  Serial.begin(115200);
  // fans.begin();
  adBms6830_init_config(TOTAL_IC, &IC[0]);
  adBms6830_write_read_config(TOTAL_IC, &IC[1]);
  state = FIRST;
  
}
std::vector<byte> pong;
cell_asic test;
void loop() {
  switch (state)
  {
  case FIRST:
  {
    // systemCheck(state, systemCheckOK);
    short message[8] = {60000,4,0,0,0,0,0,0};
    can.send(97, message);
    can.recieve_one();
    std::vector<byte> pong = can.recieve(97);
    break;
  }
  case PRECHARGE:
    /* code */
    break;
  case NORMAL:
    delay(1000);
    adBmsWakeupIc(1);
    run_command(3);
    run_command(4);
    adbms6830_write_gpio(TOTAL_IC, &IC[0], test_bool);
    adBms6830_read_aux_voltages(TOTAL_IC, &IC[0]);
    /* code */
    break;
  case CHARGE:
    /* code */
    break;
  case SHUTDOWN: 
    /* code */
    break;
  default:
  //should never be here
    break;
  }
  
}

void wakeBms() {
  // Pull CS low for more than 240nS
  digitalWrite(10, LOW);
  delayMicroseconds(1);
  digitalWrite(10, HIGH);
  // Wait 10us for the chip to wake up
  delayMicroseconds(10);
}

