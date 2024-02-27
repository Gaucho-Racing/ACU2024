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

public bool systemCheck(Battery &battery, States &state);
public void shutdownState();
public void normalState();
public void chargeState();
public void preChargeState();
public void standByState();
