////////////////Libraries///////////////////////////////////////
#include "appSend2Server.h"
#include "EE24LC256.h"
#include "myCredentials.h"
#include "PCT2075.h"
#include <stdint.h>
#include <time.h>
#include <Wire.h>
////////////////////////////////////////////////////////////////


////////////////Macros//////////////////////////////////////////
/* Pin to signal success of different stages of process */
#define SIGNAL_LED          2


/* Baud rate for serial communications */
#define UART_BAUDRATE       115200
/* Clockspeed for I2C communication */
#define I2C_CLK_SPEED_Hz    400000

/* Number of PCT2075 devices in circuit */
#define PCT2075_NUM_DEVICES 8
/* Sensor sampling period in milli-seconds */
#define SAMPLNG_PERIOD_ms   (5*60*1000)

/* EEPROM I2C device address */
#define EEPROM_DB_I2C_ADDR  0x50
/* Pin used for EEPROM WriteProtect security operations */
#define EEPROM_DB_WP_pin    15

/* Define Travel ID */
#define CURRENT_TRAVEL_ID   0xABCD    

/* Samplin duration in ms */
#define TAVEL_DURATION      (10*3600*1000)
/* Conditional posting */
#define POST_2_SERVER_EN
/* Post delay to avoid errors */
#define POST_DELAY          500

/* Wifi connection timeout (ms) */
#define WIFI_TIMEOUT        5000
/* NTP server max retries */
#define NTP_MAX_RETRIES     16
/* HTTP POST retries */
#define HTTP_MAX_RETRIES    16
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
/* NTP server URL */
const char ntpServer [] = "time.google.com";//"pool.ntp.org";
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
/* Epoch offset to add to millis() for accurate time */
uint32_t epoch_offset;
/* Json manager to communicate data to API Gateway */
GatewayInterposer Json2Gateway;
/* Flag to signal if all internet transactions were successful */
uint8_t full_internet_success;
////////////////////////////////////////////////////////////////


////////////////Function Prototypes/////////////////////////////
/*!
* @brief Setup LSIO communications
* @return void
*/
void setup_LSIO(void);
/*!
* @brief Setup sensors and log the corresponding messages to serial console
* @return void
*/
void setup_sensors(void);
/*!
* @brief Erase and verify EEprom + log corresponding messages
* @return void
*/
void erase_memory(void);
/*!
* @brief Connect to a WiFi network
* @return 0 if success, non-0 if failure
*/
uint8_t connect_WiFi(char* ssid, char* pwd);
/*!
* @brief Connect to NTP server, get epoch, calculate offset, disconnect from ntp server
* @return void
*/
void synch_time(void);
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
/*!
* @brief Post Json string to API Gateway and log to serial console
    Note: Actual posting depends on conditional compilation
* @return void
*/
void post_2_APIgateway(void);
/*!
* @brief Dump the entire content of the EEPROM into the Serial console
* @return void
*/
void dump_memory(void);
////////////////////////////////////////////////////////////////


void setup()
{   
    /* Setup LED */
    pinMode(SIGNAL_LED, OUTPUT);
    digitalWrite(SIGNAL_LED, LOW);

    /* Setup UART and I2C */
    setup_LSIO();

    /* Setup Sensors */
    setup_sensors();
    
    /* Erase EEPROM */
    erase_memory();
    
    /* Assume success on all internet TXN's */
    full_internet_success = 0;
    
    /* Synch time */
    synch_time();

    // /* Test */
    // Serial.print("WL_CONNECTED = 0x");
    // Serial.println(WL_CONNECTED, HEX);
}


void loop()
{
    
    /* Read sensors and store data into EEPROM */
    sample_and_push();

    /* Retrieve memory from */
    pop_and_send();
    
    /* Dump memory */
    // dump_memory();
    
    /* Turn off WiFi */
    WiFi.mode(WIFI_OFF);
    Serial.println("");
    Serial.println("****WiFi:   OFF****");
    Serial.println("");
    
    /* EOP */
    while(-1)
    {
        digitalWrite(SAMPLNG_PERIOD_ms, (!full_internet_success) ? HIGH : LOW);
        delay(1000);
        digitalWrite(SAMPLNG_PERIOD_ms, LOW);
    }
}



void setup_LSIO(void)
{
    /* Setup UART */
    Serial.begin(UART_BAUDRATE);
    while(!Serial);
    Serial.println("");
    Serial.println("UART Setup:   DONE");
    
    
    /* Setup I2C */
    Serial.println("");
    Serial.println("I2C Setup:  BEGIN");
    Serial.print("    SCK @ ");
    Serial.print( ((float)(I2C_CLK_SPEED_Hz)) / 1000.0 );
    Serial.println(" kHz");
        Wire.begin();
        Wire.setClock(I2C_CLK_SPEED_Hz);
    Serial.println("I2C Setup:  DONE");
}
void setup_sensors(void)
{
    /* Signal start of process */
    Serial.println("");
    Serial.println("Sensor Setup:   BEGIN");
    
    /* Set pointer register of all listed PCT2075 devices to point towards Temperature register + retrieve cc */
    uint8_t sensor_setup_cc = 0;
    uint8_t pass_count = 0;
    uint8_t fail_count = 0;
    for(uint8_t i = 0; i < PCT2075_NUM_DEVICES; i++)
    {
        /* Try to setup sensor */
        sensor_setup_cc = PCT2075_Mgr.point2TempReg( PCT2075_device_list[i] );
        
        /* Increment count */
        pass_count += (0 == sensor_setup_cc) ? (1) : (0);
        fail_count += (0 == sensor_setup_cc) ? (0) : (1);
        
        /* Log failure/success */
        Serial.print("    Sensor 0x");
        Serial.print(PCT2075_device_list[i], HEX);
        Serial.print(" Setup: ");
        Serial.print( 0 == sensor_setup_cc ? "PASS" : "FAIL" );
        Serial.print("  cc = 0x");
        Serial.println(sensor_setup_cc, HEX);
    }
    
    /* Signal task completion */
    Serial.print("Sensor Setup:   DONE");
    Serial.print("  passed: "); Serial.print(pass_count);
    Serial.print("  failed: "); Serial.print(fail_count);
    Serial.println("");
}
void erase_memory(void)
{
    /* Signal begginning of operation */
    Serial.println("");
    Serial.println("Memory Erase:   BEGIN");
    
    /* Execute erase and retreive cc */
    uint8_t memory_erase_cc = EEPROM_DB.erase();
    
    /* Log completion code */
    Serial.print("    cc = 0x");
    Serial.println(memory_erase_cc, HEX);
    
    /* Signal result */
    Serial.print("Memory Erase:   ");
    Serial.println( 0 == memory_erase_cc ? "PASS" : "FAIL" );
}
uint8_t connect_WiFi(char* ssid, char* pwd)
{
    /* Signal task beginning */
    Serial.println("");
    Serial.println("Connecting to WiFi network: BEGIN");
    
    /* Connect to Wifi newtork */
    WiFi.begin(ssid, pwd);
    
    /* Wait for connection to establish */
    delay(WIFI_TIMEOUT);
    
    /* Get status */
    uint8_t wifi_status = WiFi.status();
    uint8_t wifi_cc = (WL_CONNECTED == wifi_status) ? 0 : -1;
    
    /* If success log IP */
    if(WL_CONNECTED == wifi_status)
    {
        Serial.print("    IP Address: ");
        Serial.println( WiFi.localIP() );
    }
    /* If error warn user */
    else
    {
        Serial.println("    WiFi timeout: could not connect :(");
    }
    
    /* Signal task completion */
    Serial.print("Connecting to WiFi network: ");
    Serial.println( WL_CONNECTED == wifi_status ? "PASS" : "FAIL" );
    
    /* Return cc */
    return wifi_cc;
}
void synch_time(void)
{    
    /* Connect to Wifi */
    uint8_t wifi_cc = -1;
    for(uint8_t i = 0; i < NTP_MAX_RETRIES; i++)
    {
        wifi_cc = connect_WiFi(SSID_departure_point, PSWD_departure_point);
        if(0 == wifi_cc) break;
    }
    
    /* Synch cc */
    uint8_t synch_cc = -1;
    
    /* Signal begginning of task */
    Serial.println("");
    Serial.println("NTP synch:  BEGIN");
    
    /* Attempt synch only if WiFi connection succeeded */
    if(0 == wifi_cc)
    {
        /* Configure NTP server */
        configTime(0, 0, ntpServer);
        
        /* Variables to store time */
        time_t now;
        struct tm timeinfo;
        
        /* Synch time */
        uint8_t ntp_cc = -1;
        for(uint8_t i = 0; i < NTP_MAX_RETRIES; i++)
        {
            Serial.println("    trying to synch with NTP server...");
            ntp_cc = getLocalTime(&timeinfo);
            if(ntp_cc) break;
            delay(WIFI_TIMEOUT);
        }
        
        /* If synch is successful */
        if(ntp_cc)
        {
            /* Extract epoch */
            time(&now);
            /* Calculate offset */
            epoch_offset = ((uint32_t)(now)) - ( millis() / 1000 );
            /* Set cc to pass */
            synch_cc = 0;
            /* Log */
            Serial.println("    Success!");
            Serial.print("    Epoch offset = 0x");
            Serial.println(epoch_offset, HEX);
            /* Turn LED on */
            digitalWrite(SIGNAL_LED, HIGH);
        }
        /* If synch fails */
        else
        {
            /* Set offset to 0 */
            epoch_offset = 0;
            /* Log */
            Serial.println("    fail");
            Serial.print("    could not connect to \"");
            Serial.print( ntpServer );
            Serial.println("\"");
            Serial.print("    epoch offset = 0x");
            Serial.println(epoch_offset, HEX);
            /* Add error to flag */
            full_internet_success |= 0x01;
        }
        
        /* Disconnect from wifi */
        WiFi.disconnect(true);
    }
    /* Log failure if wifi connection could not be stablished */
    else
    {
        /* Set offset to 0 */
        epoch_offset = 0;
            
        /* Log */
        Serial.println("    fail");
        Serial.println("    no wifi connection");
        Serial.print("    epoch offset = 0x");
        Serial.println(epoch_offset, HEX);
        
        /* Add error to flag */
        full_internet_success |= 0x02;
    }
    
    /* Signal task completion */
    Serial.print("NTP synch:  ");
    Serial.println( 0 == synch_cc ? "PASS" : "FAIL" );
    
    /* Turn off WiFi */
    WiFi.mode(WIFI_OFF);
    Serial.println("");
    Serial.println("****WiFi:   OFF****");
    
}
void sample_and_push(void)
{
    /* Signal start of task */
    Serial.println("");
    Serial.println("Sensor Sampling:    BEGIN");
    
    /* Initialize EEPROM pointer to 0 */
    EEPROM_DB_ptr = 0;

    /* Sample, print, and push until signaled to stop */
    while( TAVEL_DURATION > millis() )
    {
        /* Wait until next sample time */
        delay(SAMPLNG_PERIOD_ms);
        
        /* Print separator */
        for(uint8_t i = 0; i < 64; i++) Serial.print("*");
        Serial.println("");
    
        /* Iterate over all listed PCT2075 devices */
        for(uint8_t i = 0; i < PCT2075_NUM_DEVICES; i++)
        { 
            /* Fill in record */
            singleTempRecord.PCT2075_device         = PCT2075_device_list[i];
            singleTempRecord.temp_reg_reading       = PCT2075_Mgr.readTempReg( PCT2075_device_list[i] );
            singleTempRecord.timestamp_unix_epoch   = epoch_offset + ( millis() / 1000 );
            singleTempRecord.zeros                  = 0;
            
            /* Log */
            Serial.print("    Sensor 0x");
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
            uint8_t EE_cc = EEPROM_DB.write(EEPROM_DB_ptr, sizeof(TempRecord_t), ((uint8_t*)(&singleTempRecord)) );
            Serial.print("        Store into EEPROM cc = 0x");
            Serial.println(EE_cc, HEX);
            EEPROM_DB_ptr += sizeof(TempRecord_t);
        }
    }
    
    /* Log */
    Serial.println("Sensor Sampling:   DONE");
}
void pop_and_send(void)
{
    /* Turn on WiFi */
    WiFi.mode(WIFI_STA);
    Serial.println("");
    Serial.println("****WiFi:   ON****");
    
    /* Wait until WiFi connection is available */
    Serial.println("");
    Serial.println("****Blocking wait for WiFi connection****");
    uint8_t wifi_cc = -1;
    while(0 != wifi_cc)
    {
        delay(WIFI_TIMEOUT);
        wifi_cc = connect_WiFi(SSID_arrival_point, PSWD_arrival_point);
    }
    
    /* Signal beginning of task */
    Serial.println("");
    Serial.println("Pop and Send:   BEGIN");
    
    /* Reset EEPROM pointer */
    EEPROM_DB_ptr = 0;
    
    /* Reset record */
    singleTempRecord.timestamp_unix_epoch   = 0;
    singleTempRecord.temp_reg_reading       = -1;
    singleTempRecord.PCT2075_device         = 0;
    singleTempRecord.zeros                  = 0;
    
    /* Reset Json string */
    Json2Gateway.reset();
    
    /* Repeat until valid readings stop */
    while( 0 == singleTempRecord.zeros)
    {
        /* Retrieve data from memory */
        uint8_t EE_cc = EEPROM_DB.read(EEPROM_DB_ptr, sizeof(TempRecord_t), ((uint8_t*)(&singleTempRecord)) );
        EEPROM_DB_ptr += sizeof(TempRecord_t);

        /* Break if data is not useful */
        if(0 != singleTempRecord.zeros) break;
        
        /* Log */
        Serial.print("    Reading ");
        Serial.print( sizeof(TempRecord_t) );
        Serial.print(" bytes from memory  cc = 0x");
        Serial.println(EE_cc);
        Serial.print("        Sensor 0x");
        Serial.print(singleTempRecord.PCT2075_device, HEX);
        Serial.print(" reported ");
        Serial.print( PCT2075_Mgr.decodeTempReg(singleTempRecord.temp_reg_reading) );
        Serial.print("°C at time ");
        Serial.print( singleTempRecord.timestamp_unix_epoch, DEC );
        Serial.println(" ms");
        
        /* Add data to Json string */
        uint8_t n = Json2Gateway.getSize();
        Json2Gateway.setTravelID(n, CURRENT_TRAVEL_ID);
        Json2Gateway.setSensorID(n, singleTempRecord.PCT2075_device);
        Json2Gateway.setTempture(n, singleTempRecord.temp_reg_reading);
        Json2Gateway.setTimestmp(n, singleTempRecord.timestamp_unix_epoch);
        Json2Gateway.setSize( n + 1 );
        
        /* Flush json file if full */
        if(Json2Gateway.getSize() == NUM_JSON_OBJ) post_2_APIgateway();
        
    }
    
    /* Flush json file if not empty */
    if(Json2Gateway.getSize() != 0) post_2_APIgateway();
    
    /* Signal completion of task */
    Serial.println("Pop and Send:   DONE");
    
}
void post_2_APIgateway(void)
{
    /* Signal beginning of task */
    Serial.println("");
    Serial.println("    HTTP Post:  BEGIN");
    
    /* Send Json to API Gateway */
    #ifdef POST_2_SERVER_EN
        /* Wait until http handler is free */
        delay(POST_DELAY);
        
        /* Post json string */
        uint8_t http_cc = -1;
        for(uint8_t i = 0; i < HTTP_MAX_RETRIES; i++)
        {
            Serial.println("        attempting HTTP post...");
            http_cc = Json2Gateway.post();
            if(0xFF != http_cc) break;
            delay(WIFI_TIMEOUT);
        }
        
        /* Log cc */
        Serial.print("        http.post cc = 0x");
        Serial.println(http_cc, HEX);
        
        /* Add error to flag */
        full_internet_success |= 0x04;
        
    #else
        /* Signal mock send */
        Serial.println("        mock mode ON");

    #endif
    
    /* Print Json string*/
    Serial.print("        ");
    Json2Gateway.printJson();
    
    /* Reset Json string */
    Json2Gateway.reset();
    
    /* Signal completion of task */
    Serial.print("    HTTP Post:  ");
    #ifdef POST_2_SERVER_EN
        Serial.println( (0xFF == http_cc) ? "FAIL" : "PASS" );
    #else
        Serial.println("DONE");
    #endif
    Serial.println("");
}
void dump_memory(void)
{
    /* Sgignal beginning of task */
    Serial.println("");
    Serial.println("EEPROM Dump:  BEGIN");
    
    /* Dump memory */
    uint8_t dump_cc = EEPROM_DB.dump();
    
    /* Log cc */
    Serial.print("    Memory Dump cc = 0x");
    Serial.println(dump_cc, HEX);
    
    /* Signal end of task */
    Serial.print("EEPROM Dump:  ");
    Serial.print( 0 == dump_cc ? "PASS" : "FAIL" );
}