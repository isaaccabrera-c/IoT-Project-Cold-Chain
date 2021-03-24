#include "PCT2075.h"
#include <stdint.h>
#include <Wire.h>

#define UART_BAUDRATE   115200
#define I2C_CLK_SPEED   400000
#define SMPLNG_PERIOD   1000

#define NUM_SENSORS 8

const uint8_t device[NUM_SENSORS] = {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F};

int16_t test_reading = 0x0000;
uint8_t i = 0;

PCT2075 PCT2075_Mgr;

void setup()
{
    /* Setup UART */
    Serial.begin(UART_BAUDRATE);
    
    /* Setup I2C */
    Wire.begin();
    Wire.setClock(I2C_CLK_SPEED);
    
    /* Test library */
    for(i = 0; i < NUM_SENSORS; i++) PCT2075_Mgr.point2TempReg(device[i]);
    
    /* NaN test */
    delay(5000);
    Serial.print("NaN test: ");
    Serial.println( sqrt(-1) );
}


void loop()
{
    delay(SMPLNG_PERIOD);
    
    for(i = 0; i < NUM_SENSORS; i++)
    { 
        test_reading = PCT2075_Mgr.readTempReg( device[i] );
        
        Serial.print("Sensor 0x");
        Serial.print(device[i], HEX);
        Serial.print(" : 0x");
        Serial.print(test_reading, HEX);
        Serial.print(" = ");
        Serial.print( PCT2075_Mgr.decodeTempReg(test_reading) );
        Serial.println(" °C");
    }
    
    Serial.println("****************");
}
