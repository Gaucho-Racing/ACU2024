/*
    FanController.h - Library for interfacing the GR fan controller
    Created by Yandong Liu 20240130
*/

#include "FanController.h"

fanController::fanController(HardwareSerial *mySerial) {
    _mySerial = mySerial;
}

void fanController::begin() {
    _mySerial -> begin(1000000);
}

// If return value > 255, read error
uint16_t fanController::readRegister(uint8_t addr) {
    while (_mySerial -> available()) { // clear buffer
        _mySerial -> read();
    }
    _mySerial -> write(addr & 0b01111111); // set R/W flag bit to 0 (read)
    uint32_t startTime = micros();
    while (!(_mySerial -> available())) {
        if (micros() - startTime > 1000) return 0xffff; // error
    }
    return _mySerial -> read();
}

bool fanController::writeRegister(uint8_t addr, uint8_t value) {
    _mySerial -> write(addr | 0b10000000); // set R/W flag bit to 1 (write)
    _mySerial -> write(value);
    return value == readRegister(addr);;
}