/*
  ADC1283.h - Library for reading voltages from MCP3x0x ADCs
  Created by Yandong Liu, 20230930
*/

#ifndef ADC1283_H
#define ADC1283_H

#include <Arduino.h>

// #define USE_SPI0 1
#define USE_SPI1 1
#include <SPI.h>
#define spi_settings SPISettings

#ifdef USE_SPI0
#define spi SPI
#elif USE_SPI1
#define spi SPI1
#endif

#define CS_ADC 0

class ADC1283 {
  public:
    ADC1283();
    ADC1283(uint8_t pin, float voltage, uint32_t freq);
    void begin();
    void setFsck(uint32_t newFreq);
    void setVref(float newVref);
    void setCsPin(uint8_t newPin);
    float readVoltage(uint8_t mux);
    float* readVoltage(uint8_t mux, uint8_t nextMux, float results[2]);
    float readVoltageLast(uint8_t nextMux);
    float readVoltageTot(uint8_t mux, uint16_t nSample);
    uint16_t readRaw(uint8_t mux);
    uint16_t* readRaw(uint8_t mux, uint8_t nextMux, uint16_t results[2]);
    uint16_t readRawLast(uint8_t nextMux);
    uint16_t readRawTot(uint8_t mux, uint8_t nSample);
    uint32_t readRawTotLong(uint8_t mux, uint16_t nSample);

  protected:
    uint8_t _csPin = CS_ADC;
    float _vref = 5.0;
    uint32_t _fsck = 3200000;
    spi_settings _mySetting = spi_settings(_fsck, MSBFIRST, SPI_MODE3);
    void updateSpiSettings();
};

#endif
