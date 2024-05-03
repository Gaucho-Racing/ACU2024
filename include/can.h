#ifndef CAN_H
#define CAN_H

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "canID.h"
#include "ACU.h"


void parseCANData(Battery &battery);
int readCANData(Battery &battery);
void sendCANData(Battery &battery, uint32_t ID);



void parseCANData(Battery &battery){
  switch(battery.msg.id){
    case Configure_Cell_Data:
      // if single send requested send immediately
      if (battery.msg.buf[3] == 0x0){
        //if condensed
        if(battery.msg.buf[2] == 0x0){
          sendCANData(battery, Condensed_Cell_Voltage_n0 + battery.msg.buf[1]/8);
          sendCANData(battery, Condensed_Cell_Temp_n0 + battery.msg.buf[1]/8);
        }
        //if expanded
        else{
          sendCANData(battery, Expanded_Cell_Data);
        }
      }
      break;

    case ACU_Control:
      battery.relay_state = battery.msg.buf[0];
      break;

    case Battery_Limits:
      battery.max_chrg_voltage =    (battery.msg.buf[0] << 8) | battery.msg.buf[1];
      battery.max_output_current =  (battery.msg.buf[2] << 8) | battery.msg.buf[3];
      battery.maxCellTemp =         ((battery.msg.buf[4] << 8) | battery.msg.buf[5])*0.01;
      battery.max_chrg_current =    (battery.msg.buf[6] << 8) | battery.msg.buf[7];
      break;

    case ACU_Ping_Request:
      sendCANData(battery, ACU_Ping_Response);
      break;

    case Charging_SDC_Ping_Response:
      sendCANData(battery, Charging_SDC_Ping_Request);
      break;

    case Charging_SDC_States:
      //FILLER
      battery.msg.buf[0] = 0b0000000;
      battery.msg.buf[1] = 0b0000000;
      battery.msg.buf[2] = 0b0000000;
      battery.msg.buf[3] = 0b0000000;
      battery.msg.buf[4] = 0b0000000;
      battery.msg.buf[5] = 0b0000000;
      battery.msg.buf[6] = 0b0000000;
      battery.msg.buf[7] = 0b0000000;
      break;
      
    case Charger_Data:
      // parse the max voltage, max current & chaging/not charging bool & get all failures
      battery.max_chrg_voltage = (battery.msg.buf[0] << 8) | battery.msg.buf[1];
      battery.max_chrg_current = (battery.msg.buf[2] << 8) | battery.msg.buf[3];
      // battery.chargerDataStatus.hardwareStatus = battery.msg.buf[4] & ERR_Hardware;
      // battery.chargerDataStatus.temperatureStatus = battery.msg.buf[4] & ERR_Temp;
      // battery.chargerDataStatus.inputVoltageStatus = battery.msg.buf[4] & ERR_InputVolt;
      // battery.chargerDataStatus.startingState = battery.msg.buf[4] & ERR_Start;
      // battery.chargerDataStatus.communicationState = battery.msg.buf[4] & ERR_Comm;
      Serial.println("Charger Data Read, yaya we won't die");
      break;

    default:
      Serial.println("lol no message here for ya\t\t\t\t\t\t\t\t\tFucker");
  }
}

int readCANData(Battery &battery){
  int which_can = 0;
  int msgReads = 5;  //Max number of CAN message reads per function call

  for(; msgReads >= 0; msgReads--){
    if(!battery.can_prim.read(battery.msg))
      break;
    which_can = 1;
    parseCANData(battery);
  }

  for(; msgReads >= 0; msgReads--){
    if(!battery.can_chgr.read(battery.msg))
      break;
    if(battery.msg.id == Charger_Data)
      which_can = 2;
    parseCANData(battery);
  }
  return which_can;

}

void sendCANData(Battery &battery, uint32_t ID){
  //moved CAN id set to bottom for temp config cell data fix, see line 134
  battery.msg.flags.extended = true;
  battery.msg.id = ID;
  switch(ID){
    case ACU_General:{
      uint16_t accVolt = getAccumulatorVoltage(battery);
      battery.msg.buf[0] = accVolt >> 8;
      battery.msg.buf[1] = accVolt;
      battery.msg.buf[2] = battery.accumCurrent >> 8;
      battery.msg.buf[3] = battery.accumCurrent;
      int16_t tempCodeSend = (int16_t)(battery.maxCellTemp * 100);
      battery.msg.buf[4] = tempCodeSend >> 8;
      battery.msg.buf[5] = tempCodeSend;
      battery.msg.buf[6] = battery.errs;
      battery.msg.buf[7] = battery.warns;
      battery.can_prim.write(battery.msg);
    }break;

    case ACU_General2:{
      uint16_t tsVoltage = battery.ts_voltage;
      battery.msg.buf[0] = tsVoltage >> 8;
      battery.msg.buf[1] = tsVoltage;
      battery.msg.buf[2] = battery.relay_state;
      int16_t tempCodeSend = (int16_t)(battery.maxBalTemp * 100);
      battery.msg.buf[3] = tempCodeSend >> 8;
      battery.msg.buf[4] = tempCodeSend;
      battery.msg.buf[5] = battery.sdc_voltage;
      battery.msg.buf[6] = battery.glv_voltage;
      battery.msg.buf[7] = calcCharge(battery); // calcCharge needs 2B implemented
      battery.can_prim.write(battery.msg); 
    }break;

    case Powertrain_Cooling:{
      battery.msg.buf[0] = (int)(battery.fanRpm[0] / 50);
      battery.msg.buf[1] = (int)(battery.fanRpm[1] / 50);
      battery.msg.buf[2] = (int)(battery.fanRpm[2] / 50);
      battery.msg.buf[3] = 0; // --> PUMP SPEED TBD
      battery.msg.buf[4] = (int)(battery.acuTemp[0] / 2);
      battery.msg.buf[5] = (int)(battery.acuTemp[1] / 2);
      battery.msg.buf[6] = (int)(battery.acuTemp[2] / 2);
      battery.msg.buf[7] = (int)(battery.fans.readRegister(0x00)); // fan status --> to be implemented
      battery.can_prim.write(battery.msg); 
    }break;

    case Charging_Cart_Config:{
      uint16_t max_charge_current = battery.max_chrg_current;
      uint16_t max_charge_volt = battery.max_chrg_voltage;
      battery.msg.buf[0] = max_charge_current >> 8;
      battery.msg.buf[1] = max_charge_current;
      battery.msg.buf[2] = max_charge_volt >> 8;
      battery.msg.buf[3] = max_charge_volt;
      battery.msg.buf[4] = battery.state == CHARGE ? 1: 0; // not sure abt this one
      battery.msg.buf[5] = 0b0000000;
      battery.msg.buf[6] = 0b0000000;
      battery.msg.buf[7] = 0b0000000;
      battery.can_prim.write(battery.msg);
    }break;
      
      //this is unable to be sent reflexively, temporary fix, TODO: fix this
    case Expanded_Cell_Data: {
      //what's the point of this?
      // battery.msg.buf[0] = 0xA0; // 0b10100000;
      uint8_t cell_num = battery.msg.buf[1];
      if(battery.msg.id == Configure_Cell_Data)
        cell_num = battery.msg.buf[1];
      if(cell_num > 128){
        Serial.println("CAN send Expanded Cell Error: Invalid cell number");
        cell_num = 128;
      }
      uint16_t cell_volt = condenseVoltage(cell_num);
      uint16_t open_cell_volt = 0b0000000; // what's this? No idea yet
      uint16_t cell_temp = condenseTemperature(cell_num);
      battery.msg.buf[1] = cell_volt >> 8;
      battery.msg.buf[2] = cell_volt;
      battery.msg.buf[3] = open_cell_volt >> 8;
      battery.msg.buf[4] = open_cell_volt;
      battery.msg.buf[5] = cell_temp >> 8;
      battery.msg.buf[6] = cell_temp;
      battery.msg.buf[7] = 0b0000000; // TBD
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
      battery.msg.buf[0] = condenseVoltage(battery.cellVoltage[i * 8 + 0]);
      battery.msg.buf[1] = condenseVoltage(battery.cellVoltage[i * 8 + 1]);
      battery.msg.buf[2] = condenseVoltage(battery.cellVoltage[i * 8 + 2]);
      battery.msg.buf[3] = condenseVoltage(battery.cellVoltage[i * 8 + 3]);
      battery.msg.buf[4] = condenseVoltage(battery.cellVoltage[i * 8 + 4]);
      battery.msg.buf[5] = condenseVoltage(battery.cellVoltage[i * 8 + 5]);
      battery.msg.buf[6] = condenseVoltage(battery.cellVoltage[i * 8 + 6]);
      battery.msg.buf[7] = condenseVoltage(battery.cellVoltage[i * 8 + 7]);
      battery.can_prim.write(battery.msg);
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
      battery.msg.buf[0] = condenseTemperature((battery.cellTemp[i * 16 + 0] + battery.cellTemp[i * 16 + 1]) / 2);
      battery.msg.buf[1] = condenseTemperature((battery.cellTemp[i * 16 + 2] + battery.cellTemp[i * 16 + 3]) / 2);
      battery.msg.buf[2] = condenseTemperature((battery.cellTemp[i * 16 + 4] + battery.cellTemp[i * 16 + 5]) / 2);
      battery.msg.buf[3] = condenseTemperature((battery.cellTemp[i * 16 + 6] + battery.cellTemp[i * 16 + 7]) / 2);
      battery.msg.buf[4] = condenseTemperature((battery.cellTemp[i * 16 + 8] + battery.cellTemp[i * 16 + 9]) / 2);
      battery.msg.buf[5] = condenseTemperature((battery.cellTemp[i * 16 + 10] + battery.cellTemp[i * 16 + 11]) / 2);
      battery.msg.buf[6] = condenseTemperature((battery.cellTemp[i * 16 + 12] + battery.cellTemp[i * 16 + 13]) / 2);
      battery.msg.buf[7] = condenseTemperature((battery.cellTemp[i * 16 + 14] + battery.cellTemp[i * 16 + 15]) / 2);
      battery.can_prim.write(battery.msg);
    }
      break;
      
    case ACU_Ping_Response:
      battery.can_prim.write(battery.msg);
      break;
      
    case Charger_Control:
      battery.msg.buf[0] = battery.max_chrg_current >> 8;
      battery.msg.buf[1] = battery.max_chrg_current;
      battery.msg.buf[2] = battery.max_chrg_voltage >> 8;
      battery.msg.buf[3] = battery.max_chrg_voltage;
      battery.msg.buf[4] = battery.state == CHARGE ? 1:0; 
      battery.msg.buf[5] = 0b0000000;
      battery.msg.buf[6] = 0b0000000; 
      battery.msg.buf[7] = 0b0000000; 
      battery.can_chgr.write(battery.msg);
      break;
      
    default:
      Serial.println("FUCK U U IDIOT"); // language, sheesh
  }
  battery.msg.id = ID;
}



#endif