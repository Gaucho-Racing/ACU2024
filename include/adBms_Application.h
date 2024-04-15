/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adBms_Application.h
* @brief:   Bms application header file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! @addtogroup APPLICATION
*  @{
*
*/

/*! @addtogroup APPLICATION
*  @{
*
*/

#ifndef __APPLICATION_H
#define __APPLICATION_H

#include <stdint.h>
#include "ACU_data.h"
#include "adBms6830Data.h"
#include "adBms6830GenericType.h"
#include "adBms6830ParseCreate.h"
#include "adBms6830CmdList.h"

void adbms_main(void);
void run_command(int cmd);
void adBms6830_init_config(uint8_t tIC, cell_asic *ic);
void adBms6830_write_read_config(uint8_t tIC, cell_asic *ic);
void adBms6830_read_config(uint8_t tIC, cell_asic *ic);
void adBms6830_start_adc_cell_voltage_measurment(uint8_t tIC);
void adBms6830_read_cell_voltages(uint8_t tIC, cell_asic *ic);
void adBms6830_start_adc_s_voltage_measurment(uint8_t tIC);
void adBms6830_read_s_voltages(uint8_t tIC, cell_asic *ic);
void adBms6830_start_avgcell_voltage_measurment(uint8_t tIC);
void adBms6830_read_avgcell_voltages(uint8_t tIC, cell_asic *ic);
void adBms6830_start_fcell_voltage_measurment(uint8_t tIC);
void adBms6830_read_fcell_voltages(uint8_t tIC, cell_asic *ic);
void adBms6830_start_aux_voltage_measurment(uint8_t tIC, cell_asic *ic);
void adBms6830_read_aux_voltages(uint8_t tIC, cell_asic *ic);
void adBms6830_start_raux_voltage_measurment(uint8_t tIC, cell_asic *ic);
void adBms6830_read_raux_voltages(uint8_t tIC, cell_asic *ic);
void adBms6830_read_status_registers(uint8_t tIC, cell_asic *ic);
void measurement_loop(void);
void adbms6830_write_gpio(uint8_t tIC, cell_asic *ic, bool *pinConfig);
void adBms6830_clear_cell_measurement(uint8_t tIC);
void adBms6830_clear_aux_measurement(uint8_t tIC);
void adBms6830_clear_spin_measurement(uint8_t tIC);
void adBms6830_clear_fcell_measurement(uint8_t tIC);

extern cell_asic IC[TOTAL_IC];

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
//from the count indices from the left

#endif
/** @}*/
/** @}*/