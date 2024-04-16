#ifndef _ACU_DATA_H
#define _ACU_DATA_H
#include <Arduino.h>
#include <SPI.h>
// CONFIG OPTIONS FOR 6830 ARE IN THE CPP FILE

#define TOTAL_IC 8

//TODO: Change these to the correct values


//Pin Definitions
#define PIN_AIR_POS -1
#define PIN_AIR_NEG -1
#define PIN_PRECHG -1 // also controls discharge relay
#define CS_ADC -1
#define PIN_AIR_POS_AUX -1 // auxilary contacts
#define PIN_AIR_NEG_AUX -1 // auxilary contacts
#define PIN_AMS_OK -1
#define PIN_DCDC_EN -1 // enable DC/DC converter
#define PIN_DCDC_SLOW -1 // DC/DC low power mode

// ADC1283 connections
#define ADC_MUX_GLV_VOLT -1
#define ADC_MUX_HV_VOLT -1
#define ADC_MUX_HV_CURRENT -1
#define ADC_MUX_DCDC_CURRENT -1
#define ADC_MUX_DCDC_TEMP1 -1
#define ADC_MUX_DCDC_TEMP2 -1
#define ADC_MUX_DCDC_TEMP3 -1

// Warning bit positions (ACU General)
#define WARN_OpenWire 0b10000000
#define WARN_ADBMSADC 0b01000000
#define WARN_CellDrop 0b00100000
#define WARN_HighCurr 0b00010000
#define WARN_LowChrg  0b00001000
#define WARN_CellInbl 0b00000100
#define WARN_Humidity 0b00000010
#define WARN_Hydrogen 0b00000001

// Error bit positions (ACU General)
#define ERR_OverTemp 0b10000000
#define ERR_OverVolt 0b01000000
#define ERR_OverCurr 0b00100000
#define ERR_BMS      0b00010000
#define ERR_UndrVolt 0b00001000
#define ERR_Prechrg  0b00000100
#define ERR_Teensy   0b00000010
#define ERR_UndrTemp 0b00000001

// Charger Values
#define CHARGER_VOLTAGE 540 //V
#define CHARGER_CURRENT 5 //A

#define PRECHARGE_THRESHOLD 500 // * 10mV

#endif