#include "PCT2075.h"
#include <stdint.h>
#include <Wire.h>

#define UART_BAUDRATE   115200
#define I2C_CLK_SPEED   400000
#define SMPLNG_PERIOD   1000

uint8_t test_address = 0x48;
int16_t test_reading = 0x0000;

PCT2075 PCT2075_Mgr;

void setup()
{
    /* Setup UART */
    Serial.begin(UART_BAUDRATE);
    
    /* Setup I2C */
    Wire.begin();
    Wire.setClock(I2C_CLK_SPEED);
    
    /* Test library */
    PCT2075_Mgr.point2TempReg(test_address);
    
    /* NaN test */
    delay(5000);
    Serial.print("NaN test: ");
    Serial.println( sqrt(-1) );
}


void loop()
{
    delay(SMPLNG_PERIOD);
        
    test_reading = PCT2075_Mgr.readTempReg(test_address);
    
    Serial.print("Sensor 0x");
    Serial.print(test_address, HEX);
    Serial.print(" : 0x");
    Serial.print(test_reading, HEX);
    Serial.print(" = ");
    Serial.print( PCT2075_Mgr.decodeTempReg(test_reading) );
    Serial.println(" °C");
    Serial.println("********");
}
