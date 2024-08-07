#ifndef _ACU_DATA_H
#define _ACU_DATA_H
#include <Arduino.h>
#include <SPI.h>
// CONFIG OPTIONS FOR 6830 ARE IN THE CPP FILE

#define TOTAL_IC 8

//TRIAGE 2: Change these to the correct values


//Pin Definitions
#define PIN_AIR_POS 19 // assuming closed high
#define PIN_AIR_NEG 20 //assuming closed high
#define PIN_AIR_RESET 21
#define PIN_PRECHG 41 // also controls discharge relay, assuming preC closed high, disc open high
#define CS_ADC 0
// #define PIN_AIR_POS_AUX -1 // auxilary contacts
// #define PIN_AIR_NEG_AUX -1 // auxilary contacts
#define PIN_AMS_OK 39
#define PIN_DCDC_ER 18 // if high, buck mosfet likely fried or buck regulator error
#define PIN_DCDC_EN 15 // enable DC/DC converter
#define PIN_DCDC_SLOW 16 // DC/DC low power mode
#define PIN_IMD_OK 17 // Isolation monitoring device
#define PIN_BSPD_CLK 14 // PWM_CLK, 50k


//Charging
#define MIN_CHR_TEMP 0 //TRIAGE 3: Modify later
#define MAX_CHR_TEMP 60 

//Bat limits
#define SAFE_V_TO_TURN_OFF 60
//Balance Resistor Temp
#define MIN_BAL_TEMP -273.1 //TRIAGE 3: Modify later
#define MAX_BAL_TEMP 80 
#define MIN_CELL_TEMP 0
#define MAX_CELL_TEMP 60
//Discharge
#define MIN_DIS_TEMP -40 //TRIAGE 3: Modify later
#define MAX_DIS_TEMP 60 


//acu limits
#define MAX_HV_CURRENT 135 //TRIAGE 3: Modify later
#define MAX_DCDC_TEMP 60
#define MAX_DCDC_CURRENT 10
#define MIN_GLV_VOLT 10
#define MAX_GLV_VOLT 14.4


// hardware constants
#define CELL_INT_RESISTANCE 0.006
#define CELL_EMPTY_VOLTAGE 3
#define CELL_FULL_VOLTAGE 4.15


// ADC1283 connections
#define ADC_MUX_GLV_VOLT 7
#define ADC_MUX_HV_VOLT 6
#define ADC_MUX_HV_CURRENT 0
#define ADC_MUX_SHDN_POW 1
#define ADC_MUX_DCDC_CURRENT 5
#define ADC_MUX_TEMP1 2
#define ADC_MUX_TEMP2 3
#define ADC_MUX_FAN_REF 4



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
#define CHARGER_VOLTAGE 531 //V
#define CHARGER_CURRENT 2 //A

#define PRECHARGE_THRESHOLD 0.96 // fraction of total cell voltage

//masks
//error 
#define ERR_OverTemp 0b10000000
#define ERR_OverVolt 0b01000000
#define ERR_OverCurr 0b00100000
#define ERR_BMS      0b00010000
#define ERR_UndrVolt 0b00001000
#define ERR_Prechrg  0b00000100
#define ERR_Teensy   0b00000010
#define ERR_UndrTemp 0b00000001

//warnings
#define WARN_OpenWire 0b10000000
#define WARN_ADBMSADC 0b01000000
#define WARN_CellDrop 0b00100000
#define WARN_HighCurr 0b00010000
#define WARN_LowChrg  0b00001000
#define WARN_CellInbl 0b00000100
#define WARN_Humidity 0b00000010
#define WARN_Hydrogen 0b00000001

//bit masks of ACU_General_2
#define MASK_CAN_AIR_NEG        0b10000000
#define MASK_CAN_AIR_POS        0b01000000
#define MASK_CAN_PRECHARGE      0b00100000
#define MASK_CAN_PRECHARGE_DONE 0b00010000
#define MASK_CAN_SHUTDOWN       0b00001000

// relay switching times
#define DELAY_AIR_SW 30
#define DELAY_PCHG_SW 5

// error margins
#define ERRMG_GLV_SDC 4
#define ERRMG_ISNS_VREF 0.2
#define ERRMG_5V 0.2
#define ERRMG_CELL_VOLT_ERR 20
#define ERRMG_CELL_TEMP_ERR 50
#define ERRMG_ACU_ERR 50

#endif