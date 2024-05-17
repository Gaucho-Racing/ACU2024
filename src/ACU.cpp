#include "ACU.h"
#include "adBms_Application.h"
#include "adBms6830CmdList.h"
#include "can.h"

//index i corresponds to the gpio required to get the temperature of the balacing resistor of the ith cell
//

void ACU::ACU_init(){
  this->ACU_ADC.begin();
}

void ACU::updateGlvVoltage(){
  glv_voltage = ACU_ADC.readVoltage(ADC_MUX_GLV_VOLT);
}
void ACU::updateTsVoltage(){
  ts_voltage = ACU_ADC.readVoltage(ADC_MUX_HV_VOLT);
}
void ACU::updateShdnVolt(){
  shdn_volt = ACU_ADC.readVoltage(ADC_MUX_SHDN_POW);
}
void ACU::updateDcdcCurrent(){
  dcdc_current = ACU_ADC.readVoltage(ADC_MUX_DCDC_CURRENT) / HV_Current_Ref;
}
void ACU::updateDcdcTemp1(){
  DCDC_temp[0] = ACU_ADC.readVoltage(ADC_MUX_DCDC_TEMP1);
}
void ACU::updateDcdcTemp2(){
  DCDC_temp[1] = ACU_ADC.readVoltage(ADC_MUX_DCDC_TEMP2);
}
void ACU::updateFanRef(){
  fan_Ref = ACU_ADC.readVoltage(ADC_MUX_FAN_REF);
}

void ACU::updateAccumCurrent(){
  ts_current = ACU_ADC.readVoltage(ADC_MUX_HV_CURRENT) / HV_Current_Ref;
}
void ACU::updateAll(){
  updateGlvVoltage();
  updateTsVoltage();
  updateAccumCurrent();
  updateShdnVolt();
  updateDcdcCurrent();
  updateDcdcTemp1();
  updateDcdcTemp2();
  updateFanRef();
}


uint8_t ACU::getRelayState(){
  return relay_state;
}
float ACU::getGlvVoltage(){
  return glv_voltage;
}
float ACU::getTsVoltage(){
  return ts_voltage;
}
float ACU::getTsCurrent(){
  return ts_current;
}
float ACU::getShdnVolt(){
  return shdn_volt;
}
float ACU::getDcdcCurrent(){
  return dcdc_current;
}
float ACU::getDcdcTemp1(){
  return DCDC_temp[0];
}
float ACU::getDcdcTemp2(){
  return DCDC_temp[1];
}
float ACU::getFanRef(){
  return fan_Ref;
}



//hack to make it work with linking issues
void readCANWrapper(Battery &battery){
  readCANData(battery);
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

/// @brief sum of all cellTemps stored in battery
/// @param[in] battery
/// @return sum for accumulator temp, deg C
uint8_t getAccumulatorTemp(Battery &battery){
  uint16_t temp = 0;
  for(uint8_t index = 0; index < 128; index++)
    temp += (uint16_t)battery.cellTemp[index];
  return temp;

}

/// @brief % charge of battery
/// @param[in] battery
/// @return 0-100% charge
uint8_t calcCharge(Battery &battery){ // calculate state of charge --> TODO
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
