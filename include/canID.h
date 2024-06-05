
#define Configure_Cell_Data 0x64                //Index 0
#define ACU_Control 0x66                        //Index 2
#define Battery_Limits 0x67                     //Index 3
#define ACU_Ping_Request 0x95                   //Index 4
#define ACU_General 0x96                        //Index 5
#define ACU_General2 0x97                       //Index 6
#define Powertrain_Cooling 0x98                 //Index 7
#define Charging_Cart_Config 0x99               //Index 8
#define Expanded_Cell_Data 0xA0                 //Index 9
#define Condensed_Cell_Voltage_n0 0xA1          //Index 10
#define Condensed_Cell_Voltage_n8 0xA2          //Index 11
#define Condensed_Cell_Voltage_n16 0xA3         //Index 12
#define Condensed_Cell_Voltage_n24 0xA4         //Index 13
#define Condensed_Cell_Voltage_n32 0xA5         //Index 14
#define Condensed_Cell_Voltage_n40 0xA6         //Index 15
#define Condensed_Cell_Voltage_n48 0xA7         //Index 16
#define Condensed_Cell_Voltage_n56 0xA8         //Index 17
#define Condensed_Cell_Voltage_n64 0xA9         //Index 18
#define Condensed_Cell_Voltage_n72 0xAA         //Index 19
#define Condensed_Cell_Voltage_n80 0xAB         //Index 20
#define Condensed_Cell_Voltage_n88 0xAC         //Index 21
#define Condensed_Cell_Voltage_n96 0xAD         //Index 22
#define Condensed_Cell_Voltage_n104 0xAE        //Index 23
#define Condensed_Cell_Voltage_n112 0xAF        //Index 24
#define Condensed_Cell_Voltage_n120 0xB0        //Index 25
#define Condensed_Cell_Voltage_n128 0xB1        //Index 26
#define Condensed_Cell_Voltage_n136 0xB2        //Index 27
#define Condensed_Cell_Temp_n0 0xB3             //Index 28
#define Condensed_Cell_Temp_n8 0xB4             //Index 29
#define Condensed_Cell_Temp_n16 0xB5            //Index 30
#define Condensed_Cell_Temp_n24 0xB6            //Index 31
#define Condensed_Cell_Temp_n32 0xB7            //Index 32
#define Condensed_Cell_Temp_n40 0xB8            //Index 33
#define Condensed_Cell_Temp_n48 0xB9            //Index 34
#define Condensed_Cell_Temp_n56 0xBA            //Index 35
#define Condensed_Cell_Temp_n64 0xBB            //Index 36
#define Condensed_Cell_Temp_n72 0xBC            //Index 37
#define Condensed_Cell_Temp_n80 0xBD            //Index 38
#define Condensed_Cell_Temp_n88 0xBE            //Index 39
#define Condensed_Cell_Temp_n96 0xBF            //Index 40
#define Condensed_Cell_Temp_n104 0xC0           //Index 41
#define Condensed_Cell_Temp_n112 0xC1           //Index 42
#define Condensed_Cell_Temp_n120 0xC2           //Index 43
#define Condensed_Cell_Temp_n128 0xC3           //Index 44
#define Condensed_Cell_Temp_n136 0xC4           //Index 45
#define ACU_Ping_Response 0xC7                  //Index 46


#define Charging_SDC_Ping_Request 0x14000       //Index 119
#define Charging_SDC_Ping_Response 0x14001      //Index 120
#define Charging_SDC_States 0x14002             //Index 121
#define Charger_Data 0x18FF50E5                 //Index 122
#define Charger_Control 0x1806E5F4              //Index 123

#define IMD_General 0x18FF01F4                      // Index 125: PGN #: 65281
// #define IMD_Isolation_Detail 0x38              // Index 126
#define IMD_Voltage 0x39                       // Index 127
// #define IMD_IT_System 0x3A                     // Index 128
#define IMD_Request 0x18EFF4FE // 	18EFF4FE       Index 129, pretty much only used for HV
#define IMD_Response 0x18EF00FE // MAYBE?      // Index 130
#define IMD_HV 0x5E                           // Index 94

#define IMD_HV_OFFSET 1606.4