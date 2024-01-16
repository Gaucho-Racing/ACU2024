/*
ADBMS6822.h - library for ADBMS6822 dual isoSPI adapter board
Created by Yandong Liu 20240115
*/

#include "ADBMS6822_Driver.h"

isoSPI::isoSPI(SPIClass* mySPI, uint8_t csPin, uint8_t intPin, uint8_t wakePin, uint8_t mstrPin, uint8_t rtoPin, uint8_t xModePinA, uint8_t xModePinB, uint8_t sModePinA, uint8_t sModePinB) {
    _mySPI = mySPI;
    _csPin = csPin; //chipselect
    _intPin = intPin; //interrupt
    _wakePin = wakePin; 
    _mstrPin = mstrPin; //master
    _rtoPin = rtoPin;
    _xModePinA = xModePinA; //Tranceiver, random gpio pins that the teensy needs to mess with
    _xModePinB = xModePinB;
    _sModePinA = sModePinA; //SPI mode, random gpio pins that the teensy needs to mess with
    _sModePinB = sModePinB;
}

void isoSPI::setRTO(uint8_t rtoTime) {
    switch (rtoTime){
    case RTO_1s5:
        pinMode(_rtoPin, OUTPUT);
        digitalWrite(_rtoPin, LOW);
        break;

    case RTO_48s:
        pinMode(_rtoPin, INPUT_DISABLE);
        break;
    
    default:
        Serial.println("Invalid RTO setting");
        break;
    }
}

void isoSPI::setXcvrMode(uint8_t mode) {
    _xMode = mode;
    switch (_xMode){
    case XcvrMode_Standard:
        pinMode(_xModePinA, OUTPUT);
        digitalWrite(_xModePinA, LOW);
        pinMode(_xModePinB, OUTPUT);
        digitalWrite(_xModePinB, LOW);
        break;

    case XcvrMode_LPCM:
        pinMode(_xModePinA, INPUT_DISABLE);
        pinMode(_xModePinB, OUTPUT);
        digitalWrite(_xModePinB, LOW);
        break;
    
    case XcvrMode_4M:
        pinMode(_xModePinA, INPUT_DISABLE);
        pinMode(_xModePinB, INPUT_DISABLE);
        break;

    case XcvrMode_2MLate:
        pinMode(_xModePinA, OUTPUT);
        digitalWrite(_xModePinA, HIGH);
        pinMode(_xModePinB, OUTPUT);
        digitalWrite(_xModePinB, HIGH);
        break;
    
    default:
        Serial.println("Invalid transciever mode setting");
        break;
    }
}

void isoSPI::setSpiMode(uint8_t mode) {
    _sMode = mode;
    switch (_sMode){
    case SPI_MODE0:
        pinMode(_sModePinA, OUTPUT);
        digitalWrite(_sModePinA, LOW);
        pinMode(_sModePinB, OUTPUT);
        digitalWrite(_sModePinB, LOW);
        break;

    case SPI_MODE1:
        pinMode(_sModePinA, INPUT_DISABLE);
        pinMode(_sModePinB, OUTPUT);
        digitalWrite(_sModePinB, LOW);
        break;
    
    case SPI_MODE2:
        pinMode(_sModePinA, INPUT_DISABLE);
        pinMode(_sModePinB, INPUT_DISABLE);
        break;

    case SPI_MODE3:
        pinMode(_sModePinA, OUTPUT);
        digitalWrite(_sModePinA, HIGH);
        pinMode(_sModePinB, OUTPUT);
        digitalWrite(_sModePinB, HIGH);
        break;
    
    default:
        Serial.println("Invalid SPI mode setting");
        break;
    }
}

void isoSPI::begin() {
    _mySPI -> begin();
    _mySPI -> setCS(_csPin); // don't know why this doesn't work
    pinMode(_csPin, OUTPUT);
    pinMode(_intPin, INPUT);
    pinMode(_wakePin, INPUT);
    pinMode(_mstrPin, OUTPUT);
    digitalWrite(_csPin, HIGH); // SPI chip select
    digitalWrite(_mstrPin, HIGH); // SPI master mode
    setRTO(RTO_48s); // Default 48s LPCM timeout
    setXcvrMode(XcvrMode_Standard); // Default 2Mbps bidirectional
    setSpiMode(SPI_MODE3); // Default SPI mode 3 for ADBMS battery monitors
}

void isoSPI::beginTransaction(uint8_t spiMode, uint32_t fsck) {
    setSpiMode(spiMode);
    if (_xMode == XcvrMode_4M) { // limit clock rate
        fsck = min(fsck, 4000000);
    }
    else {
        fsck = min(fsck, 2000000);
    }
    _mySPI -> beginTransaction(SPISettings(fsck, MSBFIRST, _sMode));
    digitalWrite(_csPin, LOW);
}

void isoSPI::endTransaction() {
    digitalWrite(_csPin, HIGH);
    _mySPI -> endTransaction();
}

uint8_t isoSPI::transfer(uint8_t sendByte) {
    return _mySPI -> transfer(sendByte);
}

uint16_t isoSPI::transfer16(uint16_t sendWord) {
    return _mySPI -> transfer16(sendWord);
}

uint32_t isoSPI::transfer32(uint32_t sendDWord) {
    return _mySPI -> transfer32(sendDWord);
}

void isoSPI::transfer(uint8_t* buffer[], size_t size) {
    _mySPI -> transfer(buffer, size);
}

bool isoSPI::isAwake() {
    return digitalRead(_wakePin);
}

void isoSPI::setIntFunc(void (*function)(void)) {
    attachInterrupt(digitalPinToInterrupt(_intPin), function, RISING);
}

void isoSPI::setMasterMode(){
    digitalWrite(_mstrPin, HIGH);
}

void isoSPI::setSlaveMode(){
    digitalWrite(_mstrPin, LOW);
}