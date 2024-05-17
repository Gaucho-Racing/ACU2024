#include <Arduino.h>
#include <SPI.h>

//remove before final build
#define TOTAL_IC 1
//#define TOTAL_IC 24

//Charger Values
#define CHARGER_VOLTAGE 540 //V
#define CHARGER_CURRENT 5 //A

#define CS_ADC 0

#define ADC_MUX_GLV_VOLT 7
#define ADC_MUX_HV_VOLT 6
#define ADC_MUX_HV_CURRENT 0
#define ADC_MUX_SHDN_POW 1
#define ADC_MUX_DCDC_CURRENT 5
#define ADC_MUX_DCDC_TEMP1 2
#define ADC_MUX_DCDC_TEMP2 3
#define ADC_MUX_FAN_REF 4