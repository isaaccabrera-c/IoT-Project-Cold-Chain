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





JSONmgr::JSONmgr(void)
{
    /* Reset Json string */
    reset();
    /* Initialize http handler */
    _http.begin("https://ij60i8kpw0.execute-api.us-east-1.amazonaws.com/prod/api/coldchain");
    _http.addHeader("Content-Type", "application/json");
}
void JSONmgr::reset(void)
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
}


void JSONmgr::setSize(uint8_t size)
{
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
    
    /* Set the next comma to a closing bracket */
    _JsonStr_ptr[ JSON_OBJ_OFFSET(size) - 1 ] = ']';
    
    /* Set the next begining of object to a null character */
    _JsonStr_ptr[ JSON_OBJ_OFFSET(size) ] = '\0';
    
    /* Record size */
    _size = size;
}
void JSONmgr::setTravelID(uint8_t objNum, uint16_t travelID)
{
    /* Print travel ID into selected json object */
    printHEX16(travelID, TRAVELID_OFFSET(objNum) + myJsonString);
}
void JSONmgr::setSensorID(uint8_t objNum, uint8_t  sensorID)
{
    /* Print sensor ID into selected json object */
    printHEX8(sensorID, SENSORID_OFFSET(objNum) + myJsonString);
}
void JSONmgr::setTempture(uint8_t objNum, uint16_t tempture)
{
    /* Print temperature into selected json object */
    printHEX16(tempture, TEMPTURE_OFFSET(objNum) + myJsonString);
}
void JSONmgr::setTimestmp(uint8_t objNum, uint32_t timestmp)
{
    /* Print timestamp into selected json object */
    printHEX32(timestmp, TIMESTMP_OFFSET(objNum) + myJsonString);
}


uint8_t JSONmgr::post(void)
{
    return _http.POST(myJsonString);
}


void JSONmgr::printJson(void)
{
    Serial.println( _JsonStr_ptr );
}
void JSONmgr::printHEX8(uint8_t data, char* destination)
{
    uint8_t numNibbles = 2;
    
    for(uint8_t i = 0; i < numNibbles; i++)
    {
        destination[numNibbles - i - 1] = hex2str[0x0F & data];
        data = data >> 4;
    }
}
void JSONmgr::printHEX16(uint16_t data, char* destination)
{
    uint8_t numNibbles = 4;
    
    for(uint8_t i = 0; i < numNibbles; i++)
    {
        destination[numNibbles - i - 1] = hex2str[0x0F & data];
        data = data >> 4;
    }
}
void JSONmgr::printHEX32(uint32_t data, char* destination)
{
    uint8_t numNibbles = 8;
    
    for(uint8_t i = 0; i < numNibbles; i++)
    {
        destination[numNibbles - i - 1] = hex2str[0x0F & data];
        data = data >> 4;
    }
}



void TestJASON(void)
{
    JSONmgr JsonTest;
    
    for(uint8_t j = 0; j < 64; j++) Serial.print("$");
    
    Serial.println("");
    Serial.print("Number of bytes in string: ");
    Serial.println( TOTAL_JSON_CHARS );
    
    // JsonTest.setTravelID(0, 0xCAFE);
    // JsonTest.setSensorID(0, 0x70);
    // JsonTest.setTempture(0, 0x1E00);
    // JsonTest.setTimestmp(0, ((uint32_t)( millis() )) );
    // JsonTest.setSize(1);
    JsonTest.printJson();
    uint8_t cc = 0;
    cc = JsonTest.post();
    Serial.println("");
    Serial.print("POST cc = 0x");
    Serial.println(cc,HEX);
    
    for(uint8_t k = 0; k < 64; k++) Serial.print("$");
    Serial.println("");
}