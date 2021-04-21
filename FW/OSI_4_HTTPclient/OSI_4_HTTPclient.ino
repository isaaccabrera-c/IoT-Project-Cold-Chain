////////////////Libraries///////////////////////////////////////
#include "appSend2Server.h"
#include "EE24LC256.h"
#include "myCredentials.h"
#include "PCT2075.h"
#include <stdint.h>
#include <Wire.h>
////////////////////////////////////////////////////////////////


////////////////Macros//////////////////////////////////////////
/* Baud rate for serial communications */
#define UART_BAUDRATE       115200
/* Clockspeed for I2C communication */
#define I2C_CLK_SPEED_Hz    400000

/* Number of PCT2075 devices in circuit */
#define PCT2075_NUM_DEVICES 8
/* Sensor sampling period in milli-seconds */
#define SAMPLNG_PERIOD_ms   5000

/* EEPROM I2C device address */
#define EEPROM_DB_I2C_ADDR  0x50
/* Pin used for EEPROM WriteProtect security operations */
#define EEPROM_DB_WP_pin    15

/* Define Travel ID */
#define CURRENT_TRAVEL_ID   0xF00D
////////////////////////////////////////////////////////////////


////////////////Data Types//////////////////////////////////////
typedef struct
{
    /* Unix epoch (ms) */
    uint32_t timestamp_unix_epoch;
    /* Reading from a PCT2075 register */
    int16_t temp_reg_reading;
    /* Sensor ID */
    uint8_t PCT2075_device;
    /* Padding */
    uint8_t zeros;
} TempRecord_t;
////////////////////////////////////////////////////////////////


////////////////Global constants////////////////////////////////
/* List of I2C addresses of PCT2075 devices on circuit */
const uint8_t PCT2075_device_list[PCT2075_NUM_DEVICES] =    \
            {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F};
////////////////////////////////////////////////////////////////


////////////////Global variables////////////////////////////////
/* PCT2075 manager instance of management class */
PCT2075_Mngmt PCT2075_Mgr;
/* Single PCT2075 reading */
TempRecord_t singleTempRecord;
/* EE24LC256 instance */
EE24LC256 EEPROM_DB(EEPROM_DB_I2C_ADDR, EEPROM_DB_WP_pin);
/* EEPROM pointer */
uint16_t EEPROM_DB_ptr;
/* Json manager to communicate data to API Gateway */
GatewayInterposer Json2Gateway;
////////////////////////////////////////////////////////////////


////////////////Function Prototypes/////////////////////////////
/*!
* @brief Sample, print, and push sensor data into memory
* @return void
*/
void sample_and_push(void);
/*!
* @brief Pop, print, and send data from memory to Gateway
* @return void
*/
void pop_and_send(void);
////////////////////////////////////////////////////////////////


void setup()
{
    /* Setup UART */
    Serial.begin(UART_BAUDRATE);
    while(!Serial);
    
    /* Setup I2C */
    Wire.begin();
    Wire.setClock(I2C_CLK_SPEED_Hz);
    
    /* Setup Wifi */
    WiFi.begin(mySSID, myPSWD);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    
    // /* Test */
    // Json2Gateway.reset();
    // Json2Gateway.setSize(0);
    // Json2Gateway.printJson();
    // while(-1);
    
    /* Set pointer register of all listed PCT2075 devices to point towards Temperature register */
    Serial.println("Sensor Setup:   BEGIN");
    uint8_t sensor_setup_cc = 0;
    for(uint8_t i = 0; i < PCT2075_NUM_DEVICES; i++)
    {
        /* Try to setup sensor */
        sensor_setup_cc = PCT2075_Mgr.point2TempReg( PCT2075_device_list[i] );
        
        /* Log failure/success */
        Serial.print("  Sensor 0x");
        Serial.print(PCT2075_device_list[i], HEX);
        Serial.print(" Setup: ");
        Serial.print( 0 == sensor_setup_cc ? "PASS" : "FAIL" );
        Serial.print("  cc = 0x");
        Serial.println(sensor_setup_cc, HEX);
    }
    Serial.println("Sensor Setup:   DONE");
    
    /* Erase memory */
    Serial.println("Memory Erase:   BEGIN");
    uint8_t memory_erase_cc = EEPROM_DB.erase();
    Serial.print("Memory Erase:   ");
    Serial.print( 0 == memory_erase_cc ? "PASS" : "FAIL" );
    Serial.print("  cc = 0x");
    Serial.println(memory_erase_cc, HEX);
    
    /* Initialize record */
    singleTempRecord.timestamp_unix_epoch   = millis();
    singleTempRecord.temp_reg_reading       = -1;
    singleTempRecord.PCT2075_device         = 0;
    singleTempRecord.zeros                  = 0;
}


void loop()
{
    /* Initialize EEPROM pointer to 0 */
    EEPROM_DB_ptr = 0;

    /* Sample, print, and push until signaled to stop */
    while( 180*1000 > millis() )
    {
        /* Wait until next sample time */
        delay(SAMPLNG_PERIOD_ms);
        /* Sample, print, and push densor data */
        sample_and_push();
    }
   

    /* Reset EEPROM pointer */
    EEPROM_DB_ptr = 0;
    /* Reset Json string */
    Json2Gateway.reset();
    

    /* Retrieve memory from */
    pop_and_send();
    
    
    // Dump memory (for test only)
    delay(10);
    uint8_t dump_cc = EEPROM_DB.dump();
    Serial.print("Dump cc = 0x");
    Serial.println(dump_cc, HEX);

    
    /* EOP */
    while(1);
}




void sample_and_push(void)
{
    /* Print separator */
    Serial.println("********************************");
    
    /* Iterate over all listed PCT2075 devices */
    for(uint8_t i = 0; i < PCT2075_NUM_DEVICES; i++)
    { 
        /* Fill in record */
        singleTempRecord.PCT2075_device         = PCT2075_device_list[i];
        singleTempRecord.temp_reg_reading       = PCT2075_Mgr.readTempReg( PCT2075_device_list[i] );
        singleTempRecord.timestamp_unix_epoch  += millis();
        
        /* Log */
        Serial.print("Sensor 0x");
        Serial.print(singleTempRecord.PCT2075_device, HEX);
        Serial.print(" : 0x");
        Serial.print(singleTempRecord.temp_reg_reading, HEX);
        Serial.print(" = ");
        Serial.print( PCT2075_Mgr.decodeTempReg(singleTempRecord.temp_reg_reading) );
        Serial.print(" °C");
        Serial.print(" at time ");
        Serial.print( singleTempRecord.timestamp_unix_epoch, DEC );
        Serial.println(" ms");
        
        /* Push into memory */
        EEPROM_DB.write(EEPROM_DB_ptr, 8, ((uint8_t*)(&singleTempRecord)) );
        EEPROM_DB_ptr += 8;
    }
}
void pop_and_send(void)
{
    /* Repeat until valid readings stop */
    while( 0 == singleTempRecord.zeros)
    {
        /* Retrieve data from memory */
        EEPROM_DB.read(EEPROM_DB_ptr, 8, ((uint8_t*)(&singleTempRecord)) );
        EEPROM_DB_ptr += 8;

        /* Break if data is not useful */
        if(0 != singleTempRecord.zeros) break;
        
        /* Log */
        Serial.print("Sensor 0x");
        Serial.print(singleTempRecord.PCT2075_device, HEX);
        Serial.print(" reported ");
        Serial.print( PCT2075_Mgr.decodeTempReg(singleTempRecord.temp_reg_reading) );
        Serial.print("°C at time ");
        Serial.print( singleTempRecord.timestamp_unix_epoch, DEC );
        Serial.println(" ms");
        
        /* Flush json file if full */
        if(Json2Gateway.getSize() == NUM_JSON_OBJ)
        {
            // /* Send Json to API Gateway */
            Json2Gateway.post();
            /* Print Json string*/
            Json2Gateway.printJson();
            /* Reset Json string */
            Json2Gateway.reset();
        }
        
        /* Add data to Json string */
        uint8_t n = Json2Gateway.getSize();
        Json2Gateway.setTravelID(n, CURRENT_TRAVEL_ID);
        Json2Gateway.setSensorID(n, singleTempRecord.PCT2075_device);
        Json2Gateway.setTempture(n, singleTempRecord.temp_reg_reading);
        Json2Gateway.setTimestmp(n, singleTempRecord.timestamp_unix_epoch);
        Json2Gateway.setSize( n + 1 );
    }
    
    /* Flush json file if not empty */
    if(Json2Gateway.getSize() != 0)
    {
        // /* Send Json to API Gateway */
        Json2Gateway.post();
        /* Print Json string*/
        Json2Gateway.printJson();
        /* Reset Json string */
        Json2Gateway.reset();
    }
}