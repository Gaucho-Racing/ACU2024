#ifndef CAN_H
#define CAN_H

// #include <Arduino.h>
#include "FlexCAN_T4.h"
#include <vector>
#include <unordered_map>

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

struct batteryData {
  short maxChargeVolts;
  short maxChargeAmps;
  bool charging;
};

class CANLine {
  private:
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
    CAN_message_t msgRecieve, msgSend;
    static const int JuanMbps = 250000;
    //static const int AteMbps = 8000000;
    std::unordered_map<int, std::vector<byte>> last_messages;
    std::unordered_map<int, int> last_messages_timestamps;
    batteryData current_settings = {0, 0, false};

    byte high(short x) { return (byte)(x>>8);}
    byte low(short x) { return (byte)(x);}

    short toShort(byte x, byte y = 0) { return (short)(x<<8) + (short)(y); }

  public:
    //FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
    CANLine() {
      // Initialize the CAN bus
      can.begin();
      can.setBaudRate(JuanMbps);
      can.enableFIFO();
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

    void send(unsigned int id, byte *message, byte size = 8) {
      msgSend.id = id;
      msgSend.len = size;
      for (int i = 0; i < size; i++) {
        //Serial.print(message[i]);
        //Serial.print(" ");
        msgSend.buf[i] = message[i];
      }
      //Serial.println();
      can.write(msgSend);
      //Serial.print("Frame sent to id 0x");
      //Serial.println(id, HEX);
    }

    void send(unsigned int id, short *message, byte size = 4) {
      byte msg[2*size];
      for (int i = 0; i < size; i++) {
        msg[2*i] = high(message[i]);
        msg[2*i+1] = low(message[i]);
      }
      send(id, msg, 2*size);
    }

    int recieve_one() {
        if (can.readFIFO(msgRecieve) == 0) return 0;
        //Serial.print("0x");
        //Serial.print(msgRecieve.id, HEX);
        //Serial.print(" ");
        last_messages[msgRecieve.id] = std::vector<byte>();
        for (int i = 0; i < msgRecieve.len; i++) {
          //Serial.print(msgRecieve.buf[i]);
          last_messages[msgRecieve.id].push_back(msgRecieve.buf[i]);
        }
        last_messages_timestamps[msgRecieve.id] = millis();
        return 1;
    }

    int recieve_many() {
      if (recieve_one() == 0) return 0;
      while (recieve_one()) {};
      return 1;
    }

    std::vector<byte> recieve(unsigned int id) {
      recieve_many();
      return last_messages[id];
    }

    int age_of(unsigned int id) {
      return last_messages_timestamps[id];
    }

    /*
    std::vector<byte> recieve(unsigned int id, byte size = 8) {
      CAN_message_t msgRecieve;
      std::vector<byte> result = std::vector<byte>(size, 0);
      can.read(msgRecieve);
      Serial.print("0x");
      Serial.print(msgRecieve.id, HEX);
      //  if (msgRecieve.id != id) {
      //    Serial.println("Looks like the id is wrong!");
      //    return result;
      //  }
      for (int i = 0; i < size; i++) {
        result[i] = msgRecieve.buf[i];
      }
      return result;
    }
    */

    

    // start/stop charging
    // change limits command

    std::vector<short> recieveShort(unsigned int id) {
      std::vector<byte> bytes = recieve(id);
      bytes.push_back(0);

      std::vector<short> result = std::vector<short>((bytes.size())/2,0);

      for (int i = 0; i < result.size(); i++) {
        result[i] = toShort(bytes[2*i], bytes[2*i+1]);
      }
      return result;
    }

    short setMaxTerminalVoltage(short v) {
      current_settings.maxChargeVolts = v;
      sendToCharger(current_settings.maxChargeVolts, current_settings.maxChargeAmps, current_settings.charging);
      return current_settings.maxChargeVolts;
    }

    short setMaxChargingCurrent(short a) {
      current_settings.maxChargeAmps = a;
      sendToCharger(current_settings.maxChargeVolts, current_settings.maxChargeAmps, current_settings.charging);
      return current_settings.maxChargeAmps;
    }

    bool toggleCharging() {
      current_settings.charging = !current_settings.charging;
      sendToCharger(current_settings.maxChargeVolts, current_settings.maxChargeAmps, current_settings.charging);
      return current_settings.charging;
    }

    bool isCharging() { return current_settings.charging; }

    void sendToCharger(short maxChargeVolts, short maxChargeAmps, bool enableCharge) {
      short msg[4] = {maxChargeVolts, maxChargeAmps, (short)enableCharge, 0};
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
      short* msg = &(recieveShort(0x18FF50E5)[0]); //size 4

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

#endif
