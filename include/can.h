#ifndef CAN_H
#define CAN_H

// #include <Arduino.h>
#include "FlexCAN_T4.h"
//#include <vector>
//#include <unordered_map>

typedef uint8_t byte;

//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary; //Depends on what board u test on ig but prob just use this for now
//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary;
//FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> canData;
//CAN_message_t msgPrimary;
//CAN_message_t msg;
//CANFD_message_t msgData;

//#define JuanMbps 1000000
//#define AteMbps 8000000

class CANData {
  public:
    int id;
    byte msg[8];
    bool isToCharger;
    CANData(int id, bool isToCharger = false) {
      this->id = id;
      for (int i = 0; i < 8; i++) {
        this->msg[i] = 0;
      }
      this->isToCharger = isToCharger;
    }
};

class CANLine {
  private: 
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> vdm_can;
    FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> charger_can;
    CAN_message_t msgRecieve, msgSend;
    bool vdm_recieve_ignore_flags[2] = {0,0};

  public:
    CANLine() {
      this->vdm_can = FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16>();
      this->charger_can = FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16>();
    }

    byte high(uint16_t x) { return (uint8_t)(x>>8);}
    byte low(uint16_t x) { return (uint8_t)(x);}

    bool getBit(uint8_t x, int bitnum) {
      x = x >> (7 - bitnum);
      return (bool)(x & 1);
    }

    uint16_t toShort(uint8_t x, uint8_t y = 0) { return (uint16_t)(x<<8) + (uint16_t)(y); }

    int vdm_can_recieve(bool set_ignore_flags = false) {
      if (vdm_can.readFIFO(msgRecieve) == 0) return 0;
      //  byte msg[8];
      //  for (int i = 0; i < 8; i++) msg[i] = msgRecieve.buf[i];
      CANData *node;
      switch (msgRecieve.id) {
        case 0x64:
          node = &configCellData;
          break;
        case 0x66:
          if (vdm_recieve_ignore_flags[0]) break;
          node = &ACUControl;
          if (set_ignore_flags) vdm_recieve_ignore_flags[0] = 1;
          break;
        case 0x67:
          if (vdm_recieve_ignore_flags[1]) break;
          node = &batteryLimits;
          if (set_ignore_flags) vdm_recieve_ignore_flags[1] = 1;
          break;
        case 0x95:
          msgRecieve.id = 0xC7; // ACU to all ping send ID
          vdm_can.write(msgRecieve);
          break;
          //send()
        default:
          return -1;
      }
      for (int i = 0; i < 8; i++) (*node).msg[i] = msgRecieve.buf[i];
      return 1;
    }

    int charger_can_recieve() {
      if (charger_can.readFIFO(msgRecieve) == 0) return 0;
      //  byte msg[8];
      //  for (int i = 0; i < 8; i++) msg[i] = msgRecieve.buf[i];
      CANData *node;
      switch (msgRecieve.id) {
        case 0x18FF50E5:
          node = &chargerData;
          break;
        default:
          return -1;
      }
      for (int i = 0; i < 8; i++) (*node).msg[i] = msgRecieve.buf[i];
      return 1;    
    }

    int vdm_can_update() {
      /* pull up to 32 msgs from vdm queue */
      int ct = 0;
      for (int i = 0; i < 32 && vdm_can_recieve(true); i++) {ct = 1;}
      vdm_recieve_ignore_flags[0] = 0;
      vdm_recieve_ignore_flags[1] = 0;
      return ct;
    }
    int charger_can_update() {
      /* pull up to 32 msgs from charger queue */
      int ct = 0;
      for (int i = 0; i < 1 /* only one id, either there is a most recent message, or there isn't */ && charger_can_recieve(); i++) ct = 1;
      return ct;
     }

    CANData send(CANData d) {
      msgSend.id = d.id;
      for (int i = 0; i < 8; i++) {
        //Serial.print(message[i]);
        //Serial.print(" ");
        msgSend.buf[i] = d.msg[i];
        
      }
      //Serial.println();
      if (d.id == 0x1806E5F4) charger_can.write(msgSend);
      else vdm_can.write(msgSend);
      //Serial.print("Frame sent to id 0x");
      //Serial.println(id, HEX);
    }

    /*
    recieve id: 0x64
    byte0: cell number
        1: data type (bool)
        2 periodic
    */
    CANData configCellData = CANData(0x64);

    /*
    recieve id: 0x66
    byte0: ts active? (bool)
    */
    CANData ACUControl = CANData(0x66);

    /*
    recieve id: 0x67
    byte0-1: max voltage
    2-3: max out current
    4-5: max temp
    */
    CANData batteryLimits = CANData(0x67);

    /*
    send id: 0x95
    byte0-3: time millisecs.
    4-7: time millisecs.
    */
    CANData reqPingRequest = CANData(0x95);

    /*
    send id: 0x96
    byte0-1: accumualtor voltage (V), 0.01× scale
    2-3: accumulator current (A), 0.01× scale
    4-5: max cell temp (°C), 0.01× scale, signed
    6: errors (bools)
    7: warnings (bools)
    */
    CANData ACUGeneral1 = CANData(0x96);

    /*
    send id: 0x97
    byte0-1: ts voltage (V), 0.01× scale
    2: states (bools)
    3-4: max bal resistor temp
    5: sdc Voltage
    6: glv Voltage
    7: state of charge
    */
    CANData ACUGeneral2 = CANData(0x97);

    /*
    send id: 0x98
    byte 0: fan 1 speed (%), 0.5× scale
    1: fan 2 speed (%), 0.5× scale
    2: fan 3 speed (%), 0.5× scale
    3: pump speed (%), 0.5× scale
    4: acu temp 1
    5: acu temp 2
    6: acu temp 3
    7: errors (bools)
    */
    CANData powertrainCooling = CANData(0x98);

    /*
    recieve id: 0x99
    byte0-1: max charge current (A), 0.01× scale
    2-3: max charge voltage (V), 0.01× scale
    4: states (bools)
    5: errors (?)
    */
    CANData chargeCartConfig = CANData(0x99);

    /*
    recieve id: 0x18FF50E5
    byte0-1: output voltage (V), 0.1× scale
    byte2-3: output current (A), 0.1× scale
    byte4: status (bools)
    */
    CANData chargerData = CANData(0x18FF50E5, true);

    /*
    send id: 0x1806E5F4
    byte0-1: requested voltage (V), 0.1× scale
    byte2-3: requested current (A), 0.1× scale
    byte4: charge control (bool)
    */
    CANData chargerControl = CANData(0x1806E5F4, true);

    /*
    stores all the condensed cell data, voltage and temp. the number corresponds to the first cell
    */
    struct CellData {
      CANData CCV0 = CANData(0xA1);
      CANData CCV8 = CANData(0xA2);
      CANData CCV16 = CANData(0xA3);
      CANData CCV24 = CANData(0xA4);
      CANData CCV32 = CANData(0xA5);
      CANData CCV40 = CANData(0xA6);
      CANData CCV48 = CANData(0xA7);
      CANData CCV56 = CANData(0xA8);
      CANData CCV64 = CANData(0xA9);
      CANData CCV72 = CANData(0xAA);
      CANData CCV80 = CANData(0xAB);
      CANData CCV88 = CANData(0xAC);
      CANData CCV96 = CANData(0xAD);
      CANData CCV104 = CANData(0xAE);
      CANData CCV112 = CANData(0xAF);
      CANData CCV120 = CANData(0xB0);

      CANData CCT0 = CANData(0xB3);
      CANData CCT8 = CANData(0xB4);
      CANData CCT16 = CANData(0xB5);
      CANData CCT24 = CANData(0xB6);
      CANData CCT32 = CANData(0xB7);
      CANData CCT40 = CANData(0xB8);
      CANData CCT48 = CANData(0xB9);
      CANData CCT56 = CANData(0xBA);
      CANData CCT64 = CANData(0xBB);
      CANData CCT72 = CANData(0xBC);
      CANData CCT80 = CANData(0xBD);
      CANData CCT88 = CANData(0xBE);
      CANData CCT96 = CANData(0xBF);
      CANData CCT104 = CANData(0xC0);
      CANData CCT112 = CANData(0xC1);
      CANData CCT120 = CANData(0xC2);
    } CondensedCellData;
};

/*
  delay(1000);  // Adjust the delay according to your needs
  APPS1 += 1000;
  if(APPS1 > 10000){
    APPS1 = 0;
  }
  APPS2 += 1000;
  if(APPS2 > 10000){
    APPS2 = 0;
  }
  suspensionPos += 5;
  wheelSpeed += 69;
  tirePressure += 1;
}
*/

#endif
