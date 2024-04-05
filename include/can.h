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

struct cellDataConfigRequest {
  byte cellNum;
  bool isExpanded;
  byte periodic;
};

class CANLine {
  private:
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> charger_can;
    FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> vdm_can;
    CAN_message_t msgRecieve, msgSend;
    int Mbps1 = 1000000;
    int Mbps3 = 1000000;
    //static const int AteMbps = 8000000;
    std::unordered_map<int, std::vector<byte>> last_recieved_messages;
    std::unordered_map<int, int> last_recieved_messages_timestamps;
    std::unordered_map<int, std::vector<byte>> last_sent_messages;
    //batteryData current_settings = {0, 0, false};

    void initialize() {
      last_recieved_messages[0x64] = std::vector<byte>({255, 0, 255, 0, 0, 0, 0, 0});
      last_recieved_messages[0x66] = std::vector<byte>({0, 0, 0, 0, 0, 0, 0, 0});
      last_recieved_messages[0x67] = std::vector<byte>({255, 255, 255, 255, 255, 255, 0, 0});
      last_recieved_messages[0x95] = std::vector<byte>({255, 0, 0, 0, 0, 0, 0, 0});
      last_sent_messages[0x96] = std::vector<byte>({255, 255, 255, 255, 255, 255, 0, 0});
      last_sent_messages[0x1806E5F4] = std::vector<byte>({255, 255, 255, 255, 0, 0, 0, 0});
      last_recieved_messages[0x18FF50E5] = std::vector<byte>({255, 255, 255, 255, 0, 0, 0, 0});
    }

    byte high(unsigned short x) { return (byte)(x>>8);}
    byte low(unsigned short x) { return (byte)(x);}

    bool getBool(byte x, int bitnum) {
      x = x >> bitnum;
      return (bool)(x & 1);
    }

    unsigned short toShort(byte x, byte y = 0) { return (unsigned short)(x<<8) + (unsigned short)(y); }


    void send(unsigned int id, byte *message, bool to_charger = false, byte size = 8) {
      msgSend.id = id;
      msgSend.len = size;
      last_sent_messages[id] = std::vector<byte>();
      for (int i = 0; i < size; i++) {
        //Serial.print(message[i]);
        //Serial.print(" ");
        msgSend.buf[i] = message[i];
        last_sent_messages[id].push_back(message[i]);
      }
      //Serial.println();
      if (to_charger) charger_can.write(msgSend);
      else vdm_can.write(msgSend);
      //Serial.print("Frame sent to id 0x");
      //Serial.println(id, HEX);
    }

    void send(unsigned int id, unsigned short *message, bool to_charger = false, byte size = 4) {
      byte msg[2*size];
      for (int i = 0; i < size; i++) {
        msg[2*i] = high(message[i]);
        msg[2*i+1] = low(message[i]);
      }
      send(id, msg, to_charger, 2*size);
    }

    void editBytes(unsigned int id, unsigned int count, byte *values, unsigned int *indices, bool to_charger = false) {
      byte *msg = &(last_sent_messages[id][0]);
      for (int i = 0; i < count; i++) {
          msg[indices[i]] = values[i];
      }
      send(id, msg, to_charger, last_sent_messages[id].size());
    }

    void editShort(unsigned int id, unsigned int count, unsigned short *values, unsigned int *indices, bool to_charger = false) {
      byte *valuesBytes;
      unsigned int *indicesBytes;
      for (int i = 0; i < count; i++) {
        valuesBytes[2*i] = high(values[i]);
        valuesBytes[2*i+1] = low(values[i]);
        indicesBytes[2*i] = 2*indices[i];
        indicesBytes[2*i+1] = 2*indices[i]+1;
      }
      editBytes(id, 2*count, valuesBytes, indicesBytes, to_charger);
    }

    void editBit(unsigned int id, byte bytenum, unsigned int count, bool *values, unsigned int *indices, bool to_charger = false) {
      byte result = last_sent_messages[id][bytenum];
      for (int i = 0; i < count; i++) {
          values[i] ? (result | (0b00000001 << indices[i])) : (result & (~(0b00000001) << indices[i]));
      }
      byte byteValues[] = { result };
      unsigned int indices[] = { bytenum };
      editBytes(id, 1, byteValues, indices, to_charger);
    }

    int recieve_one(bool from_charger = false) {
        if (from_charger) if (charger_can.readFIFO(msgRecieve) == 0) return 0;
        else if (vdm_can.readFIFO(msgRecieve) == 0) return 0;
        //Serial.print("0x");
        //Serial.print(msgRecieve.id, HEX);
        //Serial.print(" ");
        last_recieved_messages[msgRecieve.id] = std::vector<byte>();
        for (int i = 0; i < msgRecieve.len; i++) {
          //Serial.print(msgRecieve.buf[i]);
          last_recieved_messages[msgRecieve.id].push_back(msgRecieve.buf[i]);
        }
        last_recieved_messages_timestamps[msgRecieve.id] = millis();
        return 1;
    }

    std::vector<byte> recieve(unsigned int id) {
      update_recieved_msgs();
      return last_recieved_messages[id];
    }

    std::vector<unsigned short> recieveShort(unsigned int id) {
      std::vector<byte> bytes = recieve(id);
      bytes.push_back(0);

      std::vector<unsigned short> result = std::vector<unsigned short>((bytes.size())/2,0);

      for (int i = 0; i < result.size(); i++) {
        result[i] = toShort(bytes[2*i], bytes[2*i+1]);
      }
      return result;
    }

    int age_of(unsigned int id) {
      return last_recieved_messages_timestamps[id];
    }

    void sendToCharger(short maxChargeVolts, short maxChargeAmps, bool enableCharge) {
      unsigned short msg[4] = {(unsigned short)maxChargeVolts, (unsigned short)maxChargeAmps, (unsigned short)enableCharge, 0};
      send(0x1806E5F4, msg, true);
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

  public:
    //FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
    CANLine(int Mbps1 = 1000000, int Mbps3 = 1000000) {
      // Initialize the CAN bus
      charger_can.begin();
      charger_can.setBaudRate(Mbps1);
      charger_can.enableFIFO();

      vdm_can.begin();
      vdm_can.setBaudRate(Mbps3);
      vdm_can.enableFIFO();
      // canData.begin();
      // CANFD_timings_t config;
      // config.clock = CLK_24MHz;
      // config.baudrate = JuanMbps;
      // config.baudrateFD = AteMbps;
      // config.propdelay = 190;
      // config.bus_length = 1;
      // config.sample = 70;
      // canData.setBaudRate(config);
      this->initialize();
    }

    int update_recieved_msgs() {
      if (recieve_one(false) == 0 && recieve_one(true) == 0) return 0;
      while (recieve_one(false)) {};
      while (recieve_one(true)) {};
      return 1;
    }

    // start/stop charging
    // charge limits command
    short charger_setMaxTerminalVoltage(short v) {
      //std::vector<byte> msg = last_sent_messages[0x1806E5F4];
      unsigned short values[] = { v };
      unsigned int indices[] = {0};
      editShort(0x1806E5F4, 1, values, indices, true);
      //current_settings.maxChargeVolts = v;
      //sendToCharger(current_settings.maxChargeVolts, current_settings.maxChargeAmps, current_settings.charging);
      return (short)toShort(last_sent_messages[0x1806E5F4][0], last_sent_messages[0x1806E5F4][1]);
    }

    short charger_setMaxChargingCurrent(short a) {
      unsigned short values[] = { a };
      unsigned int indices[] = {1};
      editShort(0x1806E5F4, 1, values, indices, true);
      return (short)toShort(last_sent_messages[0x1806E5F4][2], last_sent_messages[0x1806E5F4][3]);
    }

    bool charger_toggleCharging() {
      byte *msg = &(last_sent_messages[0x1806E5F4][0]);
      byte values[] = { !((bool)msg[4]) };
      unsigned int indices[] = {4};
      editBytes(0x1806E5F4, 1, values, indices, true);
      return (bool)last_sent_messages[0x1806E5F4][4];
   }

    //get charger stuff
    bool charger_chargingAllowed() { return (bool)last_sent_messages[0x1806E5F4][4]; }

    /*chargerData charger_output() {
      unsigned short* msg = &(recieveShort(0x18FF50E5)[0]); //size 4

      chargerData r;

      //need bitshifting
      r.maxChargeVolts = msg[0];
      r.discharging = msg[1] & 0x8000;
      r.maxChargeAmps = msg[1];
      r.hardwareFailure = msg[2] & 0x0100;
      r.temperatureFailure = msg[2] & 0x0200;
      r.inputVoltageFailure = msg[2] & 0x0400;
      r.batteryConnectionFailure = msg[2] & 0x0800;
      r.communicationFailure = msg[2] & 0x1000;

      return r;
    }  */

    unsigned short charger_max_output_voltage() {
      unsigned short* msg = &(recieveShort(0x18FF50E5)[0]);
      return msg[0];
    }

    short charger_max_output_current() {
      unsigned short* msg = &(recieveShort(0x18FF50E5)[0]);
      return (short)msg[1];
    }

    bool charger_is_discharging() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[3], 7);
    }

    bool charger_is_hardware_failure() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 7);
    }

    bool charger_is_over_temp() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 6);
    }

    bool charger_is_in_voltage_error() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 5);
    }

    bool charger_is_not_connected_properly() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 4);
    }

    bool charger_is_comms_error() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 3);
    }

    int charger_data_age() { return age_of(0x18FF50E5); }


    //vdm recieve
    cellDataConfigRequest vdm_cell_data_config_request() { 
      byte* msg = &(recieve(0x64)[0]);
      cellDataConfigRequest r = {msg[0], (bool)msg[1], msg[2]};
      return r;
    }
  
    bool vdm_ts_active() {
      byte* msg = &(recieve(0x66)[0]);
      return getBool(msg[0], 0);
    }

    short idk_battery_max_voltage() {
      unsigned short* msg = &(recieveShort(0x67)[0]);
      return (short)msg[0];
    }

    short idk_battery_max_out_current() {
      unsigned short* msg = &(recieveShort(0x67)[0]);
      return (short)msg[1];
    }

    short idk_battery_max_temperature() {
      unsigned short* msg = &(recieveShort(0x67)[0]);
      return (short)msg[2];
    }

    byte get_req_ping_request() {
      byte* msg = &(recieve(0x95)[0]);
      return (byte)msg[0];
    }

    //send to system functions
    unsigned short send_accumulator_voltage(unsigned short v) {
      unsigned short values[] = { v };
      unsigned int indices[] = {0};
      editShort(0x96, 1, values, indices, true);
      return toShort(last_sent_messages[0x96][0], last_sent_messages[0x96][1]);      
    }

    unsigned short send_accumulator_current(unsigned short a) {
      unsigned short values[] = { a };
      unsigned int indices[] = {1};
      editShort(0x96, 1, values, indices, true);
      return toShort(last_sent_messages[0x96][2], last_sent_messages[0x96][3]);      
    }

    short send_max_cell_temp(short t) {
      unsigned short values[] = { t };
      unsigned int indices[] = {2};
      editShort(0x96, 1, values, indices, true);
      return toShort(last_sent_messages[0x96][4], last_sent_messages[0x96][5]);      
    }

    bool send_over_temp_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b01111000)) };
      unsigned int indices[] = { 7, 0 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 7);      
    }

    bool send_over_voltage_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b10111000)) };
      unsigned int indices[] = { 6, 0 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 6);      
    }

    bool send_over_current_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b11011000)) };
      unsigned int indices[] = { 5, 0 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 5);      
    }

    bool send_bms_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b11110000)) };
      unsigned int indices[] = { 3, 0 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 3);      
    }
    bool send_under_voltage_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b11101000)) };
      unsigned int indices[] = { 4, 0 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 4);      
    }


    bool send_bms_open_wire_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b01101110)) };
      unsigned int indices[] = { 7, 4 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 7);
    }

    bool send_bms_adc_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b10101110)) };
      unsigned int indices[] = { 6, 4 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 6);
    }

    bool send_bms_cell_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11001110)) };
      unsigned int indices[] = { 5, 4 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 5);
    }

    bool send_low_charger_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11100110)) };
      unsigned int indices[] = { 3, 4 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 3);
    }

    bool send_cell_imbalance_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11101010)) };
      unsigned int indices[] = { 2, 4 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 2);
    }

    bool send_humidity_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11101100)) };
      unsigned int indices[] = { 1, 4 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 1);
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

