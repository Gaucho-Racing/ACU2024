#include "can.h"
void sendCANData(uint32_t ID){
  //moved CAN id set to bottom for temp config cell data fix, see line 134
  msg.flags.extended = true;
  msg.id = ID;
  switch(ID){
    case ACU_General:{
      uint16_t accVolt = battery.getTotalVoltage();
      msg.buf[0] = accVolt >> 8;
      msg.buf[1] = accVolt;
      msg.buf[2] = (int)acu.getTsCurrent() >> 8;
      msg.buf[3] = (int) acu.getTsCurrent();
      int16_t tempCodeSend = (int16_t)(battery.maxCellTemp * 100);
      msg.buf[4] = tempCodeSend >> 8;
      msg.buf[5] = tempCodeSend;
      msg.buf[6] = acu.errs;
      msg.buf[7] = acu.warns;
      can_prim.write(msg);
    }break;

    case ACU_General2:{
      uint16_t tsVoltage = acu.getTsVoltage() * 1000;
      msg.buf[0] = tsVoltage >> 8;
      msg.buf[1] = tsVoltage;
      msg.buf[2] = acu.getRelayState();
      int16_t tempCodeSend = (int16_t)(battery.maxBalTemp * 100);
      msg.buf[3] = tempCodeSend >> 8;
      msg.buf[4] = tempCodeSend;
      msg.buf[5] = acu.getShdnVolt(); //
      msg.buf[6] = acu.getGlvVoltage();
      msg.buf[7] = battery.calcCharge(); // calcCharge needs 2B implemented
      can_prim.write(msg); 
    }break;

    case Powertrain_Cooling:{
      msg.buf[0] = (int)(acu.fanRpm[0] / 50);
      msg.buf[1] = (int)(acu.fanRpm[1] / 50);
      msg.buf[2] = (int)(acu.fanRpm[2] / 50);
      msg.buf[3] = 0; // --> PUMP SPEED TBD
      msg.buf[4] = (int)(acu.getDcdcTemp1() / 2);
      msg.buf[5] = (int)(acu.getDcdcTemp2() / 2);
      // msg.buf[6] = (int)(acu.acuTemp[2] / 2);  This actually doesn't exist
      msg.buf[7] = (int)(acu.fans.readRegister(0x00)); // fan status --> to be implemented
      can_prim.write(msg); 
    }break;

    case Charging_Cart_Config:{
      uint16_t max_charge_current = battery.max_chrg_current;
      uint16_t max_charge_volt = battery.max_chrg_voltage;
      msg.buf[0] = max_charge_current >> 8;
      msg.buf[1] = max_charge_current;
      msg.buf[2] = max_charge_volt >> 8;
      msg.buf[3] = max_charge_volt;
      msg.buf[4] = state == CHARGE ? 1: 0; // not sure abt this one
      msg.buf[5] = 0b0000000;
      msg.buf[6] = 0b0000000;
      msg.buf[7] = 0b0000000;
      can_prim.write(msg);
    }break;
      
      //this is unable to be sent reflexively, temporary fix, TODO: fix this
    case Expanded_Cell_Data: {
      //what's the point of this?
      // msg.buf[0] = 0xA0; // 0b10100000;
      uint8_t cell_num = msg.buf[1];
      if(msg.id == Configure_Cell_Data)
        cell_num = msg.buf[1];
      if(cell_num > 128){
        Serial.println("CAN send Expanded Cell Error: Invalid cell number");
        cell_num = 128;
      }
      uint16_t cell_volt = condenseVoltage(cell_num);
      uint16_t open_cell_volt = 0b0000000; // what's this? No idea yet
      uint16_t cell_temp = condenseTemperature(battery.cellTemp[cell_num]);
      msg.buf[1] = cell_volt >> 8;
      msg.buf[2] = cell_volt;
      msg.buf[3] = open_cell_volt >> 8;
      msg.buf[4] = open_cell_volt;
      msg.buf[5] = cell_temp >> 8;
      msg.buf[6] = cell_temp;
      msg.buf[7] = 0b0000000; // TBD
    }break;
      
    case Condensed_Cell_Voltage_n0:
    case Condensed_Cell_Voltage_n8:
    case Condensed_Cell_Voltage_n16:
    case Condensed_Cell_Voltage_n24:
    case Condensed_Cell_Voltage_n32:
    case Condensed_Cell_Voltage_n40:
    case Condensed_Cell_Voltage_n48:
    case Condensed_Cell_Voltage_n56:
    case Condensed_Cell_Voltage_n64:
    case Condensed_Cell_Voltage_n72:
    case Condensed_Cell_Voltage_n80:
    case Condensed_Cell_Voltage_n88:
    case Condensed_Cell_Voltage_n96:
    case Condensed_Cell_Voltage_n104:
    case Condensed_Cell_Voltage_n112:
    case Condensed_Cell_Voltage_n120:
    case Condensed_Cell_Voltage_n128:
    case Condensed_Cell_Voltage_n136:{
      uint8_t i = ID - Condensed_Cell_Voltage_n0;
      msg.buf[0] = condenseVoltage(battery.cellVoltage[i * 8 + 0]);
      msg.buf[1] = condenseVoltage(battery.cellVoltage[i * 8 + 1]);
      msg.buf[2] = condenseVoltage(battery.cellVoltage[i * 8 + 2]);
      msg.buf[3] = condenseVoltage(battery.cellVoltage[i * 8 + 3]);
      msg.buf[4] = condenseVoltage(battery.cellVoltage[i * 8 + 4]);
      msg.buf[5] = condenseVoltage(battery.cellVoltage[i * 8 + 5]);
      msg.buf[6] = condenseVoltage(battery.cellVoltage[i * 8 + 6]);
      msg.buf[7] = condenseVoltage(battery.cellVoltage[i * 8 + 7]);
      can_prim.write(msg);
    }
      break;
      
    case Condensed_Cell_Temp_n0:
    case Condensed_Cell_Temp_n8:
    case Condensed_Cell_Temp_n16:
    case Condensed_Cell_Temp_n24:
    case Condensed_Cell_Temp_n32:
    case Condensed_Cell_Temp_n40:
    case Condensed_Cell_Temp_n48:
    case Condensed_Cell_Temp_n56:
    case Condensed_Cell_Temp_n64:
    case Condensed_Cell_Temp_n72:
    case Condensed_Cell_Temp_n80:
    case Condensed_Cell_Temp_n88:
    case Condensed_Cell_Temp_n96:
    case Condensed_Cell_Temp_n104:
    case Condensed_Cell_Temp_n112:
    case Condensed_Cell_Temp_n120:
    case Condensed_Cell_Temp_n128:
    case Condensed_Cell_Temp_n136:{
      uint8_t i = ID - Condensed_Cell_Temp_n0;
      msg.buf[0] = condenseTemperature((battery.cellTemp[i * 16 + 0] + battery.cellTemp[i * 16 + 1]) / 2);
      msg.buf[1] = condenseTemperature((battery.cellTemp[i * 16 + 2] + battery.cellTemp[i * 16 + 3]) / 2);
      msg.buf[2] = condenseTemperature((battery.cellTemp[i * 16 + 4] + battery.cellTemp[i * 16 + 5]) / 2);
      msg.buf[3] = condenseTemperature((battery.cellTemp[i * 16 + 6] + battery.cellTemp[i * 16 + 7]) / 2);
      msg.buf[4] = condenseTemperature((battery.cellTemp[i * 16 + 8] + battery.cellTemp[i * 16 + 9]) / 2);
      msg.buf[5] = condenseTemperature((battery.cellTemp[i * 16 + 10] + battery.cellTemp[i * 16 + 11]) / 2);
      msg.buf[6] = condenseTemperature((battery.cellTemp[i * 16 + 12] + battery.cellTemp[i * 16 + 13]) / 2);
      msg.buf[7] = condenseTemperature((battery.cellTemp[i * 16 + 14] + battery.cellTemp[i * 16 + 15]) / 2);
      can_prim.write(msg);
    }
      break;
      
    case ACU_Ping_Response:
      can_prim.write(msg);
      break;
      
    case Charger_Control:
      msg.buf[0] = battery.max_chrg_current >> 8;
      msg.buf[1] = battery.max_chrg_current;
      msg.buf[2] = battery.max_chrg_voltage >> 8;
      msg.buf[3] = battery.max_chrg_voltage;
      msg.buf[4] = state == CHARGE ? 1:0; 
      msg.buf[5] = 0b0000000;
      msg.buf[6] = 0b0000000; 
      msg.buf[7] = 0b0000000; 
      can_chgr.write(msg);
      break;
      
    default:
      Serial.println("FUCK U U IDIOT"); // language, sheesh
  }
  msg.id = ID;
}

void parseCANData(){
  switch(msg.id){
    case Configure_Cell_Data:
      // if single send requested send immediately
      if (msg.buf[3] == 0x0){
        //if condensed
        if(msg.buf[2] == 0x0){
          sendCANData(Condensed_Cell_Voltage_n0 + msg.buf[1]/8);
          sendCANData(Condensed_Cell_Temp_n0 + msg.buf[1]/8);
        }
        //if expanded
        else{
          sendCANData(Expanded_Cell_Data);
        }
      }
      break;

    case ACU_Control:
      if(msg.buf[0] & 0b00000001){
        if(state == STANDBY)
          state = PRECHARGE;
          digitalWrite(PIN_AIR_RESET, HIGH);
          delayMicroseconds(1);
          digitalWrite(PIN_AIR_RESET, LOW);

      } else {
      }
      break;

    case Battery_Limits:
      battery.max_chrg_voltage =    (msg.buf[0] << 8) | msg.buf[1];
      battery.max_output_current =  (msg.buf[2] << 8) | msg.buf[3];
      battery.maxCellTemp =         ((msg.buf[4] << 8) | msg.buf[5])*0.01;
      battery.max_chrg_current =    (msg.buf[6] << 8) | msg.buf[7];
      break;

    case ACU_Ping_Request:
      sendCANData(ACU_Ping_Response);
      break;

    case Charging_SDC_Ping_Response:
      sendCANData(Charging_SDC_Ping_Request);
      break;

    case Charging_SDC_States:
      //FILLER
      msg.buf[0] = 0b0000000;
      msg.buf[1] = 0b0000000;
      msg.buf[2] = 0b0000000;
      msg.buf[3] = 0b0000000;
      msg.buf[4] = 0b0000000;
      msg.buf[5] = 0b0000000;
      msg.buf[6] = 0b0000000;
      msg.buf[7] = 0b0000000;
      break;
      
    case Charger_Data:
      // parse the max voltage, max current & chaging/not charging bool & get all failures
      battery.max_chrg_voltage = (msg.buf[0] << 8) | msg.buf[1];
      battery.max_chrg_current = (msg.buf[2] << 8) | msg.buf[3];
      // battery.chargerDataStatus.hardwareStatus = msg.buf[4] & ERR_Hardware;
      // battery.chargerDataStatus.temperatureStatus = msg.buf[4] & ERR_Temp;
      // battery.chargerDataStatus.inputVoltageStatus = msg.buf[4] & ERR_InputVolt;
      // battery.chargerDataStatus.startingState = msg.buf[4] & ERR_Start;
      // battery.chargerDataStatus.communicationState = msg.buf[4] & ERR_Comm;
      Serial.println("Charger Data Read, yaya we won't die");
      break;

    default:
      Serial.println("lol no message here for ya\t\t\t\t\t\t\t\t\tFucker");
  }
}

//Triage 2: replace with mailboxes
int readCANData(){
  int which_can = 0;
  int msgReads = 5;  //Max number of CAN message reads per function call

  for(; msgReads >= 0; msgReads--){
    if(!can_prim.read(msg))
      break;
    which_can = 1;
    parseCANData();
  }

  for(; msgReads >= 0; msgReads--){
    if(!can_chgr.read(msg))
      break;
    if(msg.id == Charger_Data)
      which_can = 2;
    parseCANData();
  }
  return which_can;

}

//TRIAGE 2: replace with interrupt/mailboxes
static uint32_t prevmillis = 0;

/// @brief Sends data to CANbus
/// @param[in] battery
/// @return None
void dumpCANbus() {
  if(millis() - prevmillis > 1){
    prevmillis = millis();
    for (uint8_t i = 0; i < 16; i++) {
    sendCANData(Condensed_Cell_Voltage_n0 + i);
    sendCANData(Condensed_Cell_Temp_n0 + i);
    }
    sendCANData(ACU_General);
    sendCANData(ACU_General2);
    sendCANData(Powertrain_Cooling);
    sendCANData(Charging_Cart_Config);
  }
}