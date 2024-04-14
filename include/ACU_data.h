#ifndef _ACU_DATA_H
#define _ACU_DATA_H
#include <Arduino.h>
#include <SPI.h>
#include "adBms6830Data.h"
// CONFIG OPTIONS FOR 6830 ARE IN THE CPP FILE

#define TOTAL_IC 2

//TODO: Change these to the correct values


//Pin Definitions
#define AIR_PLUS -1
#define AIR_NEG -1
#define CAN_HI -1
#define CAN_LO -1
#define TSAL_RED -1
#define TSAL_GREEN -1
#define PRECHG_OUT -1
#define ADC_PIN -1
//
#define ADC_MUX_GLV_VOLT -1
#define ADC_MUX_HV_VOLT -1
#define ADC_MUX_HV_CURRENT -1
#define ADC_MUX_DCDC_CURRENT -1
#define ADC_MUX_DCDC_TEMP1 -1
#define ADC_MUX_DCDC_TEMP2 -1
#define ADC_MUX_DCDC_TEMP3 -1


//Charger Values
#define CHARGER_VOLTAGE 540 //V
#define CHARGER_CURRENT 5 //A

#define PRECHARGE_THRESHOLD -1 //V

#endif