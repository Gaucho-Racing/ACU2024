#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830CmdList.h"

uint16_t cell_to_mux[8] = {0b0011100001, 0b0000100001, 0b0001100001, 0b0010100001, 0b0100100001, 0b0110100001, 0b0111100001, 0b0101100001};

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
bool systemCheck(Battery &battery, States &state) {
    //pull data from all 6830's
    adBmsWakeupIc(TOTAL_IC);
    //update Voltage, balTemp, and cellTemp
    adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);

    pladc_count = adBmsPollAdc(PLADC);

    updateVoltage(battery);
    return true; // TO MODIFY LATER
}

    // This function is supposed to check if the system is working properly
    // It is not implemented yet
    
/// @brief offState, idk if this is needed
/// @param[in] battery TBD
/// @return N/A
void offState(Battery &battery,States& state, bool systemCheckOk){
  // When it turns on --> go to STANDBY
  battery.state = STANDBY;
}

/// @brief shutDown, send errors --> VDM
/// @param[in] battery TBD
/// @return N/A
void shutdownState(Battery &battery, States& state, bool systemCheckOk){
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
void normalState(Battery &battery, States& state, bool systemCheckOk){
  // System Checks
  //if (!systemCheck()) mockState = SHUTDOWN; return;
  
  // if CAN timeout --> Send battery data to VDM
}

/// @brief req charge, system checks
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void chargeState(Battery &battery, States& state, bool systemCheckOk){
  // sendMsg if time 0.5 s reached
  // do System Check
  // if (!SYSTEMCHECKOK || TIMEOUT) mockState = SHUTDOWN --> return;
  // else --> same state
}

/// @brief error --> VDM if timeout --> (NORMAL/SHUTDOWN)
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void preChargeState(Battery &battery, States& state, bool systemCheckOk){
  // send message to VDM to indicate Precharge
  // close AIR+, wait, check voltage
  // 10 x until threshold reached

  // systemChecks
  if (battery.containsError) battery.state = SHUTDOWN; return;
  
  // Send VDM Precharge --> TS Active (1)
  byte message[1];
  message[0] = 1;
  battery.can.send(0x66, message, 1);
  uint32_t timeout = millis(); // not sure how long we should wait until timeout

  // close AIR+
  // if (timeout) --> msg --> VDM, mockState = SHUTDOWN --> return;
  // VDM response 1 --> state = NORMAL
  // VDM response 2 --> state = SHUTDOWN
  // else --> ERROR 
}

/// @brief Runs system checks and reads can to determine whether to charge or startup car
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void standByState(Battery &battery, States& state, bool systemCheckOk){
      // WAKE UP: ISOSpi Chip & sensors
      // SYSTEM CHECKS
      if (!systemCheckOk) state = SHUTDOWN;
      else {
        // read CAN
        // if message is from Charger, set state to CHARGE
        // else if message is from VDM, set state to NORMAL
      }
}
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
  battery.containsError = true; // SEND TO SHUTDOWN
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

/* configuration registers commands */
uint8_t WRCFGA[2]        = { 0x00, 0x01 };
uint8_t WRCFGB[2]        = { 0x00, 0x24 };
uint8_t RDCFGA[2]        = { 0x00, 0x02 };
uint8_t RDCFGB[2]        = { 0x00, 0x26 };

/* Read cell voltage result registers commands */
uint8_t RDCVA[2]         = { 0x00, 0x04 };
uint8_t RDCVB[2]         = { 0x00, 0x06 };
uint8_t RDCVC[2]         = { 0x00, 0x08 };
uint8_t RDCVD[2]         = { 0x00, 0x0A };
uint8_t RDCVE[2]         = { 0x00, 0x09 };
uint8_t RDCVF[2]         = { 0x00, 0x0B };
uint8_t RDCVALL[2]       = { 0x00, 0x0C };

/* Read average cell voltage result registers commands commands */
uint8_t RDACA[2]         = { 0x00, 0x44 };
uint8_t RDACB[2]         = { 0x00, 0x46 };
uint8_t RDACC[2]         = { 0x00, 0x48 };
uint8_t RDACD[2]         = { 0x00, 0x4A };
uint8_t RDACE[2]         = { 0x00, 0x49 };
uint8_t RDACF[2]         = { 0x00, 0x4B };
uint8_t RDACALL[2]       = { 0x00, 0x4C };

/* Read s voltage result registers commands */
uint8_t RDSVA[2]         = { 0x00, 0x03 };
uint8_t RDSVB[2]         = { 0x00, 0x05 };
uint8_t RDSVC[2]         = { 0x00, 0x07 };
uint8_t RDSVD[2]         = { 0x00, 0x0D };
uint8_t RDSVE[2]         = { 0x00, 0x0E };
uint8_t RDSVF[2]         = { 0x00, 0x0F };
uint8_t RDSALL[2]        = { 0x00, 0x10 };

/* Read c and s results */
uint8_t RDCSALL[2]       = { 0x00, 0x11 };
uint8_t RDACSALL[2]      = { 0x00, 0x51 };

/* Read all AUX and all Status Registers */
uint8_t RDASALL[2]       = { 0x00, 0x35 };

/* Read filtered cell voltage result registers*/
uint8_t RDFCA[2]         = { 0x00, 0x12 };
uint8_t RDFCB[2]         = { 0x00, 0x13 };
uint8_t RDFCC[2]         = { 0x00, 0x14 };
uint8_t RDFCD[2]         = { 0x00, 0x15 };
uint8_t RDFCE[2]         = { 0x00, 0x16 };
uint8_t RDFCF[2]         = { 0x00, 0x17 };
uint8_t RDFCALL[2]       = { 0x00, 0x18 };

/* Read aux results */
uint8_t RDAUXA[2]        = { 0x00, 0x19 };
uint8_t RDAUXB[2]        = { 0x00, 0x1A };
uint8_t RDAUXC[2]        = { 0x00, 0x1B };
uint8_t RDAUXD[2]        = { 0x00, 0x1F };

/* Read redundant aux results */
uint8_t RDRAXA[2]        = { 0x00, 0x1C };
uint8_t RDRAXB[2]        = { 0x00, 0x1D };
uint8_t RDRAXC[2]        = { 0x00, 0x1E };
uint8_t RDRAXD[2]        = { 0x00, 0x25 };

/* Read status registers */
uint8_t RDSTATA[2]       = { 0x00, 0x30 };
uint8_t RDSTATB[2]       = { 0x00, 0x31 };
uint8_t RDSTATC[2]       = { 0x00, 0x32 };
uint8_t RDSTATCERR[2]    = { 0x00, 0x72 };              /* ERR */
uint8_t RDSTATD[2]       = { 0x00, 0x33 };
uint8_t RDSTATE[2]       = { 0x00, 0x34 };

/* Pwm registers commands */
uint8_t WRPWM1[2]        = { 0x00, 0x20 };
uint8_t RDPWM1[2]        = { 0x00, 0x22 };

uint8_t WRPWM2[2]        = { 0x00, 0x21 };
uint8_t RDPWM2[2]        = { 0x00, 0x23 };

/* Clear commands */
uint8_t CLRCELL[2]       = { 0x07, 0x11 };
uint8_t CLRAUX [2]       = { 0x07, 0x12 };
uint8_t CLRSPIN[2]       = { 0x07, 0x16 };
uint8_t CLRFLAG[2]       = { 0x07, 0x17 };
uint8_t CLRFC[2]         = { 0x07, 0x14 };
uint8_t CLOVUV[2]        = { 0x07, 0x15 };

/* Poll adc command */
uint8_t PLADC[2]         = { 0x07, 0x18 };
uint8_t PLAUT[2]         = { 0x07, 0x19 };
uint8_t PLCADC[2]        = { 0x07, 0x1C };
uint8_t PLSADC[2]        = { 0x07, 0x1D };
uint8_t PLAUX1[2]        = { 0x07, 0x1E };
uint8_t PLAUX2[2]        = { 0x07, 0x1F };

/* Diagn command */
uint8_t DIAGN[2]         = {0x07 , 0x15};

/* GPIOs Comm commands */
uint8_t WRCOMM[2]        = { 0x07, 0x21 };
uint8_t RDCOMM[2]        = { 0x07, 0x22 };
uint8_t STCOMM[13]       = { 0x07, 0x23, 0xB9, 0xE4 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

/* Mute and Unmute commands */
uint8_t MUTE[2] 	       = { 0x00, 0x28 };
uint8_t UNMUTE[2]        = { 0x00, 0x29 };

uint8_t RSTCC[2]         = { 0x00, 0x2E };
uint8_t SNAP[2]          = { 0x00, 0x2D };
uint8_t UNSNAP[2]        = { 0x00, 0x2F };
uint8_t SRST[2]          = { 0x00, 0x27 };

/* Read SID command */
uint8_t RDSID[2]         = { 0x00, 0x2C };
