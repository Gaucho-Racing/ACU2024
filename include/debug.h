#ifndef DEBUG_H
#define DEBUG_H


#define DEBUG 1 //0, for off, 1 for normal print, 2 for verbose print
#if DEBUG > 1
    #define D_L1(statement) do{Serial.println(statement);} while(0);
    #define D_L2(statement) do{Serial.println(statement);} while(0);
#elif DEBUG == 1
    #define D_L1(statement) do{Serial.println(statement);} while(0);
    #define D_L2(statement) do {} while(0);
#else
    #define D_L1(statement) do {} while(0);
    #define D_L2(statement) do {} while(0);
#endif

void debug();

#endif