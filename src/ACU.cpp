#include "ACU.h"

/// @brief performs system check
bool systemCheck(Battery &battery, States &state)
{
    return true;   
}
/// @brief shutDown, errors --> VDM
void shutdownState(){
  // Open AIRS and Precharge if already not open
  // error messages --> VDM
}

/// @brief timeout checks, system checks, batt data --> VDM
void normalState(){
  // System Checks
  //if (!systemCheck()) mockState = SHUTDOWN; return;
  
  // if CAN timeout --> Send battery data to VDM
}

/// @brief req charge, system checks
void chargeState(){
  // sendMsg if time 0.5 s reached
  // do System Check
  // if (!SYSTEMCHECKOK || TIMEOUT) mockState = SHUTDOWN --> return;
  // else --> same state
}

/// @brief error --> VDM if timeout --> (NORMAL/SHUTDOWN)
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
void standByState(){
      // WAKE UP: ISOSpi Chip & sensors

      // SYSTEM CHECKS
      // if (!SYSTEMCHECKOK) mockState = SHUTDOWN
      // else if (!CHARGERCAN) mockState = PRECHARGE
      // else if (CHARGERCAN) mockState = CHARGE
      // else ERROR
}