#ifndef CAN_H
#define CAN_H

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <canID.h>
#include "ACU.h"


void parseCANData(Battery &battery);
void readCANData(Battery &battery);
void sendCANData(Battery &battery, uint32_t ID);



void parseCANData(Battery &battery){
  switch(battery.msg.id){
    case Configure_Cell_Data:
      //STUFFFFF
      break;

    case ACU_Control:
      //STUFFFFF
      break;

    case Battery_Limits:
      //STUFFFFF
      break;

    case ACU_Ping_Request:
      sendCANData(battery, ACU_Ping_Response);
      break;

    case Charging_SDC_Ping_Response:
      //STUFFFFF
      break;

    case Charging_SDC_States:
      //STUFFFFF
      break;
      
    case Charger_Data:
      //STUFFFFF
      break;

    default:
      Serial.println("lol no message here for ya\t\t\t\t\t\t\t\t\tFucker");
  }
}

void readCANData(Battery &battery){
  int msgReads = 5;  //Max number of CAN message reads per function call

  for(; msgReads >= 0; msgReads--){
    if(!battery.can_prim.read(battery.msg))
      break;
    sortCANData(battery);
  }

  for(; msgReads >= 0; msgReads--){
    if(!battery.can_chgr.read(battery.msg))
      break;
    sortCANData(battery);
  }

}

void sendCANData(Battery &battery, uint32_t ID){
  switch(ID){
    case ACU_General:
      //STUFFFFF
      break;

    case ACU_General2:
      //STUFFFFF
      break;

    case Powertrain_Cooling:
      //STUFFFFF
      break;

    case Charging_Cart_Config:
      //STUFFFFF
      break;
      
    case Expanded_Cell_Data:
      //STUFFFFF
      break;
      
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
    case Condensed_Cell_Voltage_n136:
      uint8_t i = ID - Condensed_Cell_Voltage_n0;
      battery.msg.buf[0] = condenseVoltage(battery.cellVoltage[i * 8 + 0]);
      battery.msg.buf[1] = condenseVoltage(battery.cellVoltage[i * 8 + 1]);
      battery.msg.buf[2] = condenseVoltage(battery.cellVoltage[i * 8 + 2]);
      battery.msg.buf[3] = condenseVoltage(battery.cellVoltage[i * 8 + 3]);
      battery.msg.buf[4] = condenseVoltage(battery.cellVoltage[i * 8 + 4]);
      battery.msg.buf[5] = condenseVoltage(battery.cellVoltage[i * 8 + 5]);
      battery.msg.buf[6] = condenseVoltage(battery.cellVoltage[i * 8 + 6]);
      battery.msg.buf[7] = condenseVoltage(battery.cellVoltage[i * 8 + 7]);
      battery.msg.id = Condensed_Cell_Voltage_n0 + i;
      battery.msg.flags.extended = true;
      battery.can_prim.write(battery.msg);
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
    case Condensed_Cell_Temp_n136:
      uint8_t i = ID - Condensed_Cell_Temp_n0;
      battery.msg.buf[0] = condenseTemperature((battery.cellTemp[i * 16 + 0] + battery.cellTemp[i * 16 + 1]) / 2);
      battery.msg.buf[1] = condenseTemperature((battery.cellTemp[i * 16 + 2] + battery.cellTemp[i * 16 + 3]) / 2);
      battery.msg.buf[2] = condenseTemperature((battery.cellTemp[i * 16 + 4] + battery.cellTemp[i * 16 + 5]) / 2);
      battery.msg.buf[3] = condenseTemperature((battery.cellTemp[i * 16 + 6] + battery.cellTemp[i * 16 + 7]) / 2);
      battery.msg.buf[4] = condenseTemperature((battery.cellTemp[i * 16 + 8] + battery.cellTemp[i * 16 + 9]) / 2);
      battery.msg.buf[5] = condenseTemperature((battery.cellTemp[i * 16 + 10] + battery.cellTemp[i * 16 + 11]) / 2);
      battery.msg.buf[6] = condenseTemperature((battery.cellTemp[i * 16 + 12] + battery.cellTemp[i * 16 + 13]) / 2);
      battery.msg.buf[7] = condenseTemperature((battery.cellTemp[i * 16 + 14] + battery.cellTemp[i * 16 + 15]) / 2);
      battery.msg.id = ID;
      battery.msg.flags.extended = true;
      battery.can_prim.write(battery.msg);
      break;
      
    case ACU_Ping_Response:
      battery.msg.id = ACU_Ping_Response;
      battery.msg.flags.extended = true;
      battery.can_prim.write(battery.msg);
      break;
      
    case Charger_Control:
      //STUFFFFF
      break;
      
    default:
      Serial.println("FUCK U U IDIOT");
  }
}



#endif