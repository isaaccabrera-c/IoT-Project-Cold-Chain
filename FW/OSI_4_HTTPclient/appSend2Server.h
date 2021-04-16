#ifndef APP_SEND2SERVER_h
#define APP_SEND2SERVER_h

#include <Arduino.h>
#include "myCredentials.h"
#include <HTTPClient.h>
#include "stdint.h"
#include <WiFi.h>

//Defines
#define NUM_JSON_OBJ            32
#define CHARS_PER_JSON_OBJ      47
#define NUM_OBJ_SEPARATORS      (NUM_JSON_OBJ - 1)      //Commas
#define NUM_LIST_BRACKETS       2

#define TOTAL_JSON_CHARS        (NUM_LIST_BRACKETS + NUM_OBJ_SEPARATORS + CHARS_PER_JSON_OBJ*NUM_JSON_OBJ)


#define JSON_OBJ_OFFSET(n)      ( 1 + (CHARS_PER_JSON_OBJ + 1)*(n) )
#define TRAVELID_OFFSET(n)      ( JSON_OBJ_OFFSET(n) +  6 )
#define SENSORID_OFFSET(n)      ( JSON_OBJ_OFFSET(n) + 17 )
#define TEMPTURE_OFFSET(n)      ( JSON_OBJ_OFFSET(n) + 26 )
#define TIMESTMP_OFFSET(n)      ( JSON_OBJ_OFFSET(n) + 37 )




#endif

void TestJASON(void);



class JSONmgr
{
public:
    JSONmgr(void);
    void reset(void);
    void printJson(void);
    
    void setSize(uint8_t size);
    void setTravelID(uint8_t objNum, uint16_t travelID);
    void setSensorID(uint8_t objNum, uint8_t  sensorID);
    void setTempture(uint8_t objNum, uint16_t tempture);
    void setTimestmp(uint8_t objNum, uint32_t timestmp);
    
    uint8_t post(void);
    
    void printHEX8(uint8_t data, char* destination);
    void printHEX16(uint16_t data, char* destination);
    void printHEX32(uint32_t data, char* destination);
    
private:
    char* _JsonStr_ptr;
    uint8_t _size;
    HTTPClient _http;
};
