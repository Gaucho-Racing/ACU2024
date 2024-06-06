#include "can.h"
#include "IMD.h"

void acuControl(const CAN_message_t &msg){
   if (msg.id == ACU_Control) {
    if(state == STANDBY && msg.buf[0]){
      state = PRECHARGE;
      Serial.println("PRECHARGE!!!!!!!!!");
    }
    else if(msg.buf[0]==0){
      state = SHUTDOWN;
      Serial.println("SHUTDOWN!!!!!!!!!");
      acu.errs = 0; // clear errors;
    }
  }
}

//TRIAGE 2: figure out
void mailboxSetup(){
  // can_prim.setMaxMB(1);
  // can_prim.setMB(MB0, RX, EXT);
  // can_prim.setMBFilter(REJECT_ALL);
  // can_prim.enableMBInterrupts();
  // can_prim.onReceive(MB0, acuControl);
  // can_prim.setMBUserFilter(MB0, 0x66, 0xFF);
  // can_prim.setMBFilter(ACCEPT_ALL);
  // can_prim.mailboxStatus();
}

void updateCAN(){
  readCANData();
  dumpCANbus();
}

void sendCANData(uint32_t ID){
  //moved CAN id set to bottom for temp config cell data fix, see line 134
  msg.flags.extended = true;
  msg.id = ID;
  switch(ID){
    case ACU_General:{
      uint16_t accVolt = battery.getTotalVoltage();
      int16_t accCurr = acu.getTsCurrent(false) * 100;
      msg.buf[0] = accVolt >> 8;
      msg.buf[1] = accVolt;
      msg.buf[2] = accCurr >> 8;
      msg.buf[3] = accCurr;
      int16_t tempCodeSend = (int16_t)(battery.maxCellTemp * 100);
      msg.buf[4] = tempCodeSend >> 8;
      msg.buf[5] = tempCodeSend;
      msg.buf[6] = acu.errs;
      msg.buf[7] = acu.warns;
      can_prim.write(msg);
    }break;

    case ACU_General2:{
      uint16_t tsVoltage = uint16_t(acu.getTsVoltage(false) * 100);
      msg.buf[0] = tsVoltage >> 8;
      msg.buf[1] = tsVoltage;
      uint8_t relayState = acu.getRelayState();
      msg.buf[2] = 0;
      if (relayState & 0b100) msg.buf[2] |= MASK_CAN_AIR_POS;
      if (relayState & 0b010) msg.buf[2] |= MASK_CAN_AIR_NEG;
      if (state == PRECHARGE) {
        msg.buf[2] |= MASK_CAN_PRECHARGE;
        if (relayState & 0b100) msg.buf[2] |= MASK_CAN_PRECHARGE_DONE;
      }
      if (state == SHUTDOWN ) msg.buf[2] |= MASK_CAN_SHUTDOWN;
      int16_t tempCodeSend = (int16_t)(battery.maxBalTemp * 100);
      msg.buf[3] = tempCodeSend>>8;
      msg.buf[4] = tempCodeSend;
      msg.buf[5] = acu.ACU_ADC.readRaw(ADC_MUX_SHDN_POW) >> 4;
      msg.buf[6] = acu.ACU_ADC.readRaw(ADC_MUX_GLV_VOLT) >> 4;
      msg.buf[7] = battery.updateSOC();
      can_prim.write(msg); 
    }break;

    case Powertrain_Cooling:{
      // TODO: fix
      msg.buf[0] = (acu.fanRpm[0] / 50);
      msg.buf[1] = (acu.fanRpm[1] / 50);
      msg.buf[2] = (acu.fanRpm[2] / 50);
      msg.buf[3] = 0; // --> PUMP SPEED TBD
      msg.buf[4] = (acu.getTemp1(false) / 2);
      msg.buf[5] = (acu.getTemp2(false) / 2);
      msg.buf[6] = 0;
      msg.buf[7] = (acu.fans.readRegister(FAN_ERRS_addr));
      can_prim.write(msg); 
    }break;

    case Charging_Cart_Config:{
      uint16_t max_charge_current = uint16_t(battery.max_chrg_current);
      uint16_t max_charge_volt = battery.max_chrg_voltage * 100;
      msg.buf[0] = max_charge_current >> 8;
      msg.buf[1] = max_charge_current;
      msg.buf[2] = max_charge_volt >> 8;
      msg.buf[3] = max_charge_volt;
      msg.buf[4] = state == CHARGE ? 1 : 0; // not sure abt this one
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
      uint16_t cell_temp = condenseTemperature(battery.cellTemp[cell_num], 0);
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
      msg.buf[0] = condenseTemperature(battery.cellTemp[i * 16 + 0], battery.cellTemp[i * 16 + 1]);
      msg.buf[1] = condenseTemperature(battery.cellTemp[i * 16 + 2], battery.cellTemp[i * 16 + 3]);
      msg.buf[2] = condenseTemperature(battery.cellTemp[i * 16 + 4], battery.cellTemp[i * 16 + 5]);
      msg.buf[3] = condenseTemperature(battery.cellTemp[i * 16 + 6], battery.cellTemp[i * 16 + 7]);
      msg.buf[4] = condenseTemperature(battery.cellTemp[i * 16 + 8], battery.cellTemp[i * 16 + 9]);
      msg.buf[5] = condenseTemperature(battery.cellTemp[i * 16 + 10], battery.cellTemp[i * 16 + 11]);
      msg.buf[6] = condenseTemperature(battery.cellTemp[i * 16 + 12], battery.cellTemp[i * 16 + 13]);
      msg.buf[7] = condenseTemperature(battery.cellTemp[i * 16 + 14], battery.cellTemp[i * 16 + 15]);
      can_prim.write(msg);
    }
      break;
      
    case ACU_Ping_Response:
      can_prim.write(msg);
      // Serial.println("ping");
      break;
      
    case Charger_Control: {
      //TRIAGE 0: check b4 use
      uint16_t code = battery.max_chrg_voltage*10;
      msg.buf[0] = code >> 8;
      msg.buf[1] = code & 0xFF;
      code = battery.max_chrg_current*10;
      msg.buf[2] = code >> 8;
      msg.buf[3] = code & 0xFF;
      msg.buf[4] = state == CHARGE ? 1:0; 
      msg.buf[5] = 0b0000000;
      msg.buf[6] = 0b0000000; 
      msg.buf[7] = 0b0000000; 
      can_chgr.write(msg);
    }break;
    case IMD_Request: 
      msg.buf[0] = 0x5E; // index for Voltage: HV system
      msg.buf[1] = 0xFF; 
      msg.buf[2] = 0xFF;
      msg.buf[3] = 0xFF;
      msg.buf[4] = 0xFF;
      msg.buf[5] = 0xFF;
      msg.buf[6] = 0xFF;
      msg.buf[7] = 0xFF;
      can_chgr.write(msg);
      break;
    default:
      Serial.printf("Unknown CAN ID: %lu\n", msg.id);
      can_chgr.write(msg);
  }
  msg.id = ID;
}

void parseCANData(){
  // Serial.printf("Parsing CAN Data's msg.id: %lu => ", msg.id);
  // Serial.print(msg.id, HEX);
  // Serial.println();
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
      if(state == STANDBY && msg.buf[0] == 1){
        state = PRECHARGE;
        D_L1("Entering Precharge YAAAAYYYY");
      } else if(msg.buf[0] != 1) {
        state = SHUTDOWN;
        acu.errs = 0; // reset errors
        D_L1("Entering SHUTDOWN BOOOOOOOOOOOOO LAAAME");
      }
      break;


    case Battery_Limits:
      battery.max_chrg_voltage = ((uint16_t(msg.buf[0]) << 8) | msg.buf[1])*0.01;
      battery.max_output_current = ((uint16_t(msg.buf[2]) << 8) | msg.buf[3])*0.01;
      battery.cell_OT_Threshold = ((uint16_t(msg.buf[4]) << 8) | msg.buf[5])*0.01;
      battery.max_chrg_current = ((uint16_t(msg.buf[6]) << 8) | msg.buf[7])*0.01;

      break;

    case ACU_Ping_Request:
      sendCANData(ACU_Ping_Response);
      break;

    case Charging_SDC_Ping_Response:
      sendCANData(Charging_SDC_Ping_Request);
      break;

    case Charging_SDC_States:
      //FILLER TODO
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
      if(state == STANDBY) state = CHARGE;
      if(state == CHARGE){
        acu.updateChgrRecieveTime();
      } 
      // parse the max voltage, max current & chaging/not charging bool & get all failures
      battery.max_chrg_voltage = ((msg.buf[0] << 8) | msg.buf[1]) * 0.1;
      battery.max_chrg_current = ((msg.buf[2] << 8) | msg.buf[3]) * 0.1;
      // battery.chargerDataStatus.hardwareStatus = msg.buf[4] & ERR_Hardware;
      // battery.chargerDataStatus.temperatureStatus = msg.buf[4] & ERR_Temp;
      // battery.chargerDataStatus.inputVoltageStatus = msg.buf[4] & ERR_InputVolt;
      // battery.chargerDataStatus.startingState = msg.buf[4] & ERR_Start;
      // battery.chargerDataStatus.communicationState = msg.buf[4] & ERR_Comm;
      break;
    
    case IMD_General:
      // Serial.println("IMD General DEBUG PRINTS (*****************************************)");
      acu.setRIsoCorrected((uint16_t(msg.buf[0]) << 8) | (msg.buf[1]));
      acu.setRIsoStatus(msg.buf[2]);
      acu.setIsoMeasCount(msg.buf[3]);
      acu.setStatusDeviceActivity(msg.buf[6]); 
      //acu.setStatusDeviceActivity((uint16_t(msg.buf[4]) << 8) | (msg.buf[5]));
      //last byte is don't care
      break;
    
    case IMD_Response: 
      // Serial.println("IMD Response DEBUG PRINTS (*****************************************)");
      // Serial.print("Data: ");
      // for(int i = 0; i < 8; i++){
      //   Serial.print(i);
      //   Serial.print(msg.buf[i], HEX);
      //   Serial.print(" "); }
      // Serial.println();
      if(msg.buf[0] == IMD_HV){
        uint16_t temp = (msg.buf[1] << 8) | (msg.buf[2]);
        acu.setIMDHV(temp*0.05 - IMD_HV_OFFSET);
      }
    break;
    default:
      // Serial.println("lol no message here for ya\t\t\t\t\t\t\t\t\tFucker");
      break;
  }
}
uint64_t prev_millis = 0;
//Triage 2: replace with mailboxes
int readCANData(){
  // Serial.println(millis()-prev_millis);
  // prev_millis = millis();
  int maxReads = 8;  //Max number of CAN message reads per function call
  for(int i = 0; i < maxReads; i++){
    if(can_prim.read(msg)){
      parseCANData();
    }
  }
  
  for(int i = 0; i < maxReads; i++){
    if(can_chgr.read(msg)){
      // Serial.println("Can_chgr reads: "); Serial.print(msg.id, HEX); Serial.println();
      // Serial.print("Data: ");
      // for(int i = 0; i < 8; i++){
      //   Serial.print(i);
      //   Serial.print(msg.buf[i], HEX);
      //   Serial.print(" ");
      // }
      // Serial.println();
      parseCANData();
    }
  }
  return 1;
}

//TRIAGE 2: replace with interrupt/mailboxes
static uint32_t prevmillis = 0;

/// @brief Sends data to CANbus
/// @param[in] battery
/// @return None
void dumpCANbus() {
  if(millis() - prevmillis > 10){
    prevmillis = millis();
    for (uint8_t i = 0; i < TOTAL_IC * 2; i++) {
      sendCANData(Condensed_Cell_Voltage_n0 + i);
      sendCANData(Condensed_Cell_Temp_n0 + i);
    }
    sendCANData(ACU_General);
    sendCANData(ACU_General2);
    sendCANData(Powertrain_Cooling);
    sendCANData(Charging_Cart_Config);
    sendCANData(IMD_Request); // request IMD data, needs idx to specify what we need
    // sendCANData(IMD_Isolation_Detail);
    // sendCANData(IMD_Voltage);
    // sendCANData(IMD_IT_System);
  }
}