#include <Arduino.h>
#include "FlexCAN_T4.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary; //Depends on what board u test on ig but prob just use this for now
//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canPrimary;
//FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> canData;
CAN_message_t msgPrimary;
CAN_message_t msg;
CANFD_message_t msgData;

//#define JuanMbps 1000000
//#define AteMbps 8000000

struct CAN {
  static const int JuanMbps = 1000000;
  static const int AteMbps = 8000000;
  CAN_message_t msgPrimary;
  CAN_message_t msg;
  short maxChargeVolts;
  bool discharging;
  short maxChargeAmps;
  bool hardwareFailure;
  bool temperatureFailure;
  bool inputVoltageFailure;
  bool batteryConnectionFailure;
  bool communicationFailure;  

  void initialize() {
    // Initialize the CAN bus
    canPrimary.begin();
    canPrimary.setBaudRate(JuanMbps);
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

  void send(unsigned short maxChargeVolts, unsigned short maxChargeAmps, bool enableCharge) {
    msgPrimary.id = 0x1806E5F4;
    msgPrimary.len = 8;
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
  }

  void recieve() {
    canPrimary.read(msg);
    Serial.println(msg.id, HEX);
    if (msg.id != 0x18FF50E5) {
      Serial.println("Looks like the id is wrong!");
      return;
    }
    maxChargeVolts = (((short)msg.buf[0]) << 8) + (short)msg.buf[1];
    discharging = msg.buf[3] & 0x80;
    maxChargeAmps = (((short)msg.buf[3]) << 8) + (short)msg.buf[4];
    hardwareFailure = msg.buf[5] & 0x01;
    temperatureFailure = msg.buf[5] & 0x02;
    inputVoltageFailure = msg.buf[5] & 0x04;
    batteryConnectionFailure = msg.buf[5] & 0x08;
    communicationFailure = msg.buf[5] & 0x10;

    return;
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
