#ifndef _ACU_H
#define _ACU_H

#include "adBms6830Data.h"
#include "adBms6830GenericType.h"
#include <vector>
#include <utility>
#include "ACU_data.h"
#include "can.h"
#include "ADBMS.h"
#include "adBms_Application.h"
#include "ADC1283.h"

enum States {
    PRECHARGE,
    NORMAL,
    CHARGE,
    SHUTDOWN,
    OFFSTATE, 
    STANDBY
};

struct Battery{
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can_prim;
    FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can_chgr;
    CAN_message_t msg;
    
    uint8_t errs; // for general 1
    uint8_t warns; // for general 1
    
    uint8_t air_state; // AIR- | AIR+
    uint16_t ts_voltage; 
    uint8_t sdc_voltage;    
    uint8_t glv_voltage;

    uint8_t max_chrg_voltage;
    uint8_t max_chrg_current;
    

    States state;
    cell_asic *IC;
    float maxCellTemp, maxBalTemp = -1;
    uint16_t minVolt = -1;
    uint8_t cycle = 0;
    //every 10 cycles recheck Voltage
    uint8_t chargeCycle = 0;
    uint8_t temp_cycle = 0;
    uint16_t accumulatorCurrent = 0; 
    //in 0.1mV
    uint16_t cellVoltage[128];  
    float cellTemp[256];
    float balTemp[128];
    bool containsError = false;
    ADC1283 ACU_ADC = ADC1283(ADC_PIN, 4.096, 3200000);
};

// helper functions
void init_config(Battery &battery);
void get_Temperatures(Battery &battery);
void get_Current(Battery &battery);
void get_Max_Cell_Temp(Battery &battery);
void get_Max_Bal_Res_Temp(Battery &battery);
void cell_Balancing(Battery &battery);
void offState(Battery &battery,States& state);
void shutdownState(Battery &battery, States& state);
void normalState(Battery &battery, States& state);
void chargeState(Battery &battery, States& state);
void preChargeState(Battery &battery, States& state);
void standByState(Battery &battery, States& state);
bool systemCheck(Battery &battery, States& state);

// functions for cell data
void updateVoltage(Battery &battery); // parse and copy cell voltage data from ADI's array into our array
float V2T(float voltage, float B = 4390); // calculate NTC thermistor temperature
void updateTemps(Battery &battery); // read cell temperatures
void calcCharge(Battery &battery); // calculate state of charge
void dumpCANbus(Battery &battery); // send condensed cell data to primary CAN
void sendCellVoltageError(Battery &battery, const float thresholdType); // @Rachel explain this
uint8_t condenseVoltage(uint16_t voltage); // calculate condensed cell voltage value
uint16_t getAccumulatorVoltage(uint16_t *cellVoltage); // calculate sum of all cell voltages

#endif
