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
  int16_t maxChargeVolts;
  bool discharging;
  int16_t maxChargeAmps;
  bool hardwareFailure;
  bool temperatureFailure;
  bool inputVoltageFailure;
  bool batteryConnectionFailure;
  bool communicationFailure;  
};

struct batteryData {
  int16_t maxChargeVolts;
  int16_t maxChargeAmps;
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
      last_sent_messages[0x97] = std::vector<byte>({255, 255, 0, 255, 255, 255, 255, 255});
      last_sent_messages[0x98] = std::vector<byte>({255, 255, 255, 255, 255, 255, 255, 0});
      last_sent_messages[0x99] = std::vector<byte>({255, 255, 255, 255, 0, 0, 0, 0});
      last_sent_messages[0x1806E5F4] = std::vector<byte>({255, 255, 255, 255, 0, 0, 0, 0});
      last_recieved_messages[0x18FF50E5] = std::vector<byte>({255, 255, 255, 255, 0, 0, 0, 0});
    }

    byte high(uint16_t x) { return (byte)(x>>8);}
    byte low(uint16_t x) { return (byte)(x);}

    bool getBool(byte x, int bitnum) {
      x = x >> (7 - bitnum);
      return (bool)(x & 1);
    }

    uint16_t toShort(byte x, byte y = 0) { return (uint16_t)(x<<8) + (uint16_t)(y); }


    void send(uint32_t id, byte *message, bool to_charger = false, byte size = 8) {
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

    void send(uint32_t id, uint16_t *message, bool to_charger = false, byte size = 4) {
      byte msg[2*size];
      for (int i = 0; i < size; i++) {
        msg[2*i] = high(message[i]);
        msg[2*i+1] = low(message[i]);
      }
      send(id, msg, to_charger, 2*size);
    }

    void editBytes(uint32_t id, uint32_t count, byte *values, uint32_t *indices, bool to_charger = false) {
      byte *msg = &(last_sent_messages[id][0]);
      for (int i = 0; i < count; i++) {
          msg[indices[i]] = values[i];
      }
      send(id, msg, to_charger, last_sent_messages[id].size());
    }

    void editShort(uint32_t id, uint32_t count, uint16_t *values, uint32_t *indices, bool to_charger = false) {
      byte *valuesBytes;
      uint32_t *indicesBytes;
      for (int i = 0; i < count; i++) {
        valuesBytes[2*i] = high(values[i]);
        valuesBytes[2*i+1] = low(values[i]);
        indicesBytes[2*i] = 2*indices[i];
        indicesBytes[2*i+1] = 2*indices[i]+1;
      }
      editBytes(id, 2*count, valuesBytes, indicesBytes, to_charger);
    }

    void editBit(uint32_t id, byte bytenum, uint32_t count, bool *values, uint32_t *indice, bool to_charger = false) {
      byte result = last_sent_messages[id][bytenum];
      for (int i = 0; i < count; i++) {
          values[i] ? (result | (0b10000000 >> indice[i])) : (result & (~(0b010000000) >> indice[i]));
      }
      byte byteValues[] = { result };
      uint32_t indices[] = { bytenum };
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

    std::vector<byte> recieve(uint32_t id) {
      update_recieved_msgs();
      return last_recieved_messages[id];
    }

    std::vector<uint16_t> recieveShort(uint32_t id) {
      std::vector<byte> bytes = recieve(id);
      bytes.push_back(0);

      std::vector<uint16_t> result = std::vector<uint16_t>((bytes.size())/2,0);

      for (int i = 0; i < result.size(); i++) {
        result[i] = toShort(bytes[2*i], bytes[2*i+1]);
      }
      return result;
    }

    int age_of(uint32_t id) {
      return last_recieved_messages_timestamps[id];
    }

    void sendToCharger(int16_t maxChargeVolts, int16_t maxChargeAmps, bool enableCharge) {
      uint16_t msg[4] = {(uint16_t)maxChargeVolts, (uint16_t)maxChargeAmps, (uint16_t)enableCharge, 0};
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
    int16_t charger_setMaxTerminalVoltage(int16_t v) {
      //std::vector<byte> msg = last_sent_messages[0x1806E5F4];
      uint16_t values[] = { v };
      uint32_t indices[] = {0};
      editShort(0x1806E5F4, 1, values, indices, true);
      //current_settings.maxChargeVolts = v;
      //sendToCharger(current_settings.maxChargeVolts, current_settings.maxChargeAmps, current_settings.charging);
      return (int16_t)toShort(last_sent_messages[0x1806E5F4][0], last_sent_messages[0x1806E5F4][1]);
    }

    int16_t charger_setMaxChargingCurrent(int16_t a) {
      uint16_t values[] = { a };
      uint32_t indices[] = {1};
      editShort(0x1806E5F4, 1, values, indices, true);
      return (int16_t)toShort(last_sent_messages[0x1806E5F4][2], last_sent_messages[0x1806E5F4][3]);
    }

    bool charger_toggleCharging() {
      byte *msg = &(last_sent_messages[0x1806E5F4][0]);
      byte values[] = { !((bool)msg[4]) };
      uint32_t indices[] = {4};
      editBytes(0x1806E5F4, 1, values, indices, true);
      return (bool)last_sent_messages[0x1806E5F4][4];
   }

    //get charger stuff
    bool charger_chargingAllowed() { return (bool)last_sent_messages[0x1806E5F4][4]; }

    /*chargerData charger_output() {
      uint16_t* msg = &(recieveShort(0x18FF50E5)[0]); //size 4

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

    uint16_t charger_max_output_voltage() {
      uint16_t* msg = &(recieveShort(0x18FF50E5)[0]);
      return msg[0];
    }

    int16_t charger_max_output_current() {
      uint16_t* msg = &(recieveShort(0x18FF50E5)[0]);
      return (int16_t)msg[1];
    }

    bool charger_is_discharging() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[3], 0);
    }

    bool charger_is_hardware_failure() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 0);
    }

    bool charger_is_over_temp() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 1);
    }

    bool charger_is_in_voltage_error() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 2);
    }

    bool charger_is_not_connected_properly() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 3);
    }

    bool charger_is_comms_error() {
      byte* msg = &(recieve(0x18FF50E5)[0]);
      return getBool(msg[5], 4);
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
      return getBool(msg[0], 7);
    }

    int16_t idk_battery_max_voltage() {
      uint16_t* msg = &(recieveShort(0x67)[0]);
      return (int16_t)msg[0];
    }

    int16_t idk_battery_max_out_current() {
      uint16_t* msg = &(recieveShort(0x67)[0]);
      return (int16_t)msg[1];
    }

    int16_t idk_battery_max_temperature() {
      uint16_t* msg = &(recieveShort(0x67)[0]);
      return (int16_t)msg[2];
    }

    byte get_req_ping_request() {
      byte* msg = &(recieve(0x95)[0]);
      return (byte)msg[0];
    }

    //send to system functions
    uint16_t send_accumulator_voltage(uint16_t v) {
      uint16_t values[] = { v };
      uint32_t indices[] = {0};
      editShort(0x96, 1, values, indices, true);
      return toShort(last_sent_messages[0x96][0], last_sent_messages[0x96][1]);      
    }

    uint16_t send_accumulator_current(uint16_t a) {
      uint16_t values[] = { a };
      uint32_t indices[] = {1};
      editShort(0x96, 1, values, indices, true);
      return toShort(last_sent_messages[0x96][2], last_sent_messages[0x96][3]);      
    }

    int16_t send_max_cell_temp(int16_t t) {
      uint16_t values[] = { t };
      uint32_t indices[] = {2};
      editShort(0x96, 1, values, indices, true);
      return toShort(last_sent_messages[0x96][4], last_sent_messages[0x96][5]);      
    }

    bool send_over_temp_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b01111000)) };
      uint32_t indices[] = { 0, 7 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 0);      
    }

    bool send_over_voltage_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b10111000)) };
      uint32_t indices[] = { 1, 7 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 1);      
    }

    bool send_over_current_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b11011000)) };
      uint32_t indices[] = { 2, 7 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 2);      
    }

    bool send_bms_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b11110000)) };
      uint32_t indices[] = { 3, 7 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 3);      
    }
    bool send_under_voltage_error(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][6] & 0b11101000)) };
      uint32_t indices[] = { 4, 7 };
      editBit(0x96, 6, 1, values, indices);
      return getBool(last_recieved_messages[0x96][6], 4);      
    }


    bool send_bms_open_wire_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b01101110)) };
      uint32_t indices[] = { 0, 3 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 0);
    }

    bool send_bms_adc_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b10101110)) };
      uint32_t indices[] = { 1, 3 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 1);
    }

    bool send_bms_cell_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11001110)) };
      uint32_t indices[] = { 2, 3 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 2);
    }

    bool send_low_charger_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11100110)) };
      uint32_t indices[] = { 4, 3 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 4);
    }

    bool send_cell_imbalance_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11101010)) };
      uint32_t indices[] = { 5, 3 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 5);
    }

    bool send_humidity_warn(bool b) {
      bool values[] = { b, (!b && !(last_sent_messages[0x96][7] & 0b11101100)) };
      uint32_t indices[] = { 6, 3 };
      editBit(0x96, 7, 1, values, indices);
      return getBool(last_recieved_messages[0x96][7], 6);
    }


    uint16_t send_ts_voltage(uint16_t v) {
      uint16_t values[] = { v };
      uint32_t indices[] = { 0 };
      editShort(0x97, 1, values, indices);
      return toShort(last_sent_messages[0x97][0], last_sent_messages[0x97][1]);
    }

    bool send_air_plus_state(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 0 };
      editBit(0x97, 2, 1, values, indices);
      return getBool(last_sent_messages[0x97][2], 0);
    }

    bool send_air_minus_state(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 1 };
      editBit(0x97, 2, 1, values, indices);
      return getBool(last_sent_messages[0x97][2], 1);
    }

    bool send_precharging(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 2 };
      editBit(0x97, 2, 1, values, indices);
      return getBool(last_sent_messages[0x97][2], 2);
    }

    bool send_precharging_done(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 3 };
      editBit(0x97, 2, 1, values, indices);
      return getBool(last_sent_messages[0x97][2], 3);
    }

    uint16_t send_max_bal_res_temp(uint16_t t) {
      byte values[] = { high(t), low(t) };
      uint32_t indices[] = { 3, 4 };
      editBytes(0x97, 2, values, indices);
      return toShort(last_sent_messages[0x97][3], last_sent_messages[0x97][4]);
    }

    //shutdown circuit pin: 16v 8 bits ADC
    byte send_SDC_voltage(byte v) {
      byte values[] = { v };
      uint32_t indices[] = { 5 };
      editBytes(0x97, 1, values, indices);
      return last_sent_messages[0x97][5];
    }

    byte send_glv_voltage(byte v) {
      byte values[] = { v };
      uint32_t indices[] = { 6 };
      editBytes(0x97, 1, values, indices);
      return last_sent_messages[0x97][6];
    }

    byte send_soc(byte p) {
      byte values[] = { p };
      uint32_t indices[] = { 7 };
      editBytes(0x97, 1, values, indices);
      return last_sent_messages[0x97][7];
    }


    byte cooling_send_fan_speed_1(byte s) {
      byte values[] = { s };
      uint32_t indices[] = { 0 };
      editBytes(0x98, 1, values, indices);
      return last_sent_messages[0x98][0];
    }

    byte cooling_send_fan_speed_2(byte s) {
      byte values[] = { s };
      uint32_t indices[] = { 1 };
      editBytes(0x98, 1, values, indices);
      return last_sent_messages[0x98][1];
    }

    byte cooling_send_fan_speed_3(byte s) {
      byte values[] = { s };
      uint32_t indices[] = { 2 };
      editBytes(0x98, 1, values, indices);
      return last_sent_messages[0x98][2];
    }

    byte cooling_send_pump_speed(byte s) {
      byte values[] = { s };
      uint32_t indices[] = { 3 };
      editBytes(0x98, 1, values, indices);
      return last_sent_messages[0x98][3];
    }

    byte cooling_set_acu_temp_1(byte t) {
      byte values[] = { t };
      uint32_t indices[] = { 4 };
      editBytes(0x98, 1, values, indices);
      return last_sent_messages[0x98][4];
    }

    byte cooling_set_acu_temp_2(byte t) {
      byte values[] = { t };
      uint32_t indices[] = { 5 };
      editBytes(0x98, 1, values, indices);
      return last_sent_messages[0x98][5];
    }

    byte cooling_set_acu_temp_3(byte t) {
      byte values[] = { t };
      uint32_t indices[] = { 6 };
      editBytes(0x98, 1, values, indices);
      return last_sent_messages[0x98][6];
    }

    bool cooling_send_over_temp_error(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 0 };
      editBit(0x98, 7, 1, values, indices);
      return getBool(last_sent_messages[0x98][7], 0);
    }    

    bool cooling_send_fan_1_error(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 1 };
      editBit(0x98, 7, 1, values, indices);
      return getBool(last_sent_messages[0x98][7], 1);
    }    

    bool cooling_send_fan_2_error(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 2 };
      editBit(0x98, 7, 1, values, indices);
      return getBool(last_sent_messages[0x98][7], 2);
    }    

    bool cooling_send_fan_3_error(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 3 };
      editBit(0x98, 7, 1, values, indices);
      return getBool(last_sent_messages[0x98][7], 3);
    }    

    bool cooling_send_fan_4_error(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 4 };
      editBit(0x98, 7, 1, values, indices);
      return getBool(last_sent_messages[0x98][7], 4);
    }    

    bool cooling_send_pump_error(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 5 };
      editBit(0x98, 7, 1, values, indices);
      return getBool(last_sent_messages[0x98][7], 5);
    }    



    uint16_t charge_cart_send_max_current(uint16_t a) {
      uint16_t values[] = { a };
      uint32_t indices[] = { 0 };
      editShort(0x99, 1, values, indices);
      return toShort(last_sent_messages[0x99][0], last_sent_messages[0x99][1]);
    }

    uint16_t charge_cart_send_max_voltage(uint16_t v) {
      uint16_t values[] = { v };
      uint32_t indices[] = { 0 };
      editShort(0x99, 1, values, indices);
      return toShort(last_sent_messages[0x99][2], last_sent_messages[0x99][3]);
    }

    bool charge_cart_send_charging(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 0 };
      editBit(0x99, 4, 1, values, indices);
      return getBool(last_sent_messages[0x99][4], 0);
    }

    bool charge_cart_send_check_pass(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 1 };
      editBit(0x99, 4, 1, values, indices);
      return getBool(last_sent_messages[0x99][4], 1);
    }

    bool charge_cart_send_good_comms(bool b) {
      bool values[] = { b };
      uint32_t indices[] = { 2 };
      editBit(0x99, 4, 1, values, indices);
      return getBool(last_sent_messages[0x99][4], 2);
    }

    bool charge_cart_send_sdc(bool b) {
      bool values[] = { b, b, b, b };
      uint32_t indices[] = { 3, 4, 5, 6 };
      editBit(0x99, 4, 4, values, indices);
      return getBool(last_sent_messages[0x99][4], 3);
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
