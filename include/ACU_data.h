#ifndef _ACU_DATA_H
#define _ACU_DATA_H
#include <Arduino.h>
#include <SPI.h>
// CONFIG OPTIONS FOR 6830 ARE IN THE CPP FILE

#define TOTAL_IC 8


//6830 settings



//Pin Definitions
#define AIR_PLUS -1
#define AIR_NEG -1
#define CAN_H -1
#define CAN_L -1
#define TSAL_RED -1
#define TSAL_GREEN -1
#define PRECHG_OUT -1


//Charger Values
#define CHARGER_VOLTAGE 540 //V
#define CHARGER_CURRENT 5 //A

#endif