/*
    FanController.h - Library for interfacing the GR fan controller
    Created by Yandong Liu 20240130
*/

#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include <Arduino.h>

class fanController{
    public:
        fanController(HardwareSerial *mySerial);
        void begin();
        bool writeRegister(uint8_t addr, uint8_t value);
        uint8_t readRegister(uint8_t addr);
    private:
        HardwareSerial *_mySerial;
};

#endif