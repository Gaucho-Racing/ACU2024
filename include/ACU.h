#include "ACU_data.h"
#include "can.cpp"
#include "common.h"
#include <vector>
#include "adBms6830GenericType.h"

enum States {
    STANDBY,
    PRECHARGE,
    NORMAL,
    CHARGE,
    SHUTDOWN
};

struct Battery
{
    cell_asic IC[TOTAL_IC];
    CANLine can;
};

void updateVoltage(uint16_t cellVoltage[], cell_asic IC[]);
void sendCan();

bool systemCheck(Battery &battery, States &state);
void shutdownState();
void normalState();
void chargeState();
void preChargeState();
void standByState();
