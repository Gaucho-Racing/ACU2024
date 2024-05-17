#ifndef STATES_H
#define STATES_H
#include "battery.h"
#include "ACU.h"

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_prim;
extern FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_256> can_chgr;
extern Battery battery;
extern ACU acu;

enum States {
    PRECHARGE,
    NORMAL,
    CHARGE,
    SHUTDOWN,
    OFFSTATE, 
    STANDBY
};

void shutdownState();
void normalState();
void chargeState();
void preChargeState();
void standByState();


#endif