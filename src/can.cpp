#include <Arduino.h>
#include "FlexCAN_T4.h"

//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary; //Depends on what board u test on ig but prob just use this for now
//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary;
//FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> canData;
//CAN_message_t msgPrimary;
//CAN_message_t msg;
//CANFD_message_t msgData;

//#define JuanMbps 1000000
//#define AteMbps 8000000

struct chargerData {
  short maxChargeVolts;
  bool discharging;
  short maxChargeAmps;
  bool hardwareFailure;
  bool temperatureFailure;
  bool inputVoltageFailure;
  bool batteryConnectionFailure;
  bool communicationFailure;  
};

class thing {
  private:
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
    static const int JuanMbps = 1000000;
    static const int AteMbps = 8000000;

  public:
    thing() {
      // Initialize the CAN bus
      can.begin();
      can.setBaudRate(JuanMbps);
      // canData.begin();
      // CANFD_timings_t config;
      // config.clock = CLK_24MHz;
      // config.baudrate = JuanMbps;
      // config.baudrateFD = AteMbps;
      // config.propdelay = 190;
      // config.bus_length = 1;
      // config.sample = 70;
      // canData.setBaudRate(config);
    }

    void send(unsigned int id, byte *things, byte size = 8) {
      CAN_message_t msgSend;
      msgSend.id = id;
      msgSend.len = size;
      for (int i = 0; i < size; i++) {
        msgSend.buf[i] = things[i];
      }
      can.write(msgSend);
      Serial.print("Frame sent to id 0x");
      Serial.println(id, HEX);
    }

    void send(unsigned int id, short *things, byte size = 4) {
      byte msg[8];
      for (int i = 0; i < size/2; i++) {
        msg[2*i] = (byte)(things[i] >> 8);
        msg[2*i+1] = (byte)(things[i]);
      }
      send(id, msg, 2*size);
    }

    byte* recieve(unsigned int id, byte size = 8) {
      CAN_message_t msgRecieve;
      static byte result[8];
      for (int i = 0; i < size; i++) result[i] = 0;
      can.read(msgRecieve);
      Serial.print("0x");
      Serial.println(msgRecieve.id, HEX);
      if (msgRecieve.id != id) {
        Serial.println("Looks like the id is wrong!");
        return result;
      }
      for (int i = 0; i < size; i++) {
        result[i] = msgRecieve.buf[i];
      }
      return result;
    }

    short* recieveShort(unsigned int id, byte size = 4) {
      static byte *msgbyte = recieve(id, size*2);
      static short result[4];
      for (int i = 0; i < 2*size; i+=2) {
        result[i] = (((short)msgbyte[i]) << 8) + (short)msgbyte[i+1];
      }
      return result;
    }

    void sendToCharger(short maxChargeVolts, short maxChargeAmps, bool enableCharge) {
      //msgPrimary.id = 0x1806E5F4;
      //msgPrimary.len = 8;
      short chargeShort = (short)enableCharge << 8;
      short msg[4] = {maxChargeVolts, maxChargeAmps, chargeShort, 0};
      send(0x1806E5F4, msg);
      /*
      msgPrimary.buf[0] = (byte)(maxChargeVolts >> 8);
      msgPrimary.buf[1] = (byte)(maxChargeVolts);
      msgPrimary.buf[2] = (byte)(maxChargeAmps >> 8);
      msgPrimary.buf[3] = (byte)(maxChargeAmps);
      msgPrimary.buf[4] = (byte)(enableCharge);
      msgPrimary.buf[5] = 0x00;
      msgPrimary.buf[6] = 0x00;
      msgPrimary.buf[7] = 0x00;
      canPrimary.write(msgPrimary);
      Serial.println("Frame sent!");
      */
    }

    chargerData recieveCharger() {
      /*
      canPrimary.read(msg);
      Serial.println(msg.id, HEX);
      if (msg.id != 0x18FF50E5) {
        Serial.println("Looks like the id is wrong!");
        return;
      }
      */
      short *msg = recieveShort(0x18FF50E5); //size 4

      chargerData r;

      r.maxChargeVolts = msg[0];
      r.discharging = msg[1] & 0x8000;
      r.maxChargeAmps = msg[1];
      r.hardwareFailure = msg[2] & 0x0100;
      r.temperatureFailure = msg[2] & 0x0200;
      r.inputVoltageFailure = msg[2] & 0x0400;
      r.batteryConnectionFailure = msg[2] & 0x0800;
      r.communicationFailure = msg[2] & 0x1000;

      return r;
    }  
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
