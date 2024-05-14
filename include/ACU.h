#ifndef _ACU_H
#define _ACU_H

#define DEBUG 1


// #include "can.h"
#include "adBms_Application.h"
#include "FanController.h"
#include "ADC1283.h"
#include "ACU_data.h"




struct chargerDataStatus {
    bool hardwareStatus;
    bool temperatureStatus;
    bool inputVoltageStatus;
    bool startingState;
    bool communicationState;
};

struct ACU{
    
    // chargerDataStatus chargerDataStatus;

    uint8_t errs; // for general 1
    uint8_t warns; // for general 1
    
   
    uint8_t relay_state; // AIR- | AIR+ | Precharge
    uint16_t ts_voltage; // 10mV/LSB
    uint16_t sdc_voltage; // 4mV/LSB
    uint16_t glv_voltage; // 4mV/LSB

    
    
    uint8_t cycle = 0;

    //every 10 cycles recheck Voltage
    uint8_t chargeCycle = 0;
    uint8_t temp_cycle = 0;
    uint32_t prevMillis;
    uint16_t accumCurrent = 0; // 10mA/LSB
    float accumCurrentZero = 1.235; // offset for zeroing accumulator current
    
   
    bool containsError = false;
    ADC1283 ACU_ADC = ADC1283(CS_ADC, 4.096, 3200000);

    
    // fan thingamajigs
    fanController fans = fanController(&Serial8);
    float accumVoltage, tsVoltage;
    float acuTemp[3]; // DC-DC converter, something, something
    uint16_t fanRpm[4];
    float fanVoltage[4];
    float fanCurrent[4];
};

// helper functions
void init_config(Battery &battery);
void get_Temperatures(Battery &battery);
void get_Current(Battery &battery);
void get_Max_Cell_Temp(Battery &battery);

bool systemCheck(Battery &battery);

void dumpCANbus(Battery &battery); // send EVERYTHING to primary CAN except ping
void readCANWrapper(Battery &battery); // lol, lmao

uint16_t getAccumulatorVoltage(Battery &battery); // calculate sum of all cell voltages; TODO: NOT SURE WHAT THIS DOES
uint8_t getAccumulatorTemp(Battery &battery); //TODO: NOT SURE WHAT THIS DOES

#endif
