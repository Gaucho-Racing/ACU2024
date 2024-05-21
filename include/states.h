#ifndef STATES_H
#define STATES_H
#include "can.h" //includes everything



void shutdownState();
void normalState();
void chargeState();
void preChargeState();
void standByState();

void fullSystemCheck(); //reads all temp data
void standardSysCheck();

#endif