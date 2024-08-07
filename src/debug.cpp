#include "debug.h"
#include "battery.h"

#define printErrs true
#define printRelayState false
#define printWarns false
#define printTSVoltage true
#define printSDCVoltage true
#define printGLVVoltage true
#define printDcdcCurrent true
#define printTemps true
#define printMaxChrgVoltage true
#define printMaxChrgCurrent true
#define printMaxOutputCurrent false
#define printState true
#define printFanRef false
#define printMaxCellTemp true
#define printMaxBalTemp true
#define printminCellVolt true
#define printCycle false
#define printAccumCurrent true
#define printCurrentZeros true
#define printCellVoltage false
#define printCellTemp false
#define printBalTemp false
#define printIMDData false

extern ACU acu;
extern Battery battery;
extern uint8_t cycle;

extern States state;

void debug(){
    #if 1
    Serial.println("-----------------------Debug-----------------------");
    if(printState) {
        Serial.print("State: ");
        switch (state) {
        case SHUTDOWN:
            Serial.println("Shutdown");
            break;
        case PRECHARGE:
            Serial.println("Precharge");
            break;
        case STANDBY:
            Serial.println("Standby");
            break;
        case NORMAL:
            Serial.println("Normal");
            break;
        case CHARGE:
            Serial.println("Charging");
            break;
        default:
            Serial.println("Unknown");
            break;
        }
    }
    if(printErrs) {
        Serial.print("Errors: ");
        if (acu.errs & ERR_OverTemp) Serial.print("Overtemp ");
        if (acu.errs & ERR_OverVolt) Serial.print("Overvolt ");
        if (acu.errs & ERR_OverCurr) Serial.print("Overcurr ");
        if (acu.errs & ERR_BMS     ) Serial.print("BMS ");
        if (acu.errs & ERR_UndrVolt) Serial.print("Undrvolt ");
        if (acu.errs & ERR_Prechrg ) Serial.print("Prechrg ");
        if (acu.errs & ERR_Teensy  ) Serial.print("Teensy ");
        if (acu.errs & ERR_UndrTemp) Serial.print("Undrtemp ");
        if (!acu.errs) {
            Serial.print("none");
        }
        Serial.println();
    }
    if(printRelayState);
    if(printWarns);
    if(printTSVoltage){
        Serial.printf("TS Voltage: %5.03f\n", acu.getTsVoltage(false));
    }
    if(printSDCVoltage){
        Serial.printf("SDC Voltage: %5.03f\n", acu.getShdnVolt(false));
    }
    if(printGLVVoltage){
        Serial.printf("GLV Voltage: %5.03f\n", acu.getGlvVoltage(false));
    }
    if(printDcdcCurrent){
        Serial.printf("DCDC Current: %5.03f\n", acu.getDcdcCurrent(false));
    }
    if(printTemps){
        Serial.printf("ACU Temps: %5.03f, %5.03f\n", acu.getTemp1(false), acu.getTemp2(false));
    }
    if(printMaxChrgVoltage){
        Serial.printf("Max charge voltage: %4.02f\n", acu.max_chrg_voltage);
    }
    if(printMaxChrgCurrent){
        Serial.printf("Max charge current: %4.02f\n", acu.max_chrg_current);
    }
    if(printMaxOutputCurrent);
    if(printFanRef){
        Serial.printf("5V: %5.03f\n", acu.getFanRef(false));
    }
    if(printMaxCellTemp){
        Serial.printf("Max Cell Temp: %5.03f\n", battery.maxCellTemp);
    }
    if(printMaxBalTemp);
    if(printminCellVolt){
        Serial.printf("Min Cell Volt: %5.03f\n", battery.minCellVolt);
    }
    if(printAccumCurrent){
        Serial.printf("TS current: %5.03f\n", acu.getTsCurrent(false));
    }
    if(printCurrentZeros){
        Serial.printf("TS current ref: %5.05f, DCDC current ref: %5.05f\n", acu.cur_ref, acu.dcdc_ref);
    }
    if(printCycle){
        Serial.printf("----------- Cycle: %d --------------\n", cycle);
    }
    if(printCellVoltage){
        Serial.println("Cell Voltage: --------------------------");
        for(int i = 0; i < TOTAL_IC; i++){
            Serial.printf("Segment %d: ", i);
            for(int j = 0; j < 8; j++){
                Serial.printf("[%2u]%5.03f; ", j, battery.getCellVoltage(i*16 + j));
            }
            //Serial.println();
            for(int j = 8; j < 16; j++){
                Serial.printf("[%2u]%5.03f; ", j, battery.getCellVoltage(i*16 + j));
            }
            Serial.println();
        }
    }
    if(printCellTemp){
        Serial.println("Cell Temp: --------------------------");
        for(int i = 0; i < TOTAL_IC; i++){
            Serial.printf("Segment %d: ", i);
            for(int j = 0; j < 32; j++){
                Serial.printf("[%2u]%5.03f; ", j, battery.getCellTemp(i*16 + j));
            }
            Serial.println();
        }
    }

    if(printBalTemp){
        Serial.println("Bal Temp: --------------------------");
        for(int i = 0; i < TOTAL_IC; i++){
            Serial.printf("Segment %d: ", i);
            for(int j = 0; j < 16; j++){
                Serial.printf("[%2u]%5.01f; ", j, battery.getBalTemp(i*16 + j));
            }
            Serial.println();
        }
    }
    if(printIMDData){
        Serial.println("IMD Data: --------------------------");
        acu.printIso();
        Serial.println();
    }
    Serial.println("-----------------------End-----------------------");
    #endif
}