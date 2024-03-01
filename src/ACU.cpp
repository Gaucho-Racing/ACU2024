#include "ACU.h"
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
bool systemCheck(Battery& battery, States& state) {
    //pull data from all 6830's
    adBmsWakeupIc(TOTAL_IC);
    adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
    pladc_count = adBmsPollAdc(PLADC);
    return true;
}

    // This function is supposed to check if the system is working properly
    // It is not implemented yet
    
/// @brief shutDown, errors --> VDM
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD

void shutdownState(){
  // Open AIRS and Precharge if already not open
  // error messages --> VDM
  
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