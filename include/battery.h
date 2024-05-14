#ifndef BATTERY_H
#define BATTERY_H

#include "adBms6830Data.h"
#include "adBms6830GenericType.h"
#include "ACU_data.h"
#include "adBms_Application.h"

struct Battery{
    cell_asic IC[TOTAL_IC];
    //in 0.1mV
    uint16_t cellVoltage[16 * TOTAL_IC]; // 16 * 8
    float cellTemp[16 * 2 * TOTAL_IC]; // 16 * 2 * 8
    float balTemp[16 * TOTAL_IC];

    float maxCellTemp, maxBalTemp = -1;
    uint16_t minVolt = -1;
    
    uint16_t max_chrg_voltage; // 10mV/LSB
    uint16_t max_chrg_current; // 10mA/LSB
    uint16_t max_output_current; // 10mA/LSB
};

void init_config(Battery &battery);

void updateVoltage(Battery &battery);
bool checkVoltage(Battery &battery);

float V2T(float voltage, float B); // calculate NTC thermistor temperature, completely independent from update/check Voltage
void updateTemp(Battery &battery);
bool checkTemp(Battery &battery);
void updateAllTemps(Battery &battery);

bool checkFuse(Battery &battery);

uint8_t condenseVoltage(uint16_t voltage); // calculate condensed cell voltage value
uint8_t condenseTemperature(float temperature); // calculate condensed cell temperature value
uint8_t calcCharge(Battery &battery); // calculate state of charge --> TODO

void cell_Balancing(Battery &battery);

#endif // BATTERY_H