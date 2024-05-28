/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adbms_Application.c
* @brief:   adbms application test cases
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! \addtogroup APPLICATION
*  @{
*/

/*! @addtogroup Application
*  @{
*/
#include "common.h"
#include "adBms_Application.h"
#include "adBms6830CmdList.h"
#include "adBms6830GenericType.h"
#include "adBms6830ParseCreate.h"
#include "serialPrintResult.h"
#include "ADBMS.h"
// #include "ACU_data.h"

/**
*******************************************************************************
* @brief Setup Variables
* The following variables can be modified to configure the software.
*******************************************************************************
*/




/* ADC Command Configurations */
RD      REDUNDANT_MEASUREMENT = RD_OFF;
CH      AUX_CH_TO_CONVERT = AUX_ALL;
CONT    CONTINUOUS_MEASUREMENT = SINGLE;
OW_C_S  CELL_OPEN_WIRE_DETECTION = OW_OFF_ALL_CH;
OW_AUX  AUX_OPEN_WIRE_DETECTION = AUX_OW_OFF;
PUP     OPEN_WIRE_CURRENT_SOURCE = PUP_DOWN;
DCP     DISCHARGE_PERMITTED = DCP_OFF;
RSTF    RESET_FILTER = RSTF_OFF;
ERR     INJECT_ERR_SPI_READ = WITHOUT_ERR;

/* Set Under Voltage and Over Voltage Thresholds */
/* Set Under Voltage and Over Voltage Thresholds */
const float OV_THRESHOLD = 4.2;                 /* Volt in 0.1 mV*/
const float UV_THRESHOLD = 3;                 /* Volt in 0.1 mV*/
//Discharge
const float MIN_DIS_TEMP = -40; //TODO: Modify later
const float MAX_DIS_TEMP = 60; 
//Charging
const float MIN_CHR_TEMP = 0; //TODO: Modify later
const float MAX_CHR_TEMP = 60; 
//Balance Resistor Temp
const float MIN_BAL_TEMP = -273.15; //TODO: Modify later
const float MAX_BAL_TEMP = 80; 

const int OWC_Threshold = 2000;                 /* Cell Open wire threshold(mili volt) */
const int OWA_Threshold = 50000;                /* Aux Open wire threshold(mili volt) */
const uint32_t LOOP_MEASUREMENT_COUNT = 1;      /* Loop measurment count */
const uint16_t MEASUREMENT_LOOP_TIME  = 10;     /* milliseconds(mS)*/
uint32_t loop_count = 0;
uint32_t pladc_count;

/**
*******************************************************************************
* @brief Set configuration register A. Refer to the data sheet
*        Set configuration register B. Refer to the data sheet
*******************************************************************************
*/
void adBms6830_init_config(uint8_t tIC, cell_asic *ic)
{
  adBmsSpiInit();
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    ic[cic].tx_cfga.refon = PWR_UP;
//    ic[cic].cfga.cth = CVT_8_1mV;
//    ic[cic].cfga.flag_d = ConfigA_Flag(FLAG_D0, FLAG_SET) | ConfigA_Flag(FLAG_D1, FLAG_SET);
//    ic[cic].cfga.gpo = ConfigA_Gpo(GPO2, GPO_SET) | ConfigA_Gpo(GPO10, GPO_SET);
    ic[cic].tx_cfga.gpo = 0b0111111111; /* All GPIO pull down off */ //TODO: TEST
//    ic[cic].cfga.soakon = SOAKON_CLR;
//    ic[cic].cfga.fc = IIR_FPA256;

    /* Init config B */
    ic[cic].tx_cfgb.vov = SetOverVoltageThreshold(OV_THRESHOLD);
    ic[cic].tx_cfgb.vuv = SetUnderVoltageThreshold(UV_THRESHOLD);
    //SetConfigB_DischargeTimeOutValue(tIC, &ic[cic], RANG_0_TO_63_MIN, TIME_1MIN_OR_0_26HR); // seems that this thing makes daisy chain not work???
  }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, AA);
  adBmsWriteData(tIC, &ic[0], WRCFGB, Config, BB);
}

/**
*******************************************************************************
* @brief Write and Read Configuration Register A/B
*******************************************************************************
*/
void adBms6830_write_read_config(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, AA);
  adBmsWriteData(tIC, &ic[0], WRCFGB, Config, BB);
  adBmsReadData(tIC, &ic[0], RDCFGA, Config, AA);
  adBmsReadData(tIC, &ic[0], RDCFGB, Config, BB);
  printWriteConfig(tIC, &ic[0], Config, ALL_GRP);
  printReadConfig(tIC, &ic[0], Config, ALL_GRP);
}

void adbms6830_write_gpio(uint8_t tIC, cell_asic *ic, bool pinConfig[10])
{
  int temp = 0;
  for(int i = 0; i < 10; i++)
  {
    if(pinConfig[i])
    {
      temp |= (1 << i);
    }
    else
    {
      temp &= ~(1 << i);
    }
  }
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    ic[cic].tx_cfga.gpo = temp;
  }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, AA);
  adBmsWriteData(tIC, &ic[0], WRCFGB, Config, BB);
}

/**
*******************************************************************************
* @brief Read Configuration Register A/B
*******************************************************************************
*/
void adBms6830_read_config(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDCFGA, Config, AA);
  adBmsReadData(tIC, &ic[0], RDCFGB, Config, BB);
  printReadConfig(tIC, &ic[0], Config, ALL_GRP);
}

/**
*******************************************************************************
* @brief Start ADC Cell Voltage Measurement
*******************************************************************************
*/
void adBms6830_start_adc_cell_voltage_measurment(uint8_t tIC, int(*func_ptr)() = nullptr)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC, func_ptr);
  //Serial.printf("Cell conversion completed\n");
  //printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Cell Voltages
*******************************************************************************
*/
void adBms6830_read_cell_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDCVA, Cell, AA);
  adBmsReadData(tIC, &ic[0], RDCVB, Cell, BB);
  adBmsReadData(tIC, &ic[0], RDCVC, Cell, CC);
  adBmsReadData(tIC, &ic[0], RDCVD, Cell, D);
  adBmsReadData(tIC, &ic[0], RDCVE, Cell, E);
  adBmsReadData(tIC, &ic[0], RDCVF, Cell, F);

}

/**
*******************************************************************************
* @brief Start ADC S-Voltage Measurement
*******************************************************************************
*/
void adBms6830_start_adc_s_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adsv(CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
  Serial.printf("S-Voltage conversion completed\n");
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read S-Voltages
*******************************************************************************
*/
void adBms6830_read_s_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDSVA, S_volt, AA);
  adBmsReadData(tIC, &ic[0], RDSVB, S_volt, BB);
  adBmsReadData(tIC, &ic[0], RDSVC, S_volt, CC);
  adBmsReadData(tIC, &ic[0], RDSVD, S_volt, D);
  adBmsReadData(tIC, &ic[0], RDSVE, S_volt, E);
  adBmsReadData(tIC, &ic[0], RDSVF, S_volt, F);
  printVoltages(tIC, &ic[0], S_volt);
}

/**
*******************************************************************************
* @brief Start Avarage Cell Voltage Measurement
*******************************************************************************
*/
void adBms6830_start_avgcell_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adcv(RD_ON, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
  Serial.printf("Avg Cell voltage conversion completed\n");
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Avarage Cell Voltages
*******************************************************************************
*/
void adBms6830_read_avgcell_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDACA, AvgCell, AA);
  adBmsReadData(tIC, &ic[0], RDACB, AvgCell, BB);
  adBmsReadData(tIC, &ic[0], RDACC, AvgCell, CC);
  adBmsReadData(tIC, &ic[0], RDACD, AvgCell, D);
  adBmsReadData(tIC, &ic[0], RDACE, AvgCell, E);
  adBmsReadData(tIC, &ic[0], RDACF, AvgCell, F);
  printVoltages(tIC, &ic[0], AvgCell);
}

/**
*******************************************************************************
* @brief Start Filtered Cell Voltages Measurement
*******************************************************************************
*/
void adBms6830_start_fcell_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
  Serial.printf("F Cell voltage conversion completed\n");
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Filtered Cell Voltages
*******************************************************************************
*/
void adBms6830_read_fcell_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDFCA, F_volt, AA);
  adBmsReadData(tIC, &ic[0], RDFCB, F_volt, BB);
  adBmsReadData(tIC, &ic[0], RDFCC, F_volt, CC);
  adBmsReadData(tIC, &ic[0], RDFCD, F_volt, D);
  adBmsReadData(tIC, &ic[0], RDFCE, F_volt, E);
  adBmsReadData(tIC, &ic[0], RDFCF, F_volt, F);
  printVoltages(tIC, &ic[0], F_volt);
}

/**
*******************************************************************************
* @brief Start AUX, VMV, V+ Voltages Measurement
*******************************************************************************
*/
void adBms6830_start_aux_voltage_measurment(uint8_t tIC, cell_asic *ic, int(*func_ptr)() = nullptr)
{
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    ic[cic].tx_cfga.refon = PWR_UP;
  }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, AA);
  
  adBms6830_Adax(AUX_OPEN_WIRE_DETECTION, OPEN_WIRE_CURRENT_SOURCE, AUX_CH_TO_CONVERT);
  pladc_count = adBmsPollAdc(PLADC, func_ptr);
  // Serial.printf("Aux voltage conversion completed\n");
  // printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read AUX, VMV, V+ Voltages
*******************************************************************************
*/
void adBms6830_read_aux_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDAUXA, Aux, AA);
  adBmsReadData(tIC, &ic[0], RDAUXB, Aux, BB);
  adBmsReadData(tIC, &ic[0], RDAUXC, Aux, CC);
  adBmsReadData(tIC, &ic[0], RDAUXD, Aux, D);
}

/**
*******************************************************************************
* @brief Start Redundant GPIO Voltages Measurement
*******************************************************************************
*/
void adBms6830_start_raux_voltage_measurment(uint8_t tIC,  cell_asic *ic)
{
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    ic[cic].tx_cfga.refon = PWR_UP;
    ic[cic].tx_cfga.gpo = 0X3FF; /* All GPIO pull down off */
  }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, AA);
  adBms6830_Adax2(AUX_CH_TO_CONVERT);
  pladc_count = adBmsPollAdc(PLADC);
  Serial.printf("RAux voltage conversion completed\n");
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Redundant GPIO Voltages
*******************************************************************************
*/
void adBms6830_read_raux_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDRAXA, RAux, AA);
  adBmsReadData(tIC, &ic[0], RDRAXB, RAux, BB);
  adBmsReadData(tIC, &ic[0], RDRAXC, RAux, CC);
  adBmsReadData(tIC, &ic[0], RDRAXD, RAux, D);
  printVoltages(tIC, &ic[0], RAux);
}

/**
*******************************************************************************
* @brief Read Status Reg. A, B, C, D and E.
*******************************************************************************
*/
void adBms6830_read_status_registers(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, AA);
  adBmsWriteData(tIC, &ic[0], WRCFGB, Config, BB);
  adBms6830_Adax(AUX_OPEN_WIRE_DETECTION, OPEN_WIRE_CURRENT_SOURCE, AUX_CH_TO_CONVERT);
  pladc_count = adBmsPollAdc(PLADC);
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = pladc_count + adBmsPollAdc(PLADC);

  adBmsReadData(tIC, &ic[0], RDSTATA, Status, AA);
  adBmsReadData(tIC, &ic[0], RDSTATB, Status, BB);
  adBmsReadData(tIC, &ic[0], RDSTATC, Status, CC);
  adBmsReadData(tIC, &ic[0], RDSTATD, Status, D);
  adBmsReadData(tIC, &ic[0], RDSTATE, Status, E);
  printPollAdcConvTime(pladc_count);
  printStatus(tIC, &ic[0], Status, ALL_GRP);
}

/**
*******************************************************************************
* @brief Clear Cell measurement reg.
*******************************************************************************
*/
void adBms6830_clear_cell_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRCELL);
  Serial.printf("Cell Registers Cleared\n\n");
}

/**
*******************************************************************************
* @brief Clear Aux measurement reg.
*******************************************************************************
*/
void adBms6830_clear_aux_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRAUX);
  Serial.printf("Aux Registers Cleared\n\n");
}

/**
*******************************************************************************
* @brief Clear spin measurement reg.
*******************************************************************************
*/
void adBms6830_clear_spin_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRSPIN);
  Serial.printf("Spin Registers Cleared\n\n");
}

/**
*******************************************************************************
* @brief Clear fcell measurement reg.
*******************************************************************************
*/
void adBms6830_clear_fcell_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRFC);
  Serial.printf("Fcell Registers Cleared\n\n");
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

/** @}*/
/** @}*/