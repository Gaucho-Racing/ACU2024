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
    SHUTDOWN
};

struct Battery{
    uint16_t cellVoltage[128];
    float cellTemp[128][2];
    float balTemp[128];
    CANLine can;
    cell_asic IC[TOTAL_IC];
    float maxCellTemp, maxBalTemp = 0;
    uint8_t cycle = 0;
};

//LUT for cell to mux channel


void init_config(Battery &battery);
void get_Voltage(Battery &battery);
void get_Temperatures(Battery &battery);
void get_Current(Battery &battery);
void get_Max_Cell_Temp(Battery &battery);
void get_Max_Bal_Res_Temp(Battery &battery);
void cell_Balancing(Battery &battery);
bool systemCheck(Battery &battery, States &state);
void updateVoltage(uint16_t cellVoltage[], cell_asic IC[]);
uint8_t condenseVoltage(uint16_t voltage);
void dumpCANbus(CANLine *can, uint16_t cellVoltage[]);
void shutdownState(States& state, bool systemCheckOk);
void normalState(States& state, bool systemCheckOk);
void chargeState(States& state, bool systemCheckOk);
void preChargeState(States& state, bool systemCheckOk);
void standByState(States& state, bool systemCheckOk);
#endif
