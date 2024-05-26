#ifndef _ACU_DATA_H
#define _ACU_DATA_H
#include <Arduino.h>
#include <SPI.h>
// CONFIG OPTIONS FOR 6830 ARE IN THE CPP FILE

#define TOTAL_IC 3

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
#define PIN_DCDC_ER 18 //in the words of owen: "if it's high it's fucked", Buck Bad pin
#define PIN_DCDC_EN 15 // enable DC/DC converter
#define PIN_DCDC_SLOW 16 // DC/DC low power mode
#define PIN_IMD_OK 17 // Isolation monitoring device
#define PIN_BSPD_CLK 14 // PWM_CLK, 50k

//limits, need to change prob to extern when we read from EEPROM
// #define OV_THRESHOLD 42000                 /* Volt in 0.1 mV*/
// #define UV_THRESHOLD 30000                 /* Volt in 0.1 mV*/
//Charging
#define MIN_CHR_TEMP 0 //TRIAGE 3: Modify later
#define MAX_CHR_TEMP 60 

//Bat limits
#define SAFE_V_TO_TURN_OFF 60
//Balance Resistor Temp
#define MIN_BAL_TEMP -273.1 //TRIAGE 3: Modify later
#define MAX_BAL_TEMP 80 
//Discharge
#define MIN_DIS_TEMP -40 //TRIAGE 3: Modify later
#define MAX_DIS_TEMP 60 


//acu limits
#define MAX_HV_CURRENT 135 //TRIAGE 3: Modify later
#define MAX_DCDC_TEMP 70
#define MAX_DCDC_CURRENT 10
#define MIN_GLV_VOLT 10
#define MIN_FAN_REF_VOLT 4.5
#define MAX_FAN_REF_VOLT 5.7


// ADC1283 connections
#define ADC_MUX_GLV_VOLT 7
#define ADC_MUX_HV_VOLT 6
#define ADC_MUX_HV_CURRENT 0
#define ADC_MUX_SHDN_POW 1
#define ADC_MUX_DCDC_CURRENT 5
#define ADC_MUX_DCDC_TEMP1 2
#define ADC_MUX_DCDC_TEMP2 3
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
#define CHARGER_VOLTAGE 540 //V
#define CHARGER_CURRENT 5 //A

#define PRECHARGE_THRESHOLD 500 // * 10mV

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

/*  Bit 0: true = Device error active
    Bit 1: true = HV_pos connection failure
    Bit 2: true = HV_neg connection failure
    Bit 3: true = Earth connection failure
    Bit 4: true = Iso Alarm (iso value below threshold error)
    Bit 5: true = Iso Warning (iso value below threshold warning)
    Bit 6: true = Iso Outdated (value, Time elapsed since lst measurement“ > = „measurement timeout“)
    Bit 7: true = Unbalance Alarm (unbalance value below threshold)
    Bit 8: true = Undervoltage Alarm
    Bit 9: true = Unsafe to Start
    Bit 10: true = Earthlift open */
#define IMD_Failure_Bits 0b000000000000;


#endif