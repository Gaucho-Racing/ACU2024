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
/// @param[in] cellVoltage float array to store voltages
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