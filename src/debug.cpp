#include "debug.h"
#include "battery.h"

#define printErrs false
#define printRelayState false
#define printWarns false
#define printTSVoltage false
#define printSDCVoltage false
#define printGLVVoltage false
#define printMaxChrgVoltage false
#define printMaxChrgCurrent false
#define printMaxOutputCurrent false
#define printState false
#define printMaxCellTemp false
#define printMaxBalTemp false
#define printMinVolt false
#define printCycle false
#define printChargeCycle false
#define printTempCycle false
#define printAccumCurrent false
#define printAccumCurrentZero false
#define printACUTemp false
#define printCellVoltage false
#define printCellTemp false
#define printBalTemp false

extern ACU acu;
extern Battery battery;
extern uint8_t cycle;

void debug(){
    if(printErrs);
    if(printRelayState);
    if(printWarns);
    if(printTSVoltage);
    if(printSDCVoltage);
    if(printGLVVoltage);
    if(printMaxChrgVoltage);
    if(printMaxChrgCurrent);
    if(printMaxOutputCurrent);
    if(printState);
    if(printMaxCellTemp);
    if(printMaxBalTemp);
    if(printMinVolt);
    if(printCycle);
    if(printChargeCycle);
    if(printTempCycle);
    if(printAccumCurrent);
    if(printAccumCurrentZero);
    if(printACUTemp);
    if(printCycle){
        Serial.printf("----------- Cycle: %d --------------\n", cycle);
    }
    if(printCellVoltage){
        Serial.println("Cell Voltage: --------------------------");
        for(int i = 0; i < TOTAL_IC; i++){
            Serial.printf("Segment %d: ", i);
            for(int j = 0; j < 16; j++){
                Serial.printf("[%3u]%5.01f; ", j, battery.getCellVoltage(i*16 + j));
            }
            Serial.println();
        }
    }
    if(printCellTemp){
        Serial.println("Cell Temp: --------------------------");
        for(int i = 0; i < TOTAL_IC; i++){
            Serial.printf("Segment %d: ", i);
            for(int j = 0; j < 32; j++){
                Serial.printf("[%3u]%5.01f; ", j, battery.getCellTemp(i*16 + j));
            }
            Serial.println();
        }
    }

    if(printBalTemp){
        Serial.println("Bal Temp: --------------------------");
        for(int i = 0; i < TOTAL_IC; i++){
            Serial.printf("Segment %d: ", i);
            for(int j = 0; j < 16; j++){
                Serial.printf("[%3u]%5.01f; ", j, battery.getBalTemp(i*16 + j));
            }
            Serial.println();
        }
    }
    Serial.println("-----------------------End-----------------------");
}