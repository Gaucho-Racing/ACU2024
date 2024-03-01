#include "ACU.h"
#include "adBms_Application.h"

/// @brief performs system check
/// @param[in] battery Battery struct
/// @param[in] state Reference to states
/// @return The false if fails, true otherwise
bool systemCheck(Battery &battery, States &state)
{
    //STUB
    return true;   
}
/// @brief shutDown, errors --> VDM
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void shutdownState(){
  // Open AIRS and Precharge if already not open
  // error messages --> VDM
  // STUB
}

/// @brief timeout checks, system checks, batt data --> VDM
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void normalState(){
  // System Checks
  //if (!systemCheck()) mockState = SHUTDOWN; return;
  
  // if CAN timeout --> Send battery data to VDM
}

/// @brief req charge, system checks
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void chargeState(){
  // sendMsg if time 0.5 s reached
  // do System Check
  // if (!SYSTEMCHECKOK || TIMEOUT) mockState = SHUTDOWN --> return;
  // else --> same state
}

/// @brief error --> VDM if timeout --> (NORMAL/SHUTDOWN)
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void preChargeState(){
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
void standByState(){
      // WAKE UP: ISOSpi Chip & sensors

      // SYSTEM CHECKS
      // if (!SYSTEMCHECKOK) mockState = SHUTDOWN
      // else if (!CHARGERCAN) mockState = PRECHARGE
      // else if (CHARGERCAN) mockState = CHARGE
      // else ERROR
}

/// @brief Reads cell voltages and copy data from cell_asic
/// @param[in] cellVoltage array to store voltages
/// @param[in] TBD TBD
/// @return None
void updateVoltage(uint16_t cellVoltage[], cell_asic IC[]) {
  adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
  adBms6830_read_cell_voltages(TOTAL_IC, IC);
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++) {
    for (uint8_t cell = 0; cell < CELL; cell++) {
      cellVoltage[ic * CELL + cell] = (IC[ic].cell.c_codes[cell] + 10000) * 3 / 2;
    }
  }
}


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