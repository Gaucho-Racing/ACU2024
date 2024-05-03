#ifndef _ACU_H
#define _ACU_H

#define DEBUG 1

#include "adBms6830Data.h"
#include "adBms6830GenericType.h"

#include <vector>
#include <utility>
#include "ACU_data.h"
// #include "can.h"
#include "ADBMS.h"
#include "adBms_Application.h"
#include "FanController.h"
#include "ADC1283.h"
#include <FlexCAN_T4.h>


enum States {
    PRECHARGE,
    NORMAL,
    CHARGE,
    SHUTDOWN,
    OFFSTATE, 
    STANDBY
};
struct chargerDataStatus {
    bool hardwareStatus;
    bool temperatureStatus;
    bool inputVoltageStatus;
    bool startingState;
    bool communicationState;
};

struct Battery{
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_256> can_prim;
    FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_256> can_chgr;
    CAN_message_t msg;
    // chargerDataStatus chargerDataStatus;


    uint8_t errs; // for general 1
    uint8_t warns; // for general 1
    
    uint8_t relay_state; // AIR- | AIR+ | Precharge
    uint16_t ts_voltage; // 10mV/LSB
    uint16_t sdc_voltage; // 4mV/LSB
    uint16_t glv_voltage; // 4mV/LSB

    uint16_t max_chrg_voltage; // 10mV/LSB
    uint16_t max_chrg_current; // 10mA/LSB
    uint16_t max_output_current; // 10mA/LSB
    

    States state;
    cell_asic IC[TOTAL_IC];
    float maxCellTemp, maxBalTemp = -1;
    uint16_t minVolt = -1;
    uint8_t cycle = 0;

    //every 10 cycles recheck Voltage
    uint8_t chargeCycle = 0;
    uint8_t temp_cycle = 0;
    uint32_t prevMillis;
    uint16_t accumCurrent = 0; // 10mA/LSB
    float accumCurrentZero = 1.235; // offset for zeroing accumulator current
    
    //in 0.1mV
    uint16_t cellVoltage[16 * TOTAL_IC]; // 16 * 8
    float cellTemp[16 * 2 * TOTAL_IC]; // 16 * 2 * 8
    float balTemp[16 * TOTAL_IC];
    bool containsError = false;
    ADC1283 ACU_ADC = ADC1283(CS_ADC, 4.096, 3200000);

    
    // fan thingamajigs
    fanController fans = fanController(&Serial8);
    float accumVoltage, tsVoltage;
    float acuTemp[3]; // DC-DC converter, something, something
    uint16_t fanRpm[4];
    float fanVoltage[4];
    float fanCurrent[4];
};

// helper functions
void init_config(Battery &battery);
void get_Temperatures(Battery &battery);
void get_Current(Battery &battery);
void get_Max_Cell_Temp(Battery &battery);
void cell_Balancing(Battery &battery);
void shutdownState(Battery &battery);
void normalState(Battery &battery);
void chargeState(Battery &battery);
void preChargeState(Battery &battery);
void standByState(Battery &battery);
bool systemCheck(Battery &battery);

// functions for cell data
void updateVoltage(Battery &battery); // parse and copy cell voltage data from ADI's array into our array
float V2T(float voltage, float B); // calculate NTC thermistor temperature
void updateTemps(Battery &battery); // update cell/bal temperatures slowly to prevent large lockout times
void updateAllTemps(Battery &battery); // update all cell/bal temperatures at once, used for first cycle as it will give junk values if not initalized
uint8_t calcCharge(Battery &battery); // calculate state of charge --> TODO
void dumpCANbus(Battery &battery); // send EVERYTHING to primary CAN except ping
void readCANWrapper(Battery &battery); // lol, lmao
uint8_t condenseVoltage(uint16_t voltage); // calculate condensed cell voltage value
uint8_t condenseTemperature(float temperature); // calculate condensed cell temperature value
uint16_t getAccumulatorVoltage(Battery &battery); // calculate sum of all cell voltages; TODO: NOT SURE WHAT THIS DOES
uint8_t getAccumulatorTemp(Battery &battery); //TODO: NOT SURE WHAT THIS DOES

#endif
