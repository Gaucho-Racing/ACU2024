#ifndef BATTERY_H
#define BATTERY_H

#include "adBms6830Data.h"
#include "adBms6830GenericType.h"
#include "adBms_Application.h"
#include "ACU.h"

extern ACU acu;
class Battery{
    private:
        cell_asic IC[TOTAL_IC];
        //in 0.1mV
        

        float maxCellTemp, maxBalTemp = -1;
        uint16_t minVolt = -1;

        float cellVoltage[16 * TOTAL_IC]; // 16 * 8
        float cellTemp[16 * 2 * TOTAL_IC]; // 16 * 2 * 8
        float balTemp[16 * TOTAL_IC];
        
        float batVoltage;

        uint16_t max_chrg_voltage; // 10mV/LSB
        uint16_t max_chrg_current; // 10mA/LSB
        uint16_t max_output_current; // 10mA/LSB
    public:
        // Battery();
        void init_config();
        void updateVoltage();
        void checkVoltage(uint8_t &errs);
        void updateTemp(uint8_t cycle);
        void checkTemp(uint8_t &errs);
        void updateAllTemps();
        void checkFuse();
        uint8_t calcCharge();
        void cell_Balancing();
        float getCellTemp(uint8_t index);
        float getBalTemp(uint8_t index);
        float getBatVoltage();
        float getTotalVoltage();

        void checkBattery(uint8_t &errs);

    friend void parseCANData();
    friend int readCANData();
    friend void sendCANData(uint32_t ID);
    friend void debug(Battery &battery);

};

uint8_t condenseVoltage(uint16_t voltage); // calculate condensed cell voltage value
uint8_t condenseTemperature(float temperature); // calculate condensed cell temperature value


#endif // BATTERY_H