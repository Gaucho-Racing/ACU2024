/*
  ADC1283.h - Library for reading voltages from ADC1283 ADCs
  Created by Yandong Liu, 20230930
*/

#include "ADC1283.h"

ADC1283::ADC1283() {
  _csPin = 4;
  _vref = 5.0;
  _fsck = 1000000;
  updateSpiSettings();
}

ADC1283::ADC1283(uint8_t pin, float voltage, uint32_t freq) {
  _csPin = pin;
  _vref = voltage;
  _fsck = freq;
  updateSpiSettings();
}

void ADC1283::updateSpiSettings() {
  _mySetting = spi_settings(_fsck, MSBFIRST, SPI_MODE3);
}

void ADC1283::setFsck(uint32_t newFreq) {
  _fsck = newFreq;
  updateSpiSettings();
}

void ADC1283::setCsPin(uint8_t newPin) {
  _csPin = newPin;
}

void ADC1283::setVref(float newVref) {
  _vref = newVref;
}

void ADC1283::begin() {
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  spi.begin();
}

uint16_t ADC1283::readRaw(uint8_t mux) {
  mux = min(mux, 8);
  spi.beginTransaction(_mySetting);
  digitalWrite(_csPin, LOW);
  spi.transfer16(mux << 11);
  uint16_t rcvdData = spi.transfer16(mux << 11);
  digitalWrite(_csPin, HIGH);
  spi.endTransaction();
  return rcvdData;
}

float ADC1283::readVoltage(uint8_t mux) {
  return readRaw(mux) / 4096.0 * _vref;
}

uint16_t* ADC1283::readRaw(uint8_t mux, uint8_t nextMux, uint16_t results[2]) {
  mux = min(mux, 8); nextMux = min(nextMux, 8);
  spi.beginTransaction(_mySetting);
  digitalWrite(_csPin, LOW);
  results[0] = spi.transfer16(mux << 11);
  results[1] = spi.transfer16(nextMux << 11);
  digitalWrite(_csPin, HIGH);
  spi.endTransaction();
  return results;
}

float* ADC1283::readVoltage(uint8_t mux, uint8_t nextMux, float results[2]) {
  uint16_t resultsRaw[2];
  readRaw(mux, nextMux, resultsRaw);
  results[0] = resultsRaw[0] / 4096.0 * _vref;
  results[1] = resultsRaw[1] / 4096.0 * _vref;
  return results;
}

uint16_t ADC1283::readRawLast(uint8_t nextMux) {
  nextMux = min(nextMux, 8);
  spi.beginTransaction(_mySetting);
  digitalWrite(_csPin, LOW);
  uint16_t rcvdData = spi.transfer16(nextMux << 11);
  digitalWrite(_csPin, HIGH);
  spi.endTransaction();
  return rcvdData;
}

float ADC1283::readVoltageLast(uint8_t nextMux) {
  return readRawLast(nextMux) / 4096.0 * _vref;
}

uint16_t ADC1283::readRawTot(uint8_t mux, uint8_t nSample) {
  mux = min(mux, 8);
  nSample = min(nSample, 16);
  uint16_t tot = 0;
  spi.beginTransaction(_mySetting);
  digitalWrite(_csPin, LOW);
  uint16_t sendWord = mux << 11;
  spi.transfer16(sendWord);
  for (uint8_t i = 0; i < nSample; i++) {
    tot += spi.transfer16(sendWord);
  }
  digitalWrite(_csPin, HIGH);
  spi.endTransaction();
  return tot;
}

uint32_t ADC1283::readRawTotLong(uint8_t mux, uint16_t nSample) {
  mux = min(mux, 8);
  uint32_t tot = 0;
  spi.beginTransaction(_mySetting);
  digitalWrite(_csPin, LOW);
  uint16_t sendWord = mux << 11;
  spi.transfer16(sendWord);
  for (uint16_t i = 0; i < nSample; i++) {
    tot += spi.transfer16(sendWord);
  }
  digitalWrite(_csPin, HIGH);
  spi.endTransaction();
  return tot;
}

float ADC1283::readVoltageTot(uint8_t mux, uint16_t nSample) {
  if (nSample > 16) {
    return readRawTotLong(mux, nSample) / 4096.0 / nSample * _vref;
  }
  else {
    return readRawTot(mux, nSample) / 4096.0 / nSample * _vref;
  }
}
