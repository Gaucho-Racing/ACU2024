#ifndef IMD_H
#define IMD_H
#include <Arduino.h>
#include <unordered_map>

// ID: 0x1CECFFF4
enum IMD_TYPE { GENERAL, 
                ISO_DETAILS, 
                VOLTAGE, 
                IT_SYS, 
                IMD_REQUEST, 
                IMD_RESPONSE }; // still needed or no?

struct IMD_Monitor {

    uint8_t imd_gen[8];    // 0x37; R_iso_corrected,R_iso_status, Iso meas count, Status: warnings & alarms, Status: dev act	N/A or 0xFF
    uint16_t R_iso_corrected;
    uint8_t R_iso_status;
    uint8_t iso_meas_count; 
    uint16_t status_warnings_alarms; // 11 bits long
    uint8_t status_device_activity; //0 - initialization, 1 - Normal Operation, 2 - Self Test 
    float hv_system_voltage;
    // Isolation_Detail >> PGN = 65282
    // Info_General >> PGN = 65281
    // Info_Voltage >> PGN = 65283

    
    // R_ISO_DETAILS
    uint16_t R_iso_positive, R_iso_original;
    uint8_t Iso_meas_count, Isolation_quality;

    // uint8_t imd_volt[8]; // 0x39; HV voltage, HV- to GND voltage, HV+ to GND voltage, Volt meas count
    // uint8_t imd_it[8];     // 0x3A; Capacity value, Cap meas count, Unbalance value, Unbalance meas, Voltage measure frequency	
};

#endif