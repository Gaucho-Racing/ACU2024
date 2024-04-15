#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830CmdList.h"
#include "can.h"

//index i corresponds to the gpio required to get the temperature of the balacing resistor of the ith cell
//
uint16_t mux_temp_codes[8] = {0b0011111111, 0b0000111111, 0b0001111111, 0b0010111111, 0b0100111111, 0b0110111111, 0b0111111111, 0b0101111111}; 

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
const float OV_THRESHOLD = 42000;                 /* Volt in 0.1 mV*/
const float UV_THRESHOLD = 30000;                 /* Volt in 0.1 mV*/
//Discharge
const float MIN_DIS_TEMP = -40; //TODO: Modify later
const float MAX_DIS_TEMP = 60; 
//Charging
const float MIN_CHR_TEMP = 0; //TODO: Modify later
const float MAX_CHR_TEMP = 60; 
//Balance Resistor Temp
const float MIN_BAL_TEMP = 0; //TODO: Modify later
const float MAX_BAL_TEMP = 60; 

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
bool systemCheck(Battery &battery) {
  // read stuff on ADC1283 chip
  battery.ts_voltage = battery.ACU_ADC.readVoltage(ADC_MUX_HV_VOLT) * 150;
  battery.accumCurrent = (battery.ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) - battery.accumCurrentZero) * 6250;
  // TODO: ACU temperatures, DC/DC current

  // pull data from all 6830's
  adBmsWakeupIc(TOTAL_IC);
  // update Voltage, balTemp, and cellTemp
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);

  pladc_count = adBmsPollAdc(PLADC);
  // every 10 cycles recheck Voltage while in charge
  if(battery.chargeCycle>0 && battery.state == CHARGE){
  } else {
    updateVoltage(battery);
  }
  if (battery.temp_cycle == 0) updateTemps(battery);
  if(battery.maxBalTemp==-1) battery.maxBalTemp = battery.balTemp[0];
  if(battery.maxCellTemp == -1) battery.maxCellTemp = battery.cellTemp[0];
  if(battery.minVolt == -1) battery.minVolt = battery.cellVoltage[0];
  // if(battery.minCellTemp == -1) battery.minCellTemp = battery.cellTemp[0];
  for (int i = 0 ; i < 128; i++){
    if(battery.chargeCycle > 0 && battery.state == CHARGE){
    }else{ //check Voltage:
      if (battery.minVolt > battery.cellVoltage[i]) battery.minVolt = battery.cellVoltage[i];
      if (battery.cellVoltage[i] > OV_THRESHOLD || battery.cellVoltage[i] < UV_THRESHOLD){
        return true;
      }
    }
    if (battery.chargeCycle == 0 && battery.state == CHARGE){
      uint16_t toDischarge = 0;
      //figure out which cells to discharge
      for(int ic = 0; ic < TOTAL_IC; ic++){
        for(int cell = 0; cell < CELL; cell++){
          //diff between the minimum cell voltage and the current cell is 20mV discharge
          if(battery.cellVoltage[ic*CELL + cell]-battery.minVolt > 200){
            toDischarge |= 1 << cell;
          }
        }
        battery.IC[ic].tx_cfgb.dcc = toDischarge;
      }
    }
    
    if (battery.maxBalTemp < battery.balTemp[i]) battery.maxBalTemp = battery.balTemp[i];
    // if (battery.minCellVo > battery.cellTemp[i]) battery.maxBalTemp = battery.balTemp[i];

    //check Bal Temp;
    if (battery.balTemp[i] > MAX_BAL_TEMP || battery.balTemp[i] < MIN_BAL_TEMP){
      return true;
    }
  }
  //check CellTemp:
  for (int i = 0; i < 256; i++){
    if (battery.maxCellTemp < battery.cellTemp[i]) battery.maxCellTemp = battery.cellTemp[i];
    if (battery.state == CHARGE){
      if (battery.cellTemp[i] > MAX_CHR_TEMP || battery.cellTemp[i] < MIN_CHR_TEMP){
        return true;
      }
    }else{
      if (battery.cellTemp[i] > MAX_DIS_TEMP || battery.cellTemp[i] < MIN_DIS_TEMP){
        return true;
      }
    }
  }
  //TODO: maybe discharge top 10% (std)
  //if next chargeCycle is 0 and Charging, get ready for cell measurement by turing off discharge
  if(battery.chargeCycle >= 9 && battery.state == CHARGE){
    battery.chargeCycle = 0;
    for(int ic = 0; ic < TOTAL_IC; ic++){
      battery.IC[ic].tx_cfgb.dcc = 0x0;
    }
  }
  adBms6830_write_read_config(TOTAL_IC, battery.IC);
  return false; 
}

/// @brief standby, send data --> VDM
/// @param[in] battery
/// @return N/A
void standByState(Battery &battery){
  //STUB
  Serial.println("standby...");
}

/// @brief shutDown, send errors --> VDM
/// @param[in] battery
/// @return N/A
void shutdownState(Battery &battery){
  // Open AIRS and Precharge if already not open
  digitalWrite(PRECHG_OUT, LOW);
  digitalWrite(AIR_NEG, LOW);
  digitalWrite(AIR_PLUS, LOW);
  sendCANData(battery, ACU_General2);
  battery.state = OFFSTATE;
}

/// @brief timeout checks, system checks, batt data --> VDM
/// @param[in] battery
/// @param[in] state
/// @return N/A
void normalState(Battery &battery){
  // System Checks
  if (battery.containsError){battery.state = SHUTDOWN; return;}

  // Send batt info to VDM at 100Hz ???
  battery.errs = 0b00000001; // No error
  battery.containsError = false;
}

/// @brief req charge, system checks
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void chargeState(Battery &battery){
  // System Checks
  if (!battery.containsError){
    battery.state = SHUTDOWN;
    return;
  }
  // sendMsg if time 0.5 s reached --> TODO
  sendCANData(battery, Charging_Cart_Config);
  sendCANData(battery, Charger_Control);
}

/// @brief error --> VDM if timeout --> (NORMAL/SHUTDOWN)
/// @param[in] TBD TBD
/// @param[in] TBD TBD
/// @return TBD
void preChargeState(Battery &battery){
  if (!(battery.relay_state & 0b00010000)) { // if discharge relay isn't open
    digitalWrite(DISCHG_STOP, HIGH); // open discharge relay
    delay(10); // wait for the relay to switch
    battery.relay_state |= 0b00010000;
  }
  if (!(battery.relay_state & 0b10000000)) { // if AIR- isn't closed
    digitalWrite(AIR_NEG, HIGH); // clost AIR-
    delay(50); // wait for the relay to switch
    battery.relay_state |= 0b10000000;
  }
  if (!(battery.relay_state & 0b00100000)) { // if precharge relay isn't closed
    digitalWrite(PRECHG_OUT, HIGH); // clost precharge relay
    delay(10); // wait for the relay to switch
    battery.relay_state |= 0b00100000;
  }
  // send message to VDM to indicate Precharge
  sendCANData(battery, ACU_General2);
  // check voltage, if difference > 5V after 2 seconds throw error
  uint32_t startTime = millis();
  while (battery.ts_voltage < getAccumulatorVoltage(battery) - 500) {
    systemCheck(battery);
    if (millis() - startTime > 2000) { // timeout, throw error
      digitalWrite(AIR_PLUS, LOW); // open AIR+, shouldn't be closed but just in case
      digitalWrite(PRECHG_OUT, LOW); // open precharge relay
      digitalWrite(AIR_NEG, LOW); // open AIR-
      digitalWrite(DISCHG_STOP, LOW); // close discharge relay
      battery.relay_state = 0b00000000;
      battery.errs |= 0b00000100; // set precharge error bit
      battery.errs &= 0b11111110; // clear no error bit
      battery.containsError = true;
      battery.state = SHUTDOWN; // enter shutdown state
      dumpCANbus(battery);
    }
    delay(20);
    dumpCANbus(battery);
  }
}

/// @brief offState, idk if this is needed
/// @param[in] battery TBD
/// @return N/A
void offState(Battery &battery){
  // When it turns on --> go to STANDBY
  battery.state = OFFSTATE;
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
  if(battery.temp_cycle >= 15){
    battery.temp_cycle = 0;
  }
  else {
    battery.temp_cycle++;
  }
}

float V2T(float voltage, float B = 4390){
  float R = voltage / ((5.0 - voltage) / 47e3) / 100e3;
  float T = 1.0 / ((log(R) / B) + (1.0 / 298.15));
  return T - 273.15;
}

void updateTemps(Battery &battery){
  for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
    battery.IC[ic].tx_cfga.gpo = mux_temp_codes[battery.cycle];
  }
    adBmsWriteData(TOTAL_IC, battery.IC, WRCFGA, Config, AA);
    adBms6830_start_aux_voltage_measurment(TOTAL_IC, battery.IC);
    adBms6830_read_aux_voltages(TOTAL_IC, battery.IC);
    for (uint8_t ic = 0; ic < TOTAL_IC; ic++){
      //all values are subtracted by one to account for indexing from 0
      //gpio 3: mux1, temp 0
      battery.cellTemp[ic*32 + (7-battery.cycle)] = V2T(battery.IC[ic].aux.a_codes[3]);
      //gpio 4: mux 2, temp 8
      battery.cellTemp[ic*32 + (7-battery.cycle) + 8] = V2T(battery.IC[ic].aux.a_codes[4]);
      //gpio 5: mux 3, bal 0
      battery.balTemp[ic*16 + battery.cycle] = V2T(battery.IC[ic].aux.a_codes[5]);
      //gpio 0: mux 4, bal 0
      battery.balTemp[ic*16 + battery.cycle + 8] = V2T(battery.IC[ic].aux.a_codes[0]);
      //gpio 1: mux 5, temp 16
      battery.cellTemp[ic*32 + (7-battery.cycle) + 16] = V2T(battery.IC[ic].aux.a_codes[1]);
      //gpio 2: mux 6, temp 24
      battery.cellTemp[ic*32 + (7-battery.cycle) + 24] = V2T(battery.IC[ic].aux.a_codes[2]);
    }
  if (battery.cycle >= 7){
    battery.cycle = 0;
  }
  else {
    battery.cycle++;
  }
}


/// @brief converts uint16_t voltage --> uint8_t voltage
/// @param[in] voltage uint16_t
/// @return uint8_t voltage converted
uint8_t condenseVoltage(uint16_t voltage) {
  //voltage = constrain(voltage, 20000, 45500);
  return (voltage / 100 + (voltage % 100 > 49));// - 200; // uncomment these when connecting to cells
}

/// @brief converts float temperature --> uint8_t temperature
/// @param[in] temperature float
/// @return uint8_t temperature converted
uint8_t condenseTemperature(float temperature) {
  return (uint8_t)((temperature - 10) * 4); // 10C~73.75C --> 0~255
}

/// @brief Sends data to CANbus
/// @param[in] battery
/// @return None
void dumpCANbus(Battery &battery) {
  for (uint8_t i = 0; i < 16; i++) {
    battery.msg.buf[0] = condenseVoltage(battery.cellVoltage[i * 8 + 0]);
    battery.msg.buf[1] = condenseVoltage(battery.cellVoltage[i * 8 + 1]);
    battery.msg.buf[2] = condenseVoltage(battery.cellVoltage[i * 8 + 2]);
    battery.msg.buf[3] = condenseVoltage(battery.cellVoltage[i * 8 + 3]);
    battery.msg.buf[4] = condenseVoltage(battery.cellVoltage[i * 8 + 4]);
    battery.msg.buf[5] = condenseVoltage(battery.cellVoltage[i * 8 + 5]);
    battery.msg.buf[6] = condenseVoltage(battery.cellVoltage[i * 8 + 6]);
    battery.msg.buf[7] = condenseVoltage(battery.cellVoltage[i * 8 + 7]);
    battery.msg.id = Condensed_Cell_Voltage_n0 + i;
    battery.msg.flags.extended = true;
    battery.can_prim.write(battery.msg);
  }
  sendCANData(battery, ACU_General);
  sendCANData(battery, ACU_General2);
  sendCANData(battery, Powertrain_Cooling);
  sendCANData(battery, Charging_Cart_Config);
}

/// @brief sends CellVoltageError data to CANbus
/// @param[in] battery
/// @param[in] thresholdType either OV_THRESHOLD or UW_THRESHOLD
/// @return N/A
void sendCellVoltageError(Battery &battery, const float thresholdType){
  uint8_t message[8];

  uint16_t accVolt = getAccumulatorVoltage(battery);
  message[0] = (uint8_t)((accVolt & 0xFF00) >> 8);
  message[1] = (uint8_t)(accVolt & 0x00FF);
  message[2] = (uint8_t)((battery.accumCurrent & 0xFF00) >> 8);
  message[3] = (uint8_t)(battery.accumCurrent & 0x00FF);
  message[4] = (uint8_t)(((uint8_t)(battery.maxCellTemp) & 0xFF00) >> 8);
  message[5] = (uint8_t)((uint8_t)(battery.maxCellTemp) & 0x00FF);
  message[7] = 3; // NOT SURE WHAT TO PUT IN HERE

  if(thresholdType == OV_THRESHOLD){ message[6] = 1;}
  else if(thresholdType == UV_THRESHOLD){ message[6] = 4;}

  // battery.can.send(0x96, message, 8);
  battery.containsError = true; // SEND TO SHUTDOWN
}

/// @brief sum of all voltages stored in battery
/// @param[in] battery
/// @return sum for accumulator voltage, 10mV/LSB
uint16_t getAccumulatorVoltage(Battery &battery){
  uint16_t accVoltage = 0;
  for(uint8_t index = 0; index < 128; index++)
    accVoltage += battery.cellVoltage[index] / 100 + (battery.cellVoltage[index] % 100 > 49);
  return accVoltage;
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
