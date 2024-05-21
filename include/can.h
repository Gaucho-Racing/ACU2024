#ifndef CAN_H
#define CAN_H

#include <FlexCAN_T4.h>
#include "canID.h"
#include "battery.h"
#include "states.h"

extern Battery battery;
extern ACU acu;
extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_prim;
extern FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_256> can_chgr;
extern CAN_message_t msg;
extern States state;

void sendCANData(uint32_t ID);
void parseCANData();
int readCANData();
void dumpCANbus();

#endif