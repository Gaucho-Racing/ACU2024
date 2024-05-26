#ifndef BATTERY_H
#define BATTERY_H

#include "adBms6830Data.h"
#include "adBms6830GenericType.h"
#include "adBms_Application.h"
#include "ACU.h"

extern ACU acu;
extern uint8_t cycle;
class Battery{
    private:
        cell_asic IC[TOTAL_IC];
        //in 0.1mV

        float maxCellTemp, maxBalTemp = -1;
        uint16_t minVolt = -1;

        float cellTemp[16 * 2 * TOTAL_IC]; // 16 * 2 * 8
        float balTemp[16 * TOTAL_IC];
        
        float batVoltage;

        uint16_t max_chrg_voltage; // 10mV/LSB
        uint16_t max_chrg_current; // 10mA/LSB
        uint16_t max_output_current; // 10mA/LSB
    public:
        // Battery();
        float cellVoltage[16 * TOTAL_IC]; // 16 * 8

        void init_config();
        void updateVoltage();
        void updateTemp();
        void updateAllTemps();
        void checkVoltage();
        void checkTemp();
        void checkFuse();
        void checkAllFuse();
        uint8_t calcCharge();
        void cell_Balancing();
        void disable_Mux();
        float getCellVoltage(uint8_t index);
        float getCellTemp(uint8_t index);
        float getBalTemp(uint8_t index);
        float getTotalVoltage();

        void checkBattery(bool fullCheck = false);

    friend void parseCANData();
    friend int readCANData();
    friend void sendCANData(uint32_t ID);
    friend void debug(Battery &battery);

};

uint8_t condenseVoltage(uint16_t voltage); // calculate condensed cell voltage value
uint8_t condenseTemperature(float temperature); // calculate condensed cell temperature value


#endif // BATTERY_H