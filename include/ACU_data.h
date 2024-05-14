#ifndef _ACU_DATA_H
#define _ACU_DATA_H
#include <Arduino.h>
#include <SPI.h>
// CONFIG OPTIONS FOR 6830 ARE IN THE CPP FILE

#define TOTAL_IC 1

//TODO: Change these to the correct values


//Pin Definitions
#define PIN_AIR_POS 19 // assuming closed high
#define PIN_AIR_NEG 20 //assuming closed high
#define PIN_AIR_RESET 21
#define PIN_PRECHG 41 // also controls discharge relay, assuming preC closed high, disc open high
#define CS_ADC 1
// #define PIN_AIR_POS_AUX -1 // auxilary contacts
// #define PIN_AIR_NEG_AUX -1 // auxilary contacts
#define PIN_AMS_OK 39
#define PIN_DCDC_ER 18 //in the words of owen: "if it's high it's fucked", Buck Bad pin
#define PIN_DCDC_EN 15 // enable DC/DC converter
#define PIN_DCDC_SLOW 16 // DC/DC low power mode
#define PIN_IMD_OK 17 // Isolation monitoring device
#define PIN_BSPD_CLK 14 // PWM_CLK, 50k

//limits, need to change prob to extern when we read from EEPROM
#define OV_THRESHOLD 42000                 /* Volt in 0.1 mV*/
#define UV_THRESHOLD 30000                 /* Volt in 0.1 mV*/
//Discharge
#define MIN_DIS_TEMP -40 //TODO: Modify later
#define MAX_DIS_TEMP 60 
//Charging
#define MIN_CHR_TEMP 0 //TODO: Modify later
#define MAX_CHR_TEMP 60 
//Balance Resistor Temp
#define MIN_BAL_TEMP -273.1 //TODO: Modify later
#define MAX_BAL_TEMP 80 

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

// Error bit positions (Powertrain Cooling)
#define ERR_WaterOverTemp   0b10000000
#define ERR_Fan_One         0b01000000
#define ERR_Fan_Two         0b00100000
#define ERR_Fan_Three       0b00010000
#define ERR_Fan_Four        0b00001000
#define ERR_Pump            0b00000100

// Charger Values
#define CHARGER_VOLTAGE 540 //V
#define CHARGER_CURRENT 5 //A

#define PRECHARGE_THRESHOLD 500 // * 10mV


#endif