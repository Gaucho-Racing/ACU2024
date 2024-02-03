/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adbms_main.h
* @brief:   adbms main Heade file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/** @addtogroup MAIN
*  @{
*
*/

/** @addtogroup ADBMS_MAIN MAIN
*  @{
*
*/
#ifndef _ADBMS6830_MAIN_H
#define _ADBMS6830_MAIN_H

//#include "main.h"
#include "common.h"



enum States {FIRST, PRECHARGE, NORMAL, CHARGE, SHUTDOWN};

bool systemCheck();

void updateVoltage();

void updateTemperature();

void updateCurrent(float &current);


#endif
/** @}*/
/** @}*/
