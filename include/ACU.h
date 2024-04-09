#include "ACU_data.h"
#include "can.h"
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

bool systemCheck(Battery &battery, States &state);
void shutdownState();
void normalState();
void chargeState();
void preChargeState();
void standByState();
