#include "appSend2Server.h"
/*
    "{"
        "'V':'WXYZ',"       //Travel ID     = 2 byte = 4 characters
        "'S':'YZ',"         //Sensor ID     = 1 byte = 2 characters
        "'C':'WXYZ',"       //Temeprature   = 2 byte = 4 characters
        "'T':'STUVWXYZ'"    //Timestamp     = 4 byte = 8 characters
    "}"
*/


/* Example Json Object */
static const char exampleJsonString[CHARS_PER_JSON_OBJ+1] = "{\"V\":\"CAFE\",\"S\":\"00\",\"C\":\"0000\",\"T\":\"000FF000\"}";    
/* String to convert hex into str */
static const char hex2str[] = "0123456789ABCDEF";

/* Reserve memory for Jason string */
static char myJsonString[TOTAL_JSON_CHARS+1];




GatewayInterposer::GatewayInterposer(void)
{
    /* Initialize http handler */
    _http.begin(API_GATEWAY_URL);
    _http.addHeader("Content-Type", "application/json");
    /* Reset Json string */
    reset();
}
void GatewayInterposer::reset(void)
{
    /* Initialize pointer to Json string */
    _JsonStr_ptr = myJsonString;
    
    /* Add the list opening bracket */
    *_JsonStr_ptr = '[';    _JsonStr_ptr++;
    
    /* Add all Json objsects as in example */
    for(uint8_t i = 0; i < NUM_JSON_OBJ; i++)
    {
        /* Copy the example */
        for(uint8_t j = 0; j < CHARS_PER_JSON_OBJ; j++)
        {
            *_JsonStr_ptr = exampleJsonString[j];    _JsonStr_ptr++;
        }
        
        /* Add a separating comma */
        if(NUM_OBJ_SEPARATORS > i)
        {
            *_JsonStr_ptr = ',';    _JsonStr_ptr++;
        }
    }
    
    /* Add the list closing bracket */
    *_JsonStr_ptr = ']';    _JsonStr_ptr++;
    
    /* Add a null character to close the string */
    *_JsonStr_ptr = '\0';    _JsonStr_ptr++;
    
    /* Reset pointer to Json string */
    _JsonStr_ptr = myJsonString;
    
    /* Set size to 0 */
    setSize(0);
}

uint8_t GatewayInterposer::getSize(void)
{
    return _size;
}

void GatewayInterposer::setSize(uint8_t size)
{
    /* Set the beginning of json string to '[' */
    _JsonStr_ptr[0] = '[';
    
    /* Set all beginings of objects to '{' */
    for(uint8_t i = 0; i < NUM_JSON_OBJ; i++)
    {
        _JsonStr_ptr[ JSON_OBJ_OFFSET(i) ] = '{';
    }
    
    /* Set all separators to ',' */
    for(uint8_t i = 1; i < NUM_JSON_OBJ; i++)
    {
        _JsonStr_ptr[ JSON_OBJ_OFFSET(i) - 1 ] = ',';
    }
    
    if(0 == size)
    {
        /* Special case: print '!' */
        _JsonStr_ptr[0] = '!';
        _JsonStr_ptr[1] = '\0';
    }
    else
    {
        /* Set the next comma to a closing bracket */
        _JsonStr_ptr[ JSON_OBJ_OFFSET(size) - 1 ] = ']';
        
        /* Set the next begining of object to a null character */
        _JsonStr_ptr[ JSON_OBJ_OFFSET(size) ] = '\0';
    }
    
    /* Record size */
    _size = size;
}
void GatewayInterposer::setTravelID(uint8_t objNum, uint16_t travelID)
{
    /* Print travel ID into selected json object */
    printHEX16(travelID, TRAVELID_OFFSET(objNum) + myJsonString);
}
void GatewayInterposer::setSensorID(uint8_t objNum, uint8_t  sensorID)
{
    /* Print sensor ID into selected json object */
    printHEX8(sensorID, SENSORID_OFFSET(objNum) + myJsonString);
}
void GatewayInterposer::setTempture(uint8_t objNum, uint16_t tempture)
{
    /* Print temperature into selected json object */
    printHEX16(tempture, TEMPTURE_OFFSET(objNum) + myJsonString);
}
void GatewayInterposer::setTimestmp(uint8_t objNum, uint32_t timestmp)
{
    /* Print timestamp into selected json object */
    printHEX32(timestmp, TIMESTMP_OFFSET(objNum) + myJsonString);
}


uint8_t GatewayInterposer::post(void)
{
    return _http.POST(myJsonString);
}


void GatewayInterposer::printJson(void)
{
    Serial.println( _JsonStr_ptr );
}
void GatewayInterposer::printHEX8(uint8_t data, char* destination)
{
    uint8_t numNibbles = 2;
    
    for(uint8_t i = 0; i < numNibbles; i++)
    {
        destination[numNibbles - i - 1] = hex2str[0x0F & data];
        data = data >> 4;
    }
}
void GatewayInterposer::printHEX16(uint16_t data, char* destination)
{
    uint8_t numNibbles = 4;
    
    for(uint8_t i = 0; i < numNibbles; i++)
    {
        destination[numNibbles - i - 1] = hex2str[0x0F & data];
        data = data >> 4;
    }
}
void GatewayInterposer::printHEX32(uint32_t data, char* destination)
{
    uint8_t numNibbles = 8;
    
    for(uint8_t i = 0; i < numNibbles; i++)
    {
        destination[numNibbles - i - 1] = hex2str[0x0F & data];
        data = data >> 4;
    }
}
