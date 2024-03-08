#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830CmdList.h"


RD      REDUNDANT_MEASUREMENT           = RD_OFF;
CH      AUX_CH_TO_CONVERT               = AUX_ALL;
CONT    CONTINUOUS_MEASUREMENT          = SINGLE;
OW_C_S  CELL_OPEN_WIRE_DETECTION        = OW_OFF_ALL_CH;
OW_AUX  AUX_OPEN_WIRE_DETECTION         = AUX_OW_OFF;
PUP     OPEN_WIRE_CURRENT_SOURCE        = PUP_DOWN;
DCP     DISCHARGE_PERMITTED             = DCP_OFF;
RSTF    RESET_FILTER                    = RSTF_OFF;
ERR     INJECT_ERR_SPI_READ             = WITHOUT_ERR;


/* Set Under Voltage and Over Voltage Thresholds */
const float OV_THRESHOLD = 4.2;                 /* Volt */
const float UV_THRESHOLD = 3.0;                 /* Volt */
const int OWC_Threshold = 2000;                 /* Cell Open wire threshold(mili volt) */
const int OWA_Threshold = 50000;                /* Aux Open wire threshold(mili volt) */
const uint32_t LOOP_MEASUREMENT_COUNT = 1;      /* Loop measurment count */
const uint16_t MEASUREMENT_LOOP_TIME  = 10;     /* milliseconds(mS)*/
uint32_t loop_count = 0;
uint32_t pladc_count;

/*Loop Measurement Setup These Variables are ENABLED or DISABLED Remember ALL CAPS*/
LOOP_MEASURMENT MEASURE_CELL            = ENABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_AVG_CELL        = ENABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_F_CELL          = ENABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_S_VOLTAGE       = ENABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_AUX             = DISABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_RAUX            = DISABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_STAT            = DISABLED;        /*   This is ENABLED or DISABLED       */

/// @brief performs system check
/// @param[in] battery Battery struct
/// @param[in] state Reference to states
/// @return The false if fails, true otherwise
void systemCheck(Battery &battery) {
    //pull data from all 6830's
    adBmsWakeupIc(TOTAL_IC);
    adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
    pladc_count = adBmsPollAdc(PLADC);

    updateVoltage(battery);
}

    // This function is supposed to check if the system is working properly
    // It is not implemented yet
    
/// @brief offState, idk if this is needed
/// @param[in] battery TBD
/// @return N/A
void offState(Battery &battery){
  // When it turns on --> go to STANDBY
  battery.state = STANDBY;
}

/// @brief shutDown, send errors --> VDM
/// @param[in] battery TBD
/// @return N/A
void shutdownState(Battery &battery){
  // Open AIRS and Precharge if already not open

  // send error --> CAN
  byte alertMsg[1];
  alertMsg[0] = 0;
  battery.can.send(0x66, alertMsg, 1);
  battery.state = OFFSTATE;
}

/// @brief timeout checks, system checks, batt data --> VDM
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void normalState(Battery &battery){
  // System Checks
  //if (!systemCheck()) mockState = SHUTDOWN; return;
  
  // if CAN timeout --> Send battery data to VDM
}

/// @brief req charge, system checks
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void chargeState(Battery &battery){
  // sendMsg if time 0.5 s reached
  // do System Check
  // if (!SYSTEMCHECKOK || TIMEOUT) mockState = SHUTDOWN --> return;
  // else --> same state
}

/// @brief error --> VDM if timeout --> (NORMAL/SHUTDOWN)
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void preChargeState(Battery &battery){
  // send message to VDM to indicate Precharge
  // close AIR+, wait 1 second, check voltage
  // 10 x until threshold reached

  // systemChecks
  // if (!SYSTEMCHECKOK) mockState = SHUTDOWN --> return;
  // if (timeout) --> msg --> VDM, mockState = SHUTDOWN --> return;
  // VDM response 1 --> state = NORMAL
  // VDM response 2 --> state = SHUTDOWN
  // else --> ERROR 
}

/// @brief WAKE UP ISOspi Chip/sensors & System Checks
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void standByState(Battery &battery){
      // WAKE UP: ISOSpi Chip & sensors

      // SYSTEM CHECKS
      systemCheck(battery);
      if(battery.state == SHUTDOWN) return;
      // else if (!CHARGERCAN) mockState = PRECHARGE
      // else if (CHARGERCAN) mockState = CHARGE
      // else ERROR
}

/// @brief Reads cell voltages and copy data from cell_asic & checks for errors
/// @param[in] battery contains cellVotage = array to store voltages
/// @return None
void updateVoltage(Battery &battery) {
  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
  adBms6830_read_cell_voltages(TOTAL_IC, battery.IC);
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++) {
    for (uint8_t cell = 0; cell < CELL; cell++) {
      battery.cellVoltage[ic * CELL + cell] = (battery.IC[ic].cell.c_codes[cell] + 10000) * 3 / 2;
      
      // send to error state & send error to CAN if voltage threshold has issues
      if(condenseVoltage(battery.cellVoltage[ic * CELL + cell]) > OV_THRESHOLD)
        sendCellVoltageError(battery, OV_THRESHOLD);
      else if(condenseVoltage(battery.cellVoltage[ic * CELL + cell]) < UV_THRESHOLD)
        sendCellVoltageError(battery, UV_THRESHOLD);
    }
  }
}

/// @brief converts uint16_t voltage --> uint8_t voltage
/// @param[in] voltage uint16_t
/// @return uint8_t voltage converted
uint8_t condenseVoltage(uint16_t voltage) {
  //voltage = constrain(voltage, 20000, 45500);
  return (voltage / 100 + (voltage % 100 > 49));// - 200; // uncomment these when connecting to cells
}

/// @brief Sends data to CANbus
/// @param[in] cellVoltage array to store voltages
/// @param[in] TBD TBD
/// @return None
void dumpCANbus(CANLine *can, uint16_t cellVoltage[]) {
  uint8_t message[8];
  for (uint8_t i = 0; i < 16; i++) {
    uint16_t id = i + 0xA1;
    message[0] = condenseVoltage(cellVoltage[i * 8 + 0]);
    message[1] = condenseVoltage(cellVoltage[i * 8 + 1]);
    message[2] = condenseVoltage(cellVoltage[i * 8 + 2]);
    message[3] = condenseVoltage(cellVoltage[i * 8 + 3]);
    message[4] = condenseVoltage(cellVoltage[i * 8 + 4]);
    message[5] = condenseVoltage(cellVoltage[i * 8 + 5]);
    message[6] = condenseVoltage(cellVoltage[i * 8 + 6]);
    message[7] = condenseVoltage(cellVoltage[i * 8 + 7]);
    can -> send(id, message, 8);
  }
}

/// @brief sends CellVoltageError data to CANbus
/// @param[in] battery
/// @param[in] thresholdType either OV_THRESHOLD or UW_THRESHOLD
/// @return N/A
void sendCellVoltageError(Battery &battery, const float thresholdType){
  uint8_t message[8];

  uint16_t accVolt = getAccumulatorVoltage(battery.cellVoltage);
  message[0] = (uint8_t)((accVolt & 0xFF00) >> 8);
  message[1] = (uint8_t)(accVolt & 0x00FF);
  message[2] = (uint8_t)((battery.accumulatorCurrent & 0xFF00) >> 8);
  message[3] = (uint8_t)(battery.accumulatorCurrent & 0x00FF);
  message[4] = (uint8_t)((battery.maxCellTemp & 0xFF00) >> 8);
  message[5] = (uint8_t)(battery.maxCellTemp & 0x00FF);
  message[7] = 3; // NOT SURE WHAT TO PUT IN HERE

  if(thresholdType == OV_THRESHOLD){ message[6] = 1;}
  else if(thresholdType == UV_THRESHOLD){ message[6] = 4;}

  battery.can.send(0x96, message, 8);
  battery.state = SHUTDOWN; // SEND TO SHUTDOWN
}

/// @brief sum of all voltages stored in battery
/// @param[in] *cellVoltage a pointer to battery's cellVoltages array
/// @return sum for accumulator voltage
uint16_t getAccumulatorVoltage(uint16_t *cellVoltage){
  uint16_t accVoltage = 0;
  for(uint8_t index = 0; index < 128; index++)
    accVoltage += cellVoltage[index] / 100 + (cellVoltage[index] % 100 > 49);
  return accVoltage;
}

uint16_t getAccumulatorCurrent(uint16_t *cellVoltage){
  return 0;
}