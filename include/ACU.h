#ifndef _ACU_H
#define _ACU_H

#include "adBms6830Data.h"
#include "adBms6830GenericType.h"
#include <vector>
#include <utility>
#include "ACU_data.h"
#include "can.h"
#include "ADBMS.h"
#include "adBms_Application.h"


enum States {
    STANDBY,
    PRECHARGE,
    NORMAL,
    CHARGE,
    SHUTDOWN,
    OFFSTATE
};

struct Battery{
    CANLine can;
    States state;
    cell_asic *IC;
    float maxCellTemp, maxBalTemp = -1;
    uint8_t cycle = 0;
    uint8_t temp_cycle = 0;
    uint16_t accumulatorCurrent = 0; 
    uint16_t cellVoltage[128];  
    float cellTemp[256];
    float balTemp[128];
    bool containsError = false;
};

// helper functions
void init_config(Battery &battery);
void get_Temperatures(Battery &battery);
void get_Current(Battery &battery);
void get_Max_Cell_Temp(Battery &battery);
void get_Max_Bal_Res_Temp(Battery &battery);
void cell_Balancing(Battery &battery);
void offState(Battery &battery,States& state);
void shutdownState(Battery &battery, States& state);
void normalState(Battery &battery, States& state);
void chargeState(Battery &battery, States& state);
void preChargeState(Battery &battery, States& state);
void standByState(Battery &battery, States& state);
bool systemCheck(Battery &battery, States& state);

// functions for cell Voltage
void updateVoltage(Battery &battery);
float V2T(float voltage, float B = 4390);
void updateTemps(Battery &battery);
void calcCharge(Battery &battery);
void dumpCANbus(CANLine *can, uint16_t cellVoltage[]);
void sendCellVoltageError(Battery &battery, const float thresholdType);
uint8_t condenseVoltage(uint16_t voltage);
uint16_t getAccumulatorVoltage(uint16_t *cellVoltage);

#endif
