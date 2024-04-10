#include "ACU_data.h"
#include "can.cpp"
#include "common.h"
#include <vector>
#include "adBms6830GenericType.h"
#include "ADC1283.h"

enum States {
    FIRST,
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