#ifndef _ACU_H
#define _ACU_H

#include <Arduino.h>
#include "adBms_Application.h"
#include "FanController.h"
#include "ADC1283.h"
#include "ACU_data.h"
#include "battery.h"

#define HV_Current_Ref 1.235
#define AIR_NEG 0b00000100
#define AIR_POS 0b00000010
#define Precharge 0b00000001

struct chargerDataStatus {
    bool hardwareStatus;
    bool temperatureStatus;
    bool inputVoltageStatus;
    bool startingState;
    bool communicationState;
};

class ACU{ 
    // chargerDataStatus chargerDataStatus;
    private:
        ADC1283 ACU_ADC = ADC1283(CS_ADC, 4.096, 3200000);
        uint8_t relay_state; // first 5 bits D/C | AIR- | AIR+ | Precharge
        float glv_voltage; 
        float ts_voltage; 
        float ts_current;
        float shdn_volt; 
        float dcdc_current; 
        float DCDC_temp[2]; // DC-DC converter
        float fan_Ref;
        
    public:
        uint8_t errs; // for general 1; OverTemp|OverVolt|OverCurr|BMS|UnderVolt|Precharge|Teensy|UnderTemp
        uint8_t warns; // for general 1; OpenWire|ADBMSADC|CellDrop|HighCurr|LowChrg|CellInbl|Humidity|Hydrogen

        // TODO: fan thingamajigs
        fanController fans = fanController(&Serial8);

        uint16_t fanRpm[4];
        float fanVoltage[4];
        float fanCurrent[4];

        void updateGlvVoltage();
        void updateTsVoltage();
        void updateAccumCurrent();
        void updateShdnVolt();
        void updateDcdcCurrent();
        void updateDcdcTemp1();
        void updateDcdcTemp2();
        void updateFanRef();
        void updateAll();

        
        uint8_t getRelayState();
        float getGlvVoltage();
        float getTsVoltage();
        float getTsCurrent();
        float getShdnVolt();
        float getDcdcCurrent();
        float getDcdcTemp1();
        float getDcdcTemp2();
        float getFanRef();
};

#endif
