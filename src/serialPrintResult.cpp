/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    serialPrintResult.c
* @brief:   Print IO terminal functions
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! \addtogroup PRINT RESULT
*  @{
*/

/*! @addtogroup RESULT PRINT
*  @{
*/
#include "common.h"
#include "serialPrintResult.h"


/**
 *******************************************************************************
 * Function: printWriteConfig
 * @brief Print write config A/B result.
 *
 * @details This function Print write config result into terminal.
 *
 * Parameters:
 * @param [in]	tIC      Total IC
 *
 * @param [in]  *IC      cell_asic stucture pointer
 *
 * @param [in]  type     Enum type of resistor
 *
 * @param [in]  grp      Enum type of resistor group
 *
 * @return None
 *
 *******************************************************************************
*/
void printWriteConfig(uint8_t tIC, cell_asic *IC, TYPE type, GRP grp)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(type == Config)
    {
      if(grp == AA)
      {
        Serial.printf("Write Config A:\n");
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[0]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[1]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[2]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[3]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[4]);
        Serial.printf("0x%X\n\n", IC[ic].configa.tx_data[5]);
      }
      else if(grp == BB)
      {
        Serial.printf("Write Config B:\n");
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[0]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[1]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[2]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[3]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[4]);
        Serial.printf("0x%X\n\n", IC[ic].configb.tx_data[5]);
      }
      else if(grp == ALL_GRP)
      {
        Serial.printf("Write Config A:\n");
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[0]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[1]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[2]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[3]);
        Serial.printf("0x%X, ", IC[ic].configa.tx_data[4]);
        Serial.printf("0x%X\n\n", IC[ic].configa.tx_data[5]);

        Serial.printf("Write Config B:\n");
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[0]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[1]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[2]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[3]);
        Serial.printf("0x%X, ", IC[ic].configb.tx_data[4]);
        Serial.printf("0x%X\n\n", IC[ic].configb.tx_data[5]);
      }
      else{ Serial.printf("Wrong Register Group Select\n"); }
    }
  }
}

/**
 *******************************************************************************
 * Function: printReadConfig
 * @brief Print read config result.
 *
 * @details This function Print read config result into terminal.
 *
 * Parameters:
 * @param [in]	tIC      Total IC
 *
 * @param [in]  *IC      cell_asic stucture pointer
 *
 * @param [in]  type     Enum type of resistor
 *
 * @param [in]  grp      Enum type of resistor group
 *
 * @return None
 *
 *******************************************************************************
*/
void printReadConfig(uint8_t tIC, cell_asic *IC, TYPE type, GRP grp)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(type == Config)
    {
      if(grp == AA)
      {
        Serial.printf("Read Config A:\n");
        Serial.printf("REFON:0x%X, ", IC[ic].rx_cfga.refon);
        Serial.printf("CTH:0x%X\n", IC[ic].rx_cfga.cth & 0x07);
        Serial.printf("FLAG_D[0]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x01));
        Serial.printf("FLAG_D[1]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x02)>>1);
        Serial.printf("FLAG_D[2]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x04)>>2);
        Serial.printf("FLAG_D[3]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x08)>>3);
        Serial.printf("FLAG_D[4]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x10)>>4);
        Serial.printf("FLAG_D[5]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x20)>>5);
        Serial.printf("FLAG_D[6]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x40)>>6);
        Serial.printf("FLAG_D[7]:0x%X\n", (IC[ic].rx_cfga.flag_d & 0x80)>>7);
        Serial.printf("OWA[2:0]:0x%X, ", (IC[ic].rx_cfga.owa));
        Serial.printf("OWRNG:0x%X, ", (IC[ic].rx_cfga.owrng));
        Serial.printf("SOAKON:0x%X, ", (IC[ic].rx_cfga.soakon));
        Serial.printf("GPO:0x%X, ", (IC[ic].rx_cfga.gpo));
        Serial.printf("FC:0x%X, ", (IC[ic].rx_cfga.fc));
        Serial.printf("COMM_BK:0x%X, ", (IC[ic].rx_cfga.comm_bk));
        Serial.printf("MUTE_ST:0x%X, ", (IC[ic].rx_cfga.mute_st));
        Serial.printf("SNAP:0x%X\n\n", (IC[ic].rx_cfga.snap));
        Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n",IC[ic].cccrc.cfgr_pec);
      }
      else if(grp == BB)
      {
        Serial.printf("Read Config B:\n");
        Serial.printf("VUV:0x%X, ", IC[ic].rx_cfgb.vuv);
        Serial.printf("VOV:0x%X, ", IC[ic].rx_cfgb.vov);
        Serial.printf("DCTO:0x%X, ", IC[ic].rx_cfgb.dcto);
        Serial.printf("DTRNG:0x%X, ", IC[ic].rx_cfgb.dtrng);
        Serial.printf("DTMEN:0x%X, ", IC[ic].rx_cfgb.dtmen);
        Serial.printf("DCC:0x%X\n\n", IC[ic].rx_cfgb.dcc);
        Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n",IC[ic].cccrc.cfgr_pec);
      }
      else if(grp == ALL_GRP)
      {
        Serial.printf("Read Config A:\n");
        Serial.printf("REFON:0x%X, ", IC[ic].rx_cfga.refon);
        Serial.printf("CTH:0x%X\n", IC[ic].rx_cfga.cth & 0x07);
        Serial.printf("FLAG_D[0]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x01));
        Serial.printf("FLAG_D[1]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x02)>>1);
        Serial.printf("FLAG_D[2]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x04)>>2);
        Serial.printf("FLAG_D[3]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x08)>>3);
        Serial.printf("FLAG_D[4]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x10)>>4);
        Serial.printf("FLAG_D[5]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x20)>>5);
        Serial.printf("FLAG_D[6]:0x%X, ", (IC[ic].rx_cfga.flag_d & 0x40)>>6);
        Serial.printf("FLAG_D[7]:0x%X\n", (IC[ic].rx_cfga.flag_d & 0x80)>>7);
        Serial.printf("OWA[2:0]:0x%X, ", (IC[ic].rx_cfga.owa));
        Serial.printf("OWRNG:0x%X, ", (IC[ic].rx_cfga.owrng));
        Serial.printf("SOAKON:0x%X, ", (IC[ic].rx_cfga.soakon));
        Serial.printf("GPO:0x%X, ", (IC[ic].rx_cfga.gpo));
        Serial.printf("FC:0x%X, ", (IC[ic].rx_cfga.fc));
        Serial.printf("COMM_BK:0x%X, ", (IC[ic].rx_cfga.comm_bk));
        Serial.printf("MUTE_ST:0x%X, ", (IC[ic].rx_cfga.mute_st));
        Serial.printf("SNAP:0x%X\n\n", (IC[ic].rx_cfga.snap));

        Serial.printf("Read Config B:\n");
        Serial.printf("VUV:0x%X, ", IC[ic].rx_cfgb.vuv);
        Serial.printf("VOV:0x%X, ", IC[ic].rx_cfgb.vov);
        Serial.printf("DCTO:0x%X, ", IC[ic].rx_cfgb.dcto);
        Serial.printf("DTRNG:0x%X, ", IC[ic].rx_cfgb.dtrng);
        Serial.printf("DTMEN:0x%X, ", IC[ic].rx_cfgb.dtmen);
        Serial.printf("DCC:0x%X\n\n", IC[ic].rx_cfgb.dcc);
        Serial.printf("CCount:%d,", IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n", IC[ic].cccrc.cfgr_pec);
      }
      else{ Serial.printf("Wrong Register Group Select\n"); }
    }
  }
}

/**
 *******************************************************************************
 * Function: printVoltages
 * @brief Print Voltages.
 *
 * @details This function Print Voltages into IAR I/O terminal.
 *
 * Parameters:
 * @param [in]	tIC    Total IC
 *
 * @param [in]  *IC    cell_asic stucture pointer
 *
 * @param [in]  type    Enum type of resistor group
 *
 * @return None
 *
 *******************************************************************************
*/
void printVoltages(uint8_t tIC, cell_asic *IC, TYPE type)
{
  float voltage;
  int16_t temp;
  uint8_t channel;
  if((type == Cell) || (type == AvgCell) || (type == F_volt) || (type == S_volt))
  {
    channel = CELL;
  }
  else if (type == Aux){ channel = AUX;}
  else if (type == RAux){channel = RAUX;}
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:",(ic+1));
    for(uint8_t index = 0; index < channel; index++)
    {
      if(type == Cell){ temp = IC[ic].cell.c_codes[index]; }
      else if(type == AvgCell){ temp = IC[ic].acell.ac_codes[index]; }
      else if(type == F_volt){ temp = IC[ic].fcell.fc_codes[index]; }
      else if(type == S_volt){ temp = IC[ic].scell.sc_codes[index]; }
      else if(type == Aux){ temp = IC[ic].aux.a_codes[index]; }
      else if(type == RAux){ temp = IC[ic].raux.ra_codes[index]; }
      voltage = getVoltage(temp);
      if(type == Cell)
      {
        Serial.printf("C%d=%fV,",(index+1), voltage);
        if(index == (channel-1))
        {
          Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
          Serial.printf("PECError:%d",IC[ic].cccrc.cell_pec);
        }
      }
      else if(type == AvgCell)
      {
        Serial.printf("AC%d=%fV,",(index+1), voltage);
        if(index == (channel-1))
        {
          Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
          Serial.printf("PECError:%d",IC[ic].cccrc.acell_pec);
        }
      }
      else if(type == F_volt)
      {
        Serial.printf("FC%d=%fV,",(index+1), voltage);
        if(index == (channel-1))
        {
          Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
          Serial.printf("PECError:%d",IC[ic].cccrc.fcell_pec);
        }
      }
      else if(type == S_volt)
      {
        Serial.printf("S%d=%fV,",(index+1), voltage);
        if(index == (channel-1))
        {
          Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
          Serial.printf("PECError:%d",IC[ic].cccrc.scell_pec);
        }
      }
      else if(type == Aux)
      {
        if(index <= 9)
        {
          Serial.printf("AUX%d=%fV,",(index+1), voltage);
        }
        else if(index == 10)
        {
          Serial.printf("VMV:%fV,",(20 * voltage));
        }
        else if(index == 11)
        {
          Serial.printf("V+:%fV,",(20 * voltage));
          Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
          Serial.printf("PECError:%d",IC[ic].cccrc.aux_pec);
        }
      }
      else if(type == RAux)
      {
        Serial.printf("RAUX%d=%fV,",(index+1), voltage);
        if(index == (channel-1))
        {
          Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
          Serial.printf("PECError:%d",IC[ic].cccrc.raux_pec);
        }
      }
      else{ Serial.printf("Wrong Register Group Select\n"); }
    }
    Serial.printf("\n\n");
  }
}

/**
 *******************************************************************************
 * Function: PrintStatus
 * @brief Print status reg. result.
 *
 * @details This function Print status result into IAR I/O terminal.
 *
 * Parameters:
 * @param [in]	tIC      Total IC
 *
 * @param [in]  *IC      cell_asic stucture pointer
 *
 * @param [in]  type     Enum type of resistor
 *
 * @param [in]  grp      Enum type of resistor group
 *
 * @return None
 *
 *******************************************************************************
*/
void printStatus(uint8_t tIC, cell_asic *IC, TYPE type, GRP grp)
{
  float voltage;
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(type == Status)
    {
      if(grp == AA)
      {
        Serial.printf("Status A:\n");
        voltage = getVoltage(IC[ic].stata.vref2);
        Serial.printf("VREF2:%fV, ", voltage);
        voltage = getVoltage(IC[ic].stata.vref3);
        Serial.printf("VREF3:%fV, ", voltage);
        voltage = getVoltage(IC[ic].stata.itmp);
        Serial.printf("ITMP: %fC\n", (voltage/0.0075)-273);
        Serial.printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n",IC[ic].cccrc.stat_pec);
      }
      else if(grp == BB)
      {
        Serial.printf("Status B:\n");
        voltage = getVoltage(IC[ic].statb.va);
        Serial.printf("VA:%fV, ", voltage);
        voltage = getVoltage(IC[ic].statb.vd);
        Serial.printf("VD:%fV, ", voltage);
        voltage = getVoltage(IC[ic].statb.vr4k);
        Serial.printf("VR4K:%fV\n", voltage);

        Serial.printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n",IC[ic].cccrc.stat_pec);
      }
      else if(grp == CC)
      {
        Serial.printf("Status C:\n");
        Serial.printf("CSFLT:0x%X, ", IC[ic].statc.cs_flt);

        Serial.printf("OTP2_MED:0x%X, ", IC[ic].statc.otp2_med);
        Serial.printf("OTP2_ED:0x%X, ", IC[ic].statc.otp2_ed);
        Serial.printf("OTP1_MED:0x%X ", IC[ic].statc.otp1_med);
        Serial.printf("OTP1_ED:0x%X, ", IC[ic].statc.otp1_ed);
        Serial.printf("VD_UV:0x%X, ", IC[ic].statc.vd_uv);
        Serial.printf("VD_OV:0x%X, ", IC[ic].statc.vd_ov);
        Serial.printf("VA_UV:0x%X, ", IC[ic].statc.va_uv);
        Serial.printf("VA_OV:0x%X\n", IC[ic].statc.va_ov);

        Serial.printf("OSCCHK:0x%X, ", IC[ic].statc.oscchk);
        Serial.printf("TMODCHK:0x%X, ", IC[ic].statc.tmodchk);
        Serial.printf("THSD:0x%X, ", IC[ic].statc.thsd);
        Serial.printf("SLEEP:0x%X, ", IC[ic].statc.sleep);
        Serial.printf("SPIFLT:0x%X, ", IC[ic].statc.spiflt);
        Serial.printf("COMP:0x%X, ", IC[ic].statc.comp);
        Serial.printf("VDEL:0x%X, ", IC[ic].statc.vdel);
        Serial.printf("VDE:0x%X\n", IC[ic].statc.vde);

        Serial.printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n", IC[ic].cccrc.stat_pec);
      }
      else if(grp == D)
      {
        Serial.printf("Status D:\n");
        Serial.printf("C1UV:0x%X, ", IC[ic].statd.c_uv[0]);
        Serial.printf("C2UV:0x%X, ", IC[ic].statd.c_uv[1]);
        Serial.printf("C3UV:0x%X, ", IC[ic].statd.c_uv[2]);
        Serial.printf("C4UV:0x%X, ", IC[ic].statd.c_uv[3]);
        Serial.printf("C5UV:0x%X, ", IC[ic].statd.c_uv[4]);
        Serial.printf("C6UV:0x%X, ", IC[ic].statd.c_uv[5]);
        Serial.printf("C7UV:0x%X, ", IC[ic].statd.c_uv[6]);
        Serial.printf("C8UV:0x%X, ", IC[ic].statd.c_uv[7]);
        Serial.printf("C9UV:0x%X, ", IC[ic].statd.c_uv[8]);
        Serial.printf("C10UV:0x%X, ", IC[ic].statd.c_uv[9]);
        Serial.printf("C11UV:0x%X, ", IC[ic].statd.c_uv[10]);
        Serial.printf("C12UV:0x%X, ", IC[ic].statd.c_uv[11]);
        Serial.printf("C13UV:0x%X, ", IC[ic].statd.c_uv[12]);
        Serial.printf("C14UV:0x%X, ", IC[ic].statd.c_uv[13]);
        Serial.printf("C15UV:0x%X, ", IC[ic].statd.c_uv[14]);
        Serial.printf("C16UV:0x%X\n", IC[ic].statd.c_uv[15]);

        Serial.printf("C1OV:0x%X, ", IC[ic].statd.c_ov[0]);
        Serial.printf("C2OV:0x%X, ", IC[ic].statd.c_ov[1]);
        Serial.printf("C3OV:0x%X, ", IC[ic].statd.c_ov[2]);
        Serial.printf("C4OV:0x%X, ", IC[ic].statd.c_ov[3]);
        Serial.printf("C5OV:0x%X, ", IC[ic].statd.c_ov[4]);
        Serial.printf("C6OV:0x%X, ", IC[ic].statd.c_ov[5]);
        Serial.printf("C7OV:0x%X, ", IC[ic].statd.c_ov[6]);
        Serial.printf("C8OV:0x%X, ", IC[ic].statd.c_ov[7]);
        Serial.printf("C9OV:0x%X, ", IC[ic].statd.c_ov[8]);
        Serial.printf("C10OV:0x%X, ", IC[ic].statd.c_ov[9]);
        Serial.printf("C11OV:0x%X, ", IC[ic].statd.c_ov[10]);
        Serial.printf("C12OV:0x%X, ", IC[ic].statd.c_ov[11]);
        Serial.printf("C13OV:0x%X, ", IC[ic].statd.c_ov[12]);
        Serial.printf("C14OV:0x%X, ", IC[ic].statd.c_ov[13]);
        Serial.printf("C15OV:0x%X, ", IC[ic].statd.c_ov[14]);
        Serial.printf("C16OV:0x%X\n", IC[ic].statd.c_ov[15]);

        Serial.printf("CTS:0x%X, ", IC[ic].statd.cts);
        Serial.printf("CT:0x%X, ", IC[ic].statd.ct);
        Serial.printf("OC_CNTR:0x%X\n", IC[ic].statd.oc_cntr);

        Serial.printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n", IC[ic].cccrc.stat_pec);
      }
      else if(grp == E)
      {
        Serial.printf("Status E:\n");
        Serial.printf("GPI:0x%X, ", IC[ic].state.gpi);
        Serial.printf("REV_ID:0x%X\n", IC[ic].state.rev);

        Serial.printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n", IC[ic].cccrc.stat_pec);
      }
      else if(grp == ALL_GRP)
      {
        Serial.printf("Status A:\n");
        voltage = getVoltage(IC[ic].stata.vref2);
        Serial.printf("VREF2:%fV, ", voltage);
        voltage = getVoltage(IC[ic].stata.vref3);
        Serial.printf("VREF3:%fV, ", voltage);
        voltage = getVoltage(IC[ic].stata.itmp);
        Serial.printf("ITMP:%fC\n\n", (voltage/0.0075)-273);

        Serial.printf("Status B:\n");
        voltage = getVoltage(IC[ic].statb.va);
        Serial.printf("VA:%fV, ", voltage);
        voltage = getVoltage(IC[ic].statb.vd);
        Serial.printf("VD:%fV, ", voltage);
        voltage = getVoltage(IC[ic].statb.vr4k);
        Serial.printf("VR4K:%fV\n\n", voltage);

        Serial.printf("Status C:\n");
        Serial.printf("CSFLT:0x%X, ", IC[ic].statc.cs_flt);

        Serial.printf("OTP2_MED:0x%X, ", IC[ic].statc.otp2_med);
        Serial.printf("OTP2_ED:0x%X, ", IC[ic].statc.otp2_ed);
        Serial.printf("OTP1_MED:0x%X, ", IC[ic].statc.otp1_med);
        Serial.printf("OTP1_ED:0x%X, ", IC[ic].statc.otp1_ed);
        Serial.printf("VD_UV:0x%X, ", IC[ic].statc.vd_uv);
        Serial.printf("VD_OV:0x%X, ", IC[ic].statc.vd_ov);
        Serial.printf("VA_UV:0x%X, ", IC[ic].statc.va_uv);
        Serial.printf("VA_OV:0x%X\n", IC[ic].statc.va_ov);

        Serial.printf("OSCCHK:0x%X, ", IC[ic].statc.oscchk);
        Serial.printf("TMODCHK:0x%X, ", IC[ic].statc.tmodchk);
        Serial.printf("THSD:0x%X, ", IC[ic].statc.thsd);
        Serial.printf("SLEEP:0x%X, ", IC[ic].statc.sleep);
        Serial.printf("SPIFLT:0x%X, ", IC[ic].statc.spiflt);
        Serial.printf("COMP:0x%X, ", IC[ic].statc.comp);
        Serial.printf("VDEL:0x%X, ", IC[ic].statc.vdel);
        Serial.printf("VDE:0x%X\n\n", IC[ic].statc.vde);

        Serial.printf("Status D:\n");
        Serial.printf("C1UV:0x%X, ", IC[ic].statd.c_uv[0]);
        Serial.printf("C2UV:0x%X, ", IC[ic].statd.c_uv[1]);
        Serial.printf("C3UV:0x%X, ", IC[ic].statd.c_uv[2]);
        Serial.printf("C4UV:0x%X, ", IC[ic].statd.c_uv[3]);
        Serial.printf("C5UV:0x%X, ", IC[ic].statd.c_uv[4]);
        Serial.printf("C6UV:0x%X, ", IC[ic].statd.c_uv[5]);
        Serial.printf("C7UV:0x%X, ", IC[ic].statd.c_uv[6]);
        Serial.printf("C8UV:0x%X, ", IC[ic].statd.c_uv[7]);
        Serial.printf("C9UV:0x%X, ", IC[ic].statd.c_uv[8]);
        Serial.printf("C10UV:0x%X, ", IC[ic].statd.c_uv[9]);
        Serial.printf("C11UV:0x%X, ", IC[ic].statd.c_uv[10]);
        Serial.printf("C12UV:0x%X, ", IC[ic].statd.c_uv[11]);
        Serial.printf("C13UV:0x%X, ", IC[ic].statd.c_uv[12]);
        Serial.printf("C14UV:0x%X, ", IC[ic].statd.c_uv[13]);
        Serial.printf("C15UV:0x%X, ", IC[ic].statd.c_uv[14]);
        Serial.printf("C16UV:0x%X\n", IC[ic].statd.c_uv[15]);

        Serial.printf("C1OV:0x%X, ", IC[ic].statd.c_ov[0]);
        Serial.printf("C2OV:0x%X, ", IC[ic].statd.c_ov[1]);
        Serial.printf("C3OV:0x%X, ", IC[ic].statd.c_ov[2]);
        Serial.printf("C4OV:0x%X, ", IC[ic].statd.c_ov[3]);
        Serial.printf("C5OV:0x%X, ", IC[ic].statd.c_ov[4]);
        Serial.printf("C6OV:0x%X, ", IC[ic].statd.c_ov[5]);
        Serial.printf("C7OV:0x%X, ", IC[ic].statd.c_ov[6]);
        Serial.printf("C8OV:0x%X, ", IC[ic].statd.c_ov[7]);
        Serial.printf("C9OV:0x%X, ", IC[ic].statd.c_ov[8]);
        Serial.printf("C10OV:0x%X, ", IC[ic].statd.c_ov[9]);
        Serial.printf("C11OV:0x%X, ", IC[ic].statd.c_ov[10]);
        Serial.printf("C12OV:0x%X, ", IC[ic].statd.c_ov[11]);
        Serial.printf("C13OV:0x%X, ", IC[ic].statd.c_ov[12]);
        Serial.printf("C14OV:0x%X, ", IC[ic].statd.c_ov[13]);
        Serial.printf("C15OV:0x%X, ", IC[ic].statd.c_ov[14]);
        Serial.printf("C16OV:0x%X\n", IC[ic].statd.c_ov[15]);

        Serial.printf("CTS:0x%X, ", IC[ic].statd.cts);
        Serial.printf("CT:0x%X\n\n", IC[ic].statd.ct);

        Serial.printf("Status E:\n");
        Serial.printf("GPI:0x%X, ", IC[ic].state.gpi);
        Serial.printf("REV_ID:0x%X\n\n", IC[ic].state.rev);

        Serial.printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n", IC[ic].cccrc.stat_pec);
      }
      else{ Serial.printf("Wrong Register Group Select\n"); }
    }
  }
}

/**
 *******************************************************************************
 * Function: PrintDeviceSID
 * @brief Print Device SID.
 *
 * @details This function Print Device SID into IAR I/O terminal.
 *
 * Parameters:
 * @param [in]	tIC      Total IC
 *
 * @param [in]  *IC      cell_asic stucture pointer
 *
 * @param [in]  type     Enum type of resistor
 *
 * @return None
 *
 *******************************************************************************
*/
void printDeviceSID(uint8_t tIC, cell_asic *IC, TYPE type)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(type == Sid)
    {
        Serial.printf("Read Device SID:\n");
        Serial.printf("0x%X, ", IC[ic].sid.sid[0]);
        Serial.printf("0x%X, ", IC[ic].sid.sid[1]);
        Serial.printf("0x%X, ", IC[ic].sid.sid[2]);
        Serial.printf("0x%X, ", IC[ic].sid.sid[3]);
        Serial.printf("0x%X, ", IC[ic].sid.sid[4]);
        Serial.printf("0x%X, ", IC[ic].sid.sid[5]);
        Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
        Serial.printf("PECError:%d\n\n",IC[ic].cccrc.sid_pec);
     }
     else{ Serial.printf("Wrong Register Type Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printWritePwmDutyCycle
* @brief Print Write Pwm Duty Cycle.
*
* @details This function Print write pwm duty cycle value.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @param [in]  grp      Enum group of resistor
*
* @return None
*
*******************************************************************************
*/
void printWritePwmDutyCycle(uint8_t tIC, cell_asic *IC, TYPE type, GRP grp)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(grp == AA)
    {
      Serial.printf("Write Pwma Duty Cycle:\n");
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[0]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[1]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[2]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[3]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[4]);
      Serial.printf("0x%X\n\n", IC[ic].pwma.tx_data[5]);
    }
    else if(grp == BB)
    {
      Serial.printf("Write Pwmb Duty Cycle:\n");
      Serial.printf("0x%X, ", IC[ic].pwmb.tx_data[0]);
      Serial.printf("0x%X\n\n", IC[ic].pwmb.tx_data[1]);
    }
    else if(grp == ALL_GRP)
    {
      Serial.printf("Write Pwma Duty Cycle:\n");
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[0]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[1]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[2]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[3]);
      Serial.printf("0x%X, ", IC[ic].pwma.tx_data[4]);
      Serial.printf("0x%X\n", IC[ic].pwma.tx_data[5]);

      Serial.printf("Write Pwmb Duty Cycle:\n");
      Serial.printf("0x%X, ", IC[ic].pwmb.tx_data[0]);
      Serial.printf("0x%X\n\n", IC[ic].pwmb.tx_data[1]);
    }
    else{ Serial.printf("Wrong Register Group Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printReadPwmDutyCycle
* @brief Print Read Pwm Duty Cycle.
*
* @details This function print read pwm duty cycle value.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @param [in]  grp      Enum group of resistor
*
* @return None
*
*******************************************************************************
*/
void printReadPwmDutyCycle(uint8_t tIC, cell_asic *IC, TYPE type, GRP grp)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(grp == AA)
    {
      Serial.printf("Read PWMA Duty Cycle:\n");
      Serial.printf("PWM1:0x%X, ", IC[ic].PwmA.pwma[0]);
      Serial.printf("PWM2:0x%X, ", IC[ic].PwmA.pwma[1]);
      Serial.printf("PWM3:0x%X, ", IC[ic].PwmA.pwma[2]);
      Serial.printf("PWM4:0x%X, ", IC[ic].PwmA.pwma[3]);
      Serial.printf("PWM5:0x%X, ", IC[ic].PwmA.pwma[4]);
      Serial.printf("PWM6:0x%X, ", IC[ic].PwmA.pwma[5]);
      Serial.printf("PWM7:0x%X, ", IC[ic].PwmA.pwma[6]);
      Serial.printf("PWM8:0x%X, ", IC[ic].PwmA.pwma[7]);
      Serial.printf("PWM9:0x%X, ", IC[ic].PwmA.pwma[8]);
      Serial.printf("PWM10:0x%X, ", IC[ic].PwmA.pwma[9]);
      Serial.printf("PWM11:0x%X, ", IC[ic].PwmA.pwma[10]);
      Serial.printf("PWM12:0x%X, ", IC[ic].PwmA.pwma[11]);
      Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
      Serial.printf("PECError:%d\n\n",IC[ic].cccrc.pwm_pec);
    }
    else if(grp == BB)
    {
      Serial.printf("Read PWMB Duty Cycle:\n");
      Serial.printf("PWM13:0x%X, ", IC[ic].PwmB.pwmb[0]);
      Serial.printf("PWM14:0x%X, ", IC[ic].PwmB.pwmb[1]);
      Serial.printf("PWM15:0x%X, ", IC[ic].PwmB.pwmb[2]);
      Serial.printf("PWM16:0x%X, ", IC[ic].PwmB.pwmb[3]);
      Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
      Serial.printf("PECError:%d\n\n",IC[ic].cccrc.pwm_pec);
    }
    else if(grp == ALL_GRP)
    {
      Serial.printf("Read PWMA Duty Cycle:\n");
      Serial.printf("PWM1:0x%X, ", IC[ic].PwmA.pwma[0]);
      Serial.printf("PWM2:0x%X, ", IC[ic].PwmA.pwma[1]);
      Serial.printf("PWM3:0x%X, ", IC[ic].PwmA.pwma[2]);
      Serial.printf("PWM4:0x%X, ", IC[ic].PwmA.pwma[3]);
      Serial.printf("PWM5:0x%X, ", IC[ic].PwmA.pwma[4]);
      Serial.printf("PWM6:0x%X, ", IC[ic].PwmA.pwma[5]);
      Serial.printf("PWM7:0x%X, ", IC[ic].PwmA.pwma[6]);
      Serial.printf("PWM8:0x%X, ", IC[ic].PwmA.pwma[7]);
      Serial.printf("PWM9:0x%X, ", IC[ic].PwmA.pwma[8]);
      Serial.printf("PWM10:0x%X, ", IC[ic].PwmA.pwma[9]);
      Serial.printf("PWM11:0x%X, ", IC[ic].PwmA.pwma[10]);
      Serial.printf("PWM12:0x%X\n", IC[ic].PwmA.pwma[11]);

      Serial.printf("Read PWMB Duty Cycle:\n");
      Serial.printf("PWM13:0x%X, ", IC[ic].PwmB.pwmb[0]);
      Serial.printf("PWM14:0x%X, ", IC[ic].PwmB.pwmb[1]);
      Serial.printf("PWM15:0x%X, ", IC[ic].PwmB.pwmb[2]);
      Serial.printf("PWM16:0x%X, ", IC[ic].PwmB.pwmb[3]);
      Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
      Serial.printf("PECError:%d\n\n",IC[ic].cccrc.pwm_pec);
    }
    else{ Serial.printf("Wrong Register Type Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printWriteCommData
* @brief Print Write Comm data.
*
* @details This function Print write comm data.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @return None
*
*******************************************************************************
*/
void printWriteCommData(uint8_t tIC, cell_asic *IC, TYPE type)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(type == Comm)
    {
      Serial.printf("Write Comm Data:\n");
      Serial.printf("0x%X, ", IC[ic].com.tx_data[0]);
      Serial.printf("0x%X, ", IC[ic].com.tx_data[1]);
      Serial.printf("0x%X, ", IC[ic].com.tx_data[2]);
      Serial.printf("0x%X, ", IC[ic].com.tx_data[3]);
      Serial.printf("0x%X, ", IC[ic].com.tx_data[4]);
      Serial.printf("0x%X\n\n", IC[ic].com.tx_data[5]);
    }
    else{ Serial.printf("Wrong Register Group Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printReadCommData
* @brief Print Read Comm Data.
*
* @details This function print read comm data.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @return None
*
*******************************************************************************
*/
void printReadCommData(uint8_t tIC, cell_asic *IC, TYPE type)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    Serial.printf("IC%d:\n",(ic+1));
    if(type == Comm)
    {
      Serial.printf("Read Comm Data:\n");
      Serial.printf("ICOM0:0x%X, ", IC[ic].comm.icomm[0]);
      Serial.printf("ICOM1:0x%X, ", IC[ic].comm.icomm[1]);
      Serial.printf("ICOM2:0x%X\n", IC[ic].comm.icomm[2]);
      Serial.printf("FCOM0:0x%X, ", IC[ic].comm.fcomm[0]);
      Serial.printf("FCOM1:0x%X, ", IC[ic].comm.fcomm[1]);
      Serial.printf("FCOM2:0x%X\n", IC[ic].comm.fcomm[2]);
      Serial.printf("DATA0:0x%X, ", IC[ic].comm.data[0]);
      Serial.printf("DATA1:0x%X, ", IC[ic].comm.data[1]);
      Serial.printf("DATA2:0x%X\n", IC[ic].comm.data[2]);
      Serial.printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
      Serial.printf("PECError:%d\n\n",IC[ic].cccrc.comm_pec);
    }
    else{ Serial.printf("Wrong Register Type Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printDiagnosticTestResult
* @brief Print diagnostic test result.
*
* @details This function Print diagnostic test result (PASS,FAIL) into console terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic stucture pointer
*
* @param [in]  TEST     Enum type diagnostic test
*
* @return None
*
*******************************************************************************
*/
void printDiagnosticTestResult(uint8_t tIC, cell_asic *IC, DIAGNOSTIC_TYPE type)
{
  if(type == OSC_MISMATCH)
  {
    Serial.printf("OSC Diagnostic Test:\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:",(ic+1));
      diagnosticTestResultPrint(IC[ic].diag_result.osc_mismatch);
    }
    Serial.printf("\n\n");
  }

  else if(type == SUPPLY_ERROR)
  {
    Serial.printf("Force Supply Error Detection Test:\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:",(ic+1));
      diagnosticTestResultPrint(IC[ic].diag_result.supply_error);
    }
    Serial.printf("\n\n");
  }

  else if(type == THSD)
  {
    Serial.printf("Thsd Diagnostic Test:\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:",(ic+1));
      diagnosticTestResultPrint(IC[ic].diag_result.thsd);
    }
    Serial.printf("\n\n");
  }

  else if(type == FUSE_ED)
  {
    Serial.printf("Fuse_ed Diagnostic Test:\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:",(ic+1));
      diagnosticTestResultPrint(IC[ic].diag_result.fuse_ed);
    }
    Serial.printf("\n\n");
  }

  else if(type == FUSE_MED)
  {
    Serial.printf("Fuse_med Diagnostic Test:\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:",(ic+1));
      diagnosticTestResultPrint(IC[ic].diag_result.fuse_med);
    }
    Serial.printf("\n\n");
  }

  else if(type == TMODCHK)
  {
    Serial.printf("TMODCHK Diagnostic Test:\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:",(ic+1));
      diagnosticTestResultPrint(IC[ic].diag_result.tmodchk);
    }
    Serial.printf("\n\n");
  }
  else{Serial.printf("Wrong Diagnostic Selected\n");}
}

/**
*******************************************************************************
* Function: diagnosticResultPrint
* @brief Print diagnostic (PASS/FAIL) result.
*
* @details This function print diagnostic (PASS/FAIL) result into console.
*
* Parameters:
* @param [in]	result   Result byte
*
* @return None
*
*******************************************************************************
*/
void diagnosticTestResultPrint(uint8_t result)
{
  if(result == 1)
  {
    Serial.printf("PASS\n");
  }
  else
  {
    Serial.printf("FAIL\n");
  }
}

/**
*******************************************************************************
* Function: printOpenWireTestResult
* @brief Print open wire test result.
*
* @details This function print open wire test result.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @return None
*
*******************************************************************************
*/
void printOpenWireTestResult(uint8_t tIC, cell_asic *IC, TYPE type)
{
  if(type == Cell)
  {
    Serial.printf("Cell Open Wire Test\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:\n",(ic+1));
      for(uint8_t cell = 0; cell < CELL; cell++)
      {
        Serial.printf("CELL%d:",(cell+1));
        openWireResultPrint(IC[ic].diag_result.cell_ow[cell]);
      }
      Serial.printf("\n\n");
    }
  }
  else if(type == S_volt)
  {
    Serial.printf("Cell redundant Open Wire Test\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:\n",(ic+1));
      for(uint8_t cell = 0; cell < CELL; cell++)
      {
        Serial.printf("CELL%d:",(cell+1));
        openWireResultPrint(IC[ic].diag_result.cellred_ow[cell]);
      }
      Serial.printf("\n\n");
    }
  }
  else if(type == Aux)
  {
    Serial.printf("Aux Open Wire Test\n");
    for(uint8_t ic = 0; ic < tIC; ic++)
    {
      Serial.printf("IC%d:\n",(ic+1));
      for(uint8_t gpio = 0; gpio < (AUX-2); gpio++)
      {
        Serial.printf("GPIO%d:",(gpio+1));
        openWireResultPrint(IC[ic].diag_result.aux_ow[gpio]);
      }
      Serial.printf("\n\n");
    }
  }
  else{Serial.printf("Wrong Resistor Type Selected\n");}
}

/**
*******************************************************************************
* Function: openWireResultPrint
* @brief Print open wire (OPEN/CLOSE) result.
*
* @details This function print open wire result into console.
*
* Parameters:
* @param [in]	result   Result byte
*
* @return None
*
*******************************************************************************
*/
void openWireResultPrint(uint8_t result)
{
  if(result == 1)
  {
    Serial.printf(" OPEN\n");
  }
  else
  {
    Serial.printf(" CLOSE\n");
  }
}

/**
 *******************************************************************************
 * Function: printPollAdcConvTime
 * @brief Print Poll adc conversion Time.
 *
 * @details This function print poll adc conversion Time.
 *
 * @return None
 *
 *******************************************************************************
*/
void printPollAdcConvTime(int count)
{
  Serial.printf("Adc Conversion Time = %fms\n", (float)(count/64000.0));
}

/**
 *******************************************************************************
 * Function: printMenu
 * @brief Print Command Menu.
 *
 * @details This function print all command menu.
 *
 * @return None
 *
 *******************************************************************************
*/
void printMenu()
{
  Serial.println("Printing menu:");
  Serial.printf("List of ADBMS6830 Command:\n");
  Serial.printf("Write and Read Configuration: 1 \n");
  Serial.printf("Read Configuration: 2 \n");
  Serial.printf("Start Cell Voltage Conversion: 3 \n");
  Serial.printf("Read Cell Voltages: 4 \n");
  Serial.printf("Start S-Voltage Conversion: 5 \n");
  Serial.printf("Read S-Voltages: 6 \n");
  Serial.printf("Start Avg Cell Voltage Conversion: 7 \n");
  Serial.printf("Read Avg Cell Voltages: 8 \n");
  Serial.printf("Start F-Cell Voltage Conversion: 9 \n");
  Serial.printf("Read F-Cell Voltages: 10 \n");
  Serial.printf("Start Aux Voltage Conversion: 11 \n");
  Serial.printf("Read Aux Voltages: 12 \n");
  Serial.printf("Start RAux Voltage Conversion: 13 \n");
  Serial.printf("Read RAux Voltages: 14 \n");
  Serial.printf("Read Status Registers: 15 \n");
  Serial.printf("Loop Measurements: 16 \n");
  Serial.printf("Clear Cell registers: 17 \n");
  Serial.printf("Clear Aux registers: 18 \n");
  Serial.printf("Clear Spin registers: 19 \n");
  Serial.printf("Clear Fcell registers: 20 \n");

  Serial.printf("\n");
  Serial.printf("Print '0' for menu\n");
  Serial.printf("Please enter command: \n");
  Serial.printf("\n\n");
}
/**
 *******************************************************************************
 * Function: getVoltage
 * @brief Get voltages with multiplication factor.
 *
 * @details This function calculate the voltage.
 *
 * Parameters:
 * @param [in]	data    voltages(uint16_t)
 *
 * @return voltage(float)
 *
 *******************************************************************************
*/
float getVoltage(int data)
{
    float voltage_float; //voltage in Volts
    voltage_float = ((data + 10000) * 0.000150);
    return voltage_float;
}

/** @}*/
/** @}*/