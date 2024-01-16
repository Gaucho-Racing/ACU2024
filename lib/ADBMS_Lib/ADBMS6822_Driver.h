/*
ADBMS6822.h - library for ADBMS6822 dual isoSPI adapter board
Created by Yandong Liu 20240115
For use with Teensy 4.1 with hardware CS pins only
*/

#ifndef ADBMS6822_H
#define ADBMS6822_H

// Transciever modes
#define XcvrMode_Standard 0
#define XcvrMode_LPCM 1
#define XcvrMode_4M 2
#define XcvrMode_2MLate 3

// LCPM timeouts
#define RTO_1s5 21
#define RTO_48s 248

#include <Arduino.h>
#include <SPI.h>

class isoSPI {
    public:
        //isoSPI();
        isoSPI(SPIClass* mySPI, uint8_t csPin, uint8_t intPin, uint8_t wakePin, uint8_t mstrPin, uint8_t rtoPin, uint8_t xModePinA, uint8_t xModePinB, uint8_t sModePinA, uint8_t sModePinB);
        void begin();

        // SPI stuff
        void beginTransaction(uint8_t spiMode, uint32_t fsck);
        uint8_t transfer(uint8_t sendByte);
        void transfer(uint8_t* buffer[], size_t size);
        uint16_t transfer16(uint16_t sendWord);
        uint32_t transfer32(uint32_t sendDWord);
        void endTransaction();

        // Config stuff
        bool isAwake();
        void setIntFunc(void (*function)(void));
        void setXcvrMode(uint8_t mode);
        void setRTO(uint8_t rtoTime);

        // Functions that the chip supports but we don't need: 
            // Master mode (MSTR = HIGH)
        void setMasterMode();
            // Slave mode (MSTR = LOW)
        void setSlaveMode();
            // WAKE pin in slave mode to alert master controller (Pg. 22)
            // Other LPCM timeout(RTO) lengths (Pg. 4, no hardware support on board)

    private:
        // Pins
        SPIClass* _mySPI;
        uint8_t _csPin;
        uint8_t _intPin;
        uint8_t _wakePin;
        uint8_t _mstrPin;
        uint8_t _rtoPin;
        uint8_t _xModePinA;
        uint8_t _xModePinB;
        uint8_t _sModePinA;
        uint8_t _sModePinB;

        // Configs
        uint8_t _xMode;
        uint8_t _sMode;
        void setSpiMode(uint8_t mode);
};

#endif