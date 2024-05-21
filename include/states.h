#ifndef STATES_H
#define STATES_H
#include "can.h" //includes everything



void shutdownState();
void normalState();
void chargeState();
void preChargeState();
void standByState();

bool SystemCheck(bool fullCheck = false); //reads all temp data


#endif