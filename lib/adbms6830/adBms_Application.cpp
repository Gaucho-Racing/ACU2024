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
extern RD      REDUNDANT_MEASUREMENT;
extern CH      AUX_CH_TO_CONVERT;
extern CONT    CONTINUOUS_MEASUREMENT;
extern OW_C_S  CELL_OPEN_WIRE_DETECTION;
extern OW_AUX  AUX_OPEN_WIRE_DETECTION;
extern PUP     OPEN_WIRE_CURRENT_SOURCE;
extern DCP     DISCHARGE_PERMITTED;
extern RSTF    RESET_FILTER;
extern ERR     INJECT_ERR_SPI_READ;

/* Set Under Voltage and Over Voltage Thresholds */
extern const float OV_THRESHOLD;                 /* Volt */
extern const float UV_THRESHOLD;                 /* Volt */
extern const int OWC_Threshold;                 /* Cell Open wire threshold(mili volt) */
extern const int OWA_Threshold;                /* Aux Open wire threshold(mili volt) */
extern const uint32_t LOOP_MEASUREMENT_COUNT;      /* Loop measurment count */
extern const uint16_t MEASUREMENT_LOOP_TIME;     /* milliseconds(mS)*/
extern uint32_t loop_count;
extern uint32_t pladc_count;

/*Loop Measurement Setup These Variables are ENABLED or DISABLED Remember ALL CAPS*/
extern LOOP_MEASURMENT MEASURE_CELL;        /*   This is ENABLED or DISABLED       */
extern LOOP_MEASURMENT MEASURE_AVG_CELL;        /*   This is ENABLED or DISABLED       */
extern LOOP_MEASURMENT MEASURE_F_CELL;        /*   This is ENABLED or DISABLED       */
extern LOOP_MEASURMENT MEASURE_S_VOLTAGE;        /*   This is ENABLED or DISABLED       */
extern LOOP_MEASURMENT MEASURE_AUX;        /*   This is ENABLED or DISABLED       */
extern LOOP_MEASURMENT MEASURE_RAUX;        /*   This is ENABLED or DISABLED       */
extern LOOP_MEASURMENT MEASURE_STAT;        /*   This is ENABLED or DISABLED       */

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
    ic[cic].tx_cfga.gpo = 0X3FF; /* All GPIO pull down off */
//    ic[cic].cfga.soakon = SOAKON_CLR;
//    ic[cic].cfga.fc = IIR_FPA256;

    /* Init config B */
    ic[cic].tx_cfgb.dtmen = DTMEN_ON;
    ic[cic].tx_cfgb.vov = SetOverVoltageThreshold(OV_THRESHOLD);
    ic[cic].tx_cfgb.vuv = SetUnderVoltageThreshold(UV_THRESHOLD);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC1, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC2, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC3, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC4, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC5, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC6, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC7, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC8, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC9, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC10, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC11, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC12, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC13, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC14, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC15, DCC_BIT_SET);
    ic[cic].tx_cfgb.dcc = ConfigB_DccBit(DCC16, DCC_BIT_SET);
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
void adBms6830_start_adc_cell_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
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
void adBms6830_start_aux_voltage_measurment(uint8_t tIC, cell_asic *ic)
{
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    ic[cic].tx_cfga.refon = PWR_UP;
    ic[cic].tx_cfga.gpo = 0X001; /* All GPIO pull down off */
  }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, AA);
  adBms6830_Adax(AUX_OPEN_WIRE_DETECTION, OPEN_WIRE_CURRENT_SOURCE, AUX_CH_TO_CONVERT);
  pladc_count = adBmsPollAdc(PLADC);

  Serial.printf("Aux voltage conversion completed\n");
  printPollAdcConvTime(pladc_count);
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
  printVoltages(tIC, &ic[0], Aux);
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

/** @}*/
/** @}*/