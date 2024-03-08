#ifndef _ACU_H
#define _ACU_H

#include <vector>
#include <utility>
#include "ACU_data.h"
#include "can.cpp"
#include "adBms6830GenericType.h"

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
    uint16_t maxCellTemp = 0;  
    uint16_t accumulatorCurrent = 0; 
    uint16_t cellVoltage[128];  
    float cellTemp[128][2];
    float balTemp[128];
};

// helper functions
void init_config(Battery &battery);
void get_Temperatures(Battery &battery);
void get_Current(Battery &battery);
void get_Max_Cell_Temp(Battery &battery);
void get_Max_Bal_Res_Temp(Battery &battery);
void cell_Balancing(Battery &battery);

void systemCheck(Battery &battery);

// functions for cell Voltage
void updateVoltage(Battery &battery);
void dumpCANbus(CANLine *can, uint16_t cellVoltage[]);
void sendCellVoltageError(Battery &battery, const float thresholdType);
uint8_t condenseVoltage(uint16_t voltage);
uint16_t getAccumulatorVoltage(uint16_t *cellVoltage);


// state functions
void shutdownState(Battery &battery);
void normalState(Battery &battery);
void chargeState(Battery &battery);
void preChargeState(Battery &battery);
void standByState(Battery &battery);
void offState(Battery &battery);

#endif
