/*
    FanController.h - Library for interfacing the GR fan controller
    Created by Yandong Liu 20240130
*/

#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include <Arduino.h>

#define FAN_SPD1_addr 0x10 //Speed for fan 1, data only take 8 bit, but later math uses 16 bit
// Voltage mode: 0 = off, 1~255 = 5.015625~12.1640625V
// RPM mode: 50RPM / LSB, 0~12750RPM
// PWM mode: 0 = off, 1~255 = 0~100% duty cycle
#define FAN_ISNS1_addr 0x40 //Current for fan 1, 50mA / LSB
#define FAN_VSNS1_addr 0x44 //Voltage for fan 1, 50mV / LSB
#define FAN_RPM1_addr 0x48 //RPM for fan 1, 50RPM / LSB

#define FAN_SPD2_addr 0x11
#define FAN_ISNS2_addr 0x41
#define FAN_VSNS2_addr 0x45
#define FAN_RPM2_addr 0x49

#define FAN_SPD3_addr 0x12
#define FAN_ISNS3_addr 0x42
#define FAN_VSNS3_addr 0x46
#define FAN_RPM3_addr 0x4A // not used

#define FAN_SPD4_addr 0x13
#define FAN_ISNS4_addr 0x43
#define FAN_VSNS4_addr 0x47
#define FAN_RPM4_addr 0x4B

#define FAN_MODE_addr 0x2A
// FAN1[1:0] | FAN2[1:0] | FAN3[1:0] | FAN4[1:0]
// 00=voltage mode; 01=reserved; 10=rpm mode; 11=PWM mode
// fan 3 (pump) only supports voltage mode, mode[3:2] has no effect

#define FAN_ERRS_addr 0x5A //Error states
// ERR1[1:0] | ERR2[1:0] | ERR3[1:0] | ERR4[1:0]
// ERRx[0] = over current; ERRx[1] = voltage mismatch

class fanController{
    public:
        fanController(HardwareSerial *mySerial);
        void begin();
        bool writeRegister(uint8_t addr, uint8_t value);
        uint16_t readRegister(uint8_t addr);
    private:
        HardwareSerial *_mySerial;
};

#endif