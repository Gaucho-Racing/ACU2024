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

//Pin Definitions
#define PIN_AIR_POS 19 // assuming closed high
#define PIN_AIR_NEG 20 //assuming closed high
#define PIN_AIR_RESET 21
#define PIN_PRECHG 41 // also controls discharge relay, assuming preC closed high, disc open high
// #define PIN_AIR_POS_AUX -1 // auxilary contacts
// #define PIN_AIR_NEG_AUX -1 // auxilary contacts
#define PIN_AMS_OK 39
#define PIN_DCDC_ER 18 //in the words of owen: "if it's high it's fucked", Buck Bad pin
#define PIN_DCDC_EN 15 // enable DC/DC converter
#define PIN_DCDC_SLOW 16 // DC/DC low power mode
#define PIN_IMD_OK 17 // Isolation monitoring device
#define PIN_BSPD_CLK 14 // PWM_CLK, 50k

#define PRECHARGE_THRESHOLD 0.9
