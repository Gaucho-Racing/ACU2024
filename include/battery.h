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
        float minCellVolt = -1;

        float cellTemp[16 * 2 * TOTAL_IC]; // 16 * 2 * 8
        float balTemp[16 * TOTAL_IC];
        uint8_t cellTempErr[16 * TOTAL_IC]; // number of consecutive error frames
        uint8_t cellVoltErr[16 * TOTAL_IC]; // number of consecutive error frames
        
        float batVoltage, batSOC;

        float cell_OT_Threshold = 60;
        float cell_UT_Threshold = 0;
        
        float max_output_current;
    public:
        // Battery();
        float max_chrg_voltage;
        float max_chrg_current;
        float cellVoltage[16 * TOTAL_IC]; // 16 * 8
        float maxCellVolt = -1;
        void init_config();
        void updateVoltage();
        void updateTemp();
        void updateAllTemps();
        void checkVoltage();
        void checkTemp();
        void checkFuse();
        void checkAllFuse();
        void cell_Balancing();
        void disable_Mux();
        float getCellVoltage(uint8_t index);
        float getCellTemp(uint8_t index);
        float getBalTemp(uint8_t index);
        float getTotalVoltage();
        float updateSOC();
        void checkBattery(bool fullCheck = false);
        void resetDischarge();

    friend void parseCANData();
    friend int readCANData();
    friend void sendCANData(uint32_t ID);
    friend void debug();

};

uint8_t condenseVoltage(float voltage); // calculate condensed cell voltage value
uint8_t condenseTemperature(float temp1, float temp2); // calculate condensed cell temperature value


#endif // BATTERY_H