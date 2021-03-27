#include "PCT2075.h"
#include <stdint.h>
#include <Wire.h>


/* Baud rate for serial communications */
#define UART_BAUDRATE       115200
/* Clockspeed for I2C communication */
#define I2C_CLK_SPEED_Hz    400000
/* Sensor sampling period in milli-seconds */
#define SAMPLNG_PERIOD_ms   1000
/* Number of PCT2075 devices in circuit */
#define PCT2075_NUM_DEVICES 8


/* List of I2C addresses of PCT2075 devices on circuit */
const uint8_t PCT2075_device_list[PCT2075_NUM_DEVICES] = {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F};
/* Variable to store a single PCT2075 reading */
int16_t Temp_reg_reading = 0x0000;
/* for-loop iterator */
uint8_t i = 0;


/* PCT2075 manager instance of management class */
PCT2075_Mngmt PCT2075_Mgr;


void setup()
{
    /* Setup UART */
    Serial.begin(UART_BAUDRATE);
    
    /* Setup I2C */
    Wire.begin();
    Wire.setClock(I2C_CLK_SPEED_Hz);
    
    /* Set pointer register of all listed PCT2075 devices to point towards Temperature register */
    for(i = 0; i < PCT2075_NUM_DEVICES; i++)
    {
        PCT2075_Mgr.point2TempReg( PCT2075_device_list[i] );
    }
    
}


void loop()
{
    /* Wait until next sample */
    delay(SAMPLNG_PERIOD_ms);
    
    /* Iterate over all listed PCT2075 devices */
    for(i = 0; i < PCT2075_NUM_DEVICES; i++)
    { 
        Temp_reg_reading = PCT2075_Mgr.readTempReg( PCT2075_device_list[i] );
        
        Serial.print("Sensor 0x");
        Serial.print(PCT2075_device_list[i], HEX);
        Serial.print(" : 0x");
        Serial.print(Temp_reg_reading, HEX);
        Serial.print(" = ");
        Serial.print( PCT2075_Mgr.decodeTempReg(Temp_reg_reading) );
        Serial.println(" °C");
    }
    
    Serial.println("********************************");
}
