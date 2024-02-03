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
#include "adbms_main.h"

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

#endif
/** @}*/
/** @}*/