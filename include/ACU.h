#ifndef _ACU_H
#define _ACU_H

#include "adBms6830Data.h"
#include "adBms6830GenericType.h"
#include <vector>
#include <utility>
#include "ACU_data.h"
#include "can.cpp"
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
    cell_asic *IC;
    chargerData cData;
    uint16_t maxCellTemp, maxBalTemp = 0;
    uint8_t cycle = 0;
    uint16_t accumulatorCurrent = 0; 
    uint16_t cellVoltage[128];  
    float cellTemp[128][2];
    float balTemp[128];
    bool containsError = false;
    long previousMillis = 0, interval = 500;
};

// helper functions
void init_config(Battery &battery);
short getMaxVoltage(CANLine *can);
short getMaxOutputCurrent(CANLine *can);

void offState(Battery &battery,States& state, bool systemCheckOk);
void shutdownState(Battery &battery, States& state, bool systemCheckOk, bool &tsActive);
void normalState(Battery &battery, States& state, bool systemCheckOk);
void chargeState(Battery &battery, States& state, bool systemCheckOk);
void preChargeState(Battery &battery, States& state, bool systemCheckOk);
void standByState(Battery &battery, States& state, bool systemCheckOk);
bool systemCheck(Battery &battery, States& state);

// not implemented yet
void get_Temperatures(Battery &battery);
void get_Current(Battery &battery);
void get_Max_Cell_Temp(Battery &battery);
void get_Max_Bal_Res_Temp(Battery &battery);
void cell_Balancing(Battery &battery);

// functions for cell Voltage
void updateVoltage(Battery &battery);
void dumpCANbus(CANLine *can, uint16_t cellVoltage[]);
void sendCellVoltageError(Battery &battery, const float thresholdType);
uint8_t condenseVoltage(uint16_t voltage);
uint16_t getAccumulatorVoltage(uint16_t *cellVoltage);

#endif
