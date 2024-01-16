#include <Arduino.h>
#include <SPI.h>

void transfer(){
    //configure SPI:
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    //transfer data:
    SPI.transfer(0x00);
    //end transaction:
    SPI.endTransaction();
}