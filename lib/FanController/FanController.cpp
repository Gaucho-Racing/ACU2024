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

uint8_t fanController::readRegister(uint8_t addr) {
    while (_mySerial -> available()) { // clear buffer
        _mySerial -> read();
    }
    addr &= 0b01111111; // set R/W flag bit to 0 (read)
    _mySerial -> write(addr);
    uint32_t startTime = micros();
    while (!(_mySerial -> available())) {
        if (micros() - startTime > 1000) return 0xff; // error
    }
    return _mySerial -> read();
}

bool fanController::writeRegister(uint8_t addr, uint8_t value) {
    addr |= 0b10000000; // set R/W flag bit to 1 (write)
    _mySerial -> write(addr);
    _mySerial -> write(value);
    return readRegister(addr) == value;
}