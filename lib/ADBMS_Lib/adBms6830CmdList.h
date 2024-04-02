/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adBms6830CmdList.h
* @brief:   Command header file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! @addtogroup BMS_DRIVER
*  @{
*
*/

/*! @addtogroup BMS_COMMAND BMS COMMAND
*  @{
*
*/

#ifndef __ADBMSCOMMAND_H
#define __ADBMSCOMMAND_H

#include "common.h"

/* configuration registers commands */
extern uint8_t WRCFGA[2];//        = { 0x00, 0x01 };
extern uint8_t WRCFGB[2];//        = { 0x00, 0x24 };
extern uint8_t RDCFGA[2];//        = { 0x00, 0x02 };
extern uint8_t RDCFGB[2];//        = { 0x00, 0x26 };

/* Read cell voltage result registers commands */
extern uint8_t RDCVA[2];//         = { 0x00, 0x04 };
extern uint8_t RDCVB[2];//         = { 0x00, 0x06 };
extern uint8_t RDCVC[2];//         = { 0x00, 0x08 };
extern uint8_t RDCVD[2];//         = { 0x00, 0x0A };
extern uint8_t RDCVE[2];//         = { 0x00, 0x09 };
extern uint8_t RDCVF[2];//         = { 0x00, 0x0B };
extern uint8_t RDCVALL[2];//       = { 0x00, 0x0C };

/* Read average cell voltage result registers commands commands */
extern uint8_t RDACA[2];//         = { 0x00, 0x44 };
extern uint8_t RDACB[2];//         = { 0x00, 0x46 };
extern uint8_t RDACC[2];//         = { 0x00, 0x48 };
extern uint8_t RDACD[2];//         = { 0x00, 0x4A };
extern uint8_t RDACE[2];//         = { 0x00, 0x49 };
extern uint8_t RDACF[2];//         = { 0x00, 0x4B };
extern uint8_t RDACALL[2];//       = { 0x00, 0x4C };

/* Read s voltage result registers commands */
extern uint8_t RDSVA[2];//         = { 0x00, 0x03 };
extern uint8_t RDSVB[2];//         = { 0x00, 0x05 };
extern uint8_t RDSVC[2];//         = { 0x00, 0x07 };
extern uint8_t RDSVD[2];//         = { 0x00, 0x0D };
extern uint8_t RDSVE[2];//         = { 0x00, 0x0E };
extern uint8_t RDSVF[2];//         = { 0x00, 0x0F };
extern uint8_t RDSALL[2];//        = { 0x00, 0x10 };

/* Read c and s results */
extern uint8_t RDCSALL[2];//       = { 0x00, 0x11 };
extern uint8_t RDACSALL[2];//      = { 0x00, 0x51 };

/* Read all AUX and all Status Registers */
extern uint8_t RDASALL[2];//       = { 0x00, 0x35 };

/* Read filtered cell voltage result registers*/
extern uint8_t RDFCA[2];//         = { 0x00, 0x12 };
extern uint8_t RDFCB[2];//         = { 0x00, 0x13 };
extern uint8_t RDFCC[2];//         = { 0x00, 0x14 };
extern uint8_t RDFCD[2];//         = { 0x00, 0x15 };
extern uint8_t RDFCE[2];//         = { 0x00, 0x16 };
extern uint8_t RDFCF[2];//         = { 0x00, 0x17 };
extern uint8_t RDFCALL[2];//       = { 0x00, 0x18 };

/* Read aux results */
extern uint8_t RDAUXA[2];//        = { 0x00, 0x19 };
extern uint8_t RDAUXB[2];//        = { 0x00, 0x1A };
extern uint8_t RDAUXC[2];//        = { 0x00, 0x1B };
extern uint8_t RDAUXD[2];//        = { 0x00, 0x1F };

/* Read redundant aux results */
extern uint8_t RDRAXA[2];//        = { 0x00, 0x1C };
extern uint8_t RDRAXB[2];//        = { 0x00, 0x1D };
extern uint8_t RDRAXC[2];//        = { 0x00, 0x1E };
extern uint8_t RDRAXD[2];//        = { 0x00, 0x25 };

/* Read status registers */
extern uint8_t RDSTATA[2];//       = { 0x00, 0x30 };
extern uint8_t RDSTATB[2];//       = { 0x00, 0x31 };
extern uint8_t RDSTATC[2];//       = { 0x00, 0x32 };
extern uint8_t RDSTATCERR[2];//    = { 0x00, 0x72 };              /* ERR */
extern uint8_t RDSTATD[2];//       = { 0x00, 0x33 };
extern uint8_t RDSTATE[2];//       = { 0x00, 0x34 };

/* Pwm registers commands */
extern uint8_t WRPWM1[2];//        = { 0x00, 0x20 };
extern uint8_t RDPWM1[2];//        = { 0x00, 0x22 };

extern uint8_t WRPWM2[2];//        = { 0x00, 0x21 };
extern uint8_t RDPWM2[2];//        = { 0x00, 0x23 };

/* Clear commands */
extern uint8_t CLRCELL[2];//       = { 0x07, 0x11 };
extern uint8_t CLRAUX [2];//       = { 0x07, 0x12 };
extern uint8_t CLRSPIN[2];//       = { 0x07, 0x16 };
extern uint8_t CLRFLAG[2];//       = { 0x07, 0x17 };
extern uint8_t CLRFC[2];//         = { 0x07, 0x14 };
extern uint8_t CLOVUV[2];//        = { 0x07, 0x15 };

/* Poll adc command */
extern uint8_t PLADC[2];//         = { 0x07, 0x18 };
extern uint8_t PLAUT[2];//         = { 0x07, 0x19 };
extern uint8_t PLCADC[2];//        = { 0x07, 0x1C };
extern uint8_t PLSADC[2];//        = { 0x07, 0x1D };
extern uint8_t PLAUX1[2];//        = { 0x07, 0x1E };
extern uint8_t PLAUX2[2];//        = { 0x07, 0x1F };

/* Diagn command */
extern uint8_t DIAGN[2];//         = {0x07 , 0x15};

/* GPIOs Comm commands */
extern uint8_t WRCOMM[2];//        = { 0x07, 0x21 };
extern uint8_t RDCOMM[2];//        = { 0x07, 0x22 };
extern uint8_t STCOMM[13];//       = { 0x07, 0x23, 0xB9, 0xE4 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

/* Mute and Unmute commands */
extern uint8_t MUTE[2];// 	       = { 0x00, 0x28 };
extern uint8_t UNMUTE[2];//        = { 0x00, 0x29 };

extern uint8_t RSTCC[2];//         = { 0x00, 0x2E };
extern uint8_t SNAP[2];//          = { 0x00, 0x2D };
extern uint8_t UNSNAP[2];//        = { 0x00, 0x2F };
extern uint8_t SRST[2];//          = { 0x00, 0x27 };

/* Read SID command */
extern uint8_t RDSID[2];//         = { 0x00, 0x2C };

#endif /* __BMS_COMMAND_H */
/** @}*/
/** @}*/
