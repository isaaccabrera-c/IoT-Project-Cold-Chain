/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include <Arduino.h>
#include "PCT2075.h"

/* Object Constructor */
PCT2075::PCT2075(void)
{
    /* Initialize constants */
    _I2C_timeout_deadline = PCT2075_I2C_TIMEOUT;
    _nan = sqrt(-1);
}


uint8_t PCT2075::point2TempReg(uint8_t sensor_I2C_address)
{
    /* Wait for any I2C activity to settle */
    delay(1);
    
    /* Select device */
    Wire.beginTransmission(sensor_I2C_address);
    
    /* Select register */
    Wire.write( byte(PCT2075_TEMP_REGISTER) );
    
    /* Return transmission retcode */
    return Wire.endTransmission();
}

int16_t PCT2075::readTempReg(uint8_t I2C_device_address)
{
    /* Request data */
    Wire.requestFrom(I2C_device_address, PCT2075_TEMP_SIZE);
    
    /* Set timeout deadline */
    _I2C_timeout_deadline = PCT2075_I2C_TIMEOUT + millis();
    
    /* Start countdown */
    while( millis() != _I2C_timeout_deadline );
    
    /* Validate reading */
    if(Wire.available() < PCT2075_TEMP_SIZE)
    {
        _last_temp_reg_read = PCT2075_TEMP_READ_CC_MASK;
    }
    else
    {
        _last_temp_reg_read = Wire.read();
        _last_temp_reg_read = _last_temp_reg_read << 8;
        _last_temp_reg_read |= Wire.read();
        _last_temp_reg_read &= ~PCT2075_TEMP_READ_CC_MASK;
    }
    
    /* Return response */
    return _last_temp_reg_read;
}

float PCT2075::decodeTempReg(int16_t temp_reg_reading)
{
    float temperature_in_celsius = 0.0;
    
    /* Check CC */
    if( (PCT2075_TEMP_READ_CC_MASK & temp_reg_reading) == 0 )
    {
        temperature_in_celsius = (float)((temp_reg_reading >> 5) * 0.125);
    }
    else
    {
        temperature_in_celsius = _nan;
    }
    
    return temperature_in_celsius;
}