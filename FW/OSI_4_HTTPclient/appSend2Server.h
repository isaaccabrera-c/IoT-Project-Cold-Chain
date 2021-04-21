#ifndef APP_SEND2SERVER_h
#define APP_SEND2SERVER_h

/* Required Libraries */
#include <Arduino.h>
#include "myCredentials.h"
#include <HTTPClient.h>
#include "stdint.h"
#include <WiFi.h>

/* Defines */
#define NUM_JSON_OBJ            32                      //Number of json objects that fit into the json string
#define CHARS_PER_JSON_OBJ      47                      //Number of characters per json object
#define NUM_OBJ_SEPARATORS      (NUM_JSON_OBJ - 1)      //Commas that separate json objects
#define NUM_LIST_BRACKETS       2                       //Openning + Closing brackets
   
/*                              Total number of characters that make up the json string */
#define TOTAL_JSON_CHARS        (NUM_LIST_BRACKETS + NUM_OBJ_SEPARATORS + CHARS_PER_JSON_OBJ*NUM_JSON_OBJ)

/*                              Offset of each Json object inside the Json string (starting on object 0) */
#define JSON_OBJ_OFFSET(n)      ( 1 + (CHARS_PER_JSON_OBJ + 1)*(n) )

/*                              Offsets of the different sections inside a json object */
#define TRAVELID_OFFSET(n)      ( JSON_OBJ_OFFSET(n) +  6 )
#define SENSORID_OFFSET(n)      ( JSON_OBJ_OFFSET(n) + 17 )
#define TEMPTURE_OFFSET(n)      ( JSON_OBJ_OFFSET(n) + 26 )
#define TIMESTMP_OFFSET(n)      ( JSON_OBJ_OFFSET(n) + 37 )

/* URL of the API Gateway */
#define API_GATEWAY_URL "https://ij60i8kpw0.execute-api.us-east-1.amazonaws.com/prod/api/coldchain"


/* Class to manage the communication with API Gateway */
class GatewayInterposer
{
public:
   /*!
    * @brief Object constructor
                Initializes attributes
    * @return Returns object handler
    */
    GatewayInterposer(void);
    
   /*!
    * @brief Reset the json string into its default value ("exampleJsonString")
    * @return void
    */
    void reset(void);
   /*!
    * @brief Print Json string into Serial console
    * @return void
    */
    void printJson(void);
    
   /*!
    * @brief Get current size of json string
    * @return Returns the number of active objects inside the json string
    */
    uint8_t getSize(void);        
    
    /*!
    * @brief Clip json string into containing the specifiedd number of objects
    * @param size - number of json objects to be contained inside the list
    * @return void
    */
    void setSize(uint8_t size);
   /*!
    * @brief Write the provided data into the TravelID field of the specified json object (starting on object 0)
    * @param objNum - Json object to be written on
    * @param travelID - ID to be pronted into travelID field of the selected json object
    * @return void
    */
    void setTravelID(uint8_t objNum, uint16_t travelID);
   /*!
    * @brief Write the provided data into the SensorID field of the specified json object (starting on object 0)
    * @param objNum - Json object to be written on
    * @param sensorID - ID to be pronted into sensorID field of the selected json object
    * @return void
    */
    void setSensorID(uint8_t objNum, uint8_t  sensorID);
   /*!
    * @brief Write the provided data into the Temperature field of the specified json object (starting on object 0)
    * @param objNum - Json object to be written on
    * @param tempture - ID to be pronted into tempture field of the selected json object
    * @return void
    */
    void setTempture(uint8_t objNum, uint16_t tempture);
   /*!
    * @brief Write the provided data into the Timestamp field of the specified json object (starting on object 0)
    * @param objNum - Json object to be written on
    * @param timestmp - ID to be pronted into timestmp field of the selected json object
    * @return void
    */
    void setTimestmp(uint8_t objNum, uint32_t timestmp);
    
    /*!
    * @brief Send json string into API Gateway
    * @return void
    */
    uint8_t post(void);
    
    /*!
    * @brief Print an 8-bit variable into a HEX string (without '0x') in the provided destination
    * @param data - 8-bit data to be written into a hex string (2 hexadecimal characters)
    * @destination - address of memory space to write the hexstring into
    * @return void
    */
    void printHEX8(uint8_t data, char* destination);
    /*!
    * @brief Print an 16-bit variable into a HEX string (without '0x') in the provided destination
    * @param data - 16-bit data to be written into a hex string (4 hexadecimal characters)
    * @destination - address of memory space to write the hexstring into
    * @return void
    */
    void printHEX16(uint16_t data, char* destination);
    /*!
    * @brief Print an 32-bit variable into a HEX string (without '0x') in the provided destination
    * @param data - 32-bit data to be written into a hex string (8 hexadecimal characters)
    * @destination - address of memory space to write the hexstring into
    * @return void
    */
    void printHEX32(uint32_t data, char* destination);
    
private:
    /* Pointer to the beginning of json string */
    char* _JsonStr_ptr;
    /* Number of active json objects in string */
    uint8_t _size;
    /* htpp handler for communication with API Gateway */
    HTTPClient _http;
};

#endif