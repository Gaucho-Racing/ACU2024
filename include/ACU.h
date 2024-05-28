#ifndef _ACU_H
#define _ACU_H

#include "FanController.h"
#include "ADC1283.h"
#include "ACU_data.h"
#include "IMD.h"
#include "debug.h"


//included here because can and states have a circular dependency if States is defined in states.h
enum States {
    PRECHARGE,
    NORMAL,
    CHARGE,
    SHUTDOWN,
    STANDBY
};

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
        void updateGlvVoltage();
        void updateTsVoltage();
        void updateTsCurrent();
        void updateShdnVolt();
        void updateDcdcCurrent();
        void updateDcdcTemp1();
        void updateDcdcTemp2();
        void updateFanRef();
        void updateRelayState();
        void updateAll();

        uint8_t relay_state; // | Reserved | Reserved | Reserved | Reserved | Reserved | AIR- | AIR+ | Pre_charge
        float glv_voltage; 
        float ts_voltage; 
        float ts_current;
        float shdn_volt; 
        float dcdc_current; 
        //TRIAGE 4: consider if we want to do some sort of "sanity" checking for the temps
        float DCDC_temp[2]; // DC-DC converter, given in volts, need to call V2T to convert to temp
        float fan_Ref;

        float cur_ref = 0;
        
        float max_output_current = MAX_HV_CURRENT;
        float max_temp = MAX_DCDC_TEMP;
        //TRIAGE 1.5 set default values for these
        float max_chrg_voltage; 
        float max_chrg_current; 

        IMD_Monitor IMD;   // IMD MONITOR
        
    public:
        ADC1283 ACU_ADC = ADC1283(CS_ADC, 4.096, 1000000);
        uint8_t errs; // for general 1; OverTemp|OverVolt|OverCurr|BMS|UnderVolt|Precharge|Teensy|UnderTemp
        uint8_t warns; // for general 1; OpenWire|ADBMSADC|CellDrop|HighCurr|LowChrg|CellInbl|Humidity|Hydrogen

        // TRIAGE 2: fan thingamajigs
        fanController fans = fanController(&Serial8);

        uint16_t fanRpm[4];
        float fanVoltage[4];
        float fanCurrent[4];

        void init_config();

        void checkACU(bool startup = false);

        void setMaxChrgVoltage(float voltage);
        void setMaxChrgCurrent(float current);
        void setMaxOutputCurrent(float current);
        void setMaxTemp(float temp);

        uint8_t getRelayState();
        float getGlvVoltage(bool update = true);
        float getTsVoltage(bool update = true);
        float getTsCurrent(bool update = true);
        float getShdnVolt(bool update = true);
        float getDcdcCurrent(bool update = true);
        float getDcdcTemp1(bool update = true);
        float getDcdcTemp2(bool update = true);
        float getFanRef(bool update = true);

        void printIso();
        // uint16_t getIsoResistor();
        // uint16_t getIsoPos(); // retrieves positive resistance value
        // uint16_t getIsoNeg(); //^ neg
        void setIMDHV(float voltage);
        void setRIsoCorrected(uint16_t resistance);
        void setRIsoStatus(uint8_t mesurement);
        void setIsoMeasCount(uint8_t count);
        void setStatusWarningsAlarms(uint16_t status);
        void setStatusDeviceActivity(uint8_t activity);
        bool setRelayState(uint8_t relayState);
        void resetLatch();

        IMD_Monitor* getIMD();

        friend class Battery;
        friend void debug();
        };

float V2T(float voltage, float B = 4390);
float V2T(int16_t voltage, float B = 4390);

#endif
