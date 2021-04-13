////////////////Libraries///////////////////////////////////////
#include "EE24LC256.h"
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
/* EE24LC256 instance */
EE24LC256 EEPROM_DB(EEPROM_DB_I2C_ADDR, EEPROM_DB_WP_pin);
/* Single PCT2075 reading */
TempRecord_t singleTempRecord = {0};
/* EEPROM pointer */
uint16_t EEPROM_DB_ptr = 0x0000;
////////////////////////////////////////////////////////////////


void setup()
{
    /* Setup UART */
    Serial.begin(UART_BAUDRATE);
    
    /* Setup I2C */
    Wire.begin();
    Wire.setClock(I2C_CLK_SPEED_Hz);
    
    // /* Test */
    delay(2000);
    // Serial.print("TempRecord_t = ");
    // Serial.println( sizeof(TempRecord_t) );
    
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
    /* Wait until next sample */
    delay(SAMPLNG_PERIOD_ms);
    
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
    
    if(1000*30 < millis())
    {
        /* Dump memory */
        delay(10);
        uint8_t dump_cc = EEPROM_DB.dump();
        Serial.print("Dump cc = 0x");
        Serial.println(dump_cc, HEX);
        
        /* Retrieve all registers on memory */
        EEPROM_DB_ptr = 0;
        while(1)
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
        }
        
        /* EOP */
        while(1);
    }
}
