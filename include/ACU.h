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
    SHUTDOWN
};

struct Battery{
    //supposedly the voltage of each cell is stored in a cell_asic
    // std::vector<std::vector<float>> cellVoltage = std::vector<std::vector<float>>(TOTAL_IC, std::vector<float>(16, 0));
    std::vector<std::vector<std::pair<float,float>>> cellTemp = std::vector<std::vector<std::pair<float,float>>>(TOTAL_IC,std::vector<std::pair<float,float>> (16,std::make_pair(0,0)));
    CANLine can;
    cell_asic IC[TOTAL_IC];
    float maxCellTemp = 0;  
};

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
void shutdownState();
void normalState();
void chargeState();
void preChargeState();
void standByState();

#endif
