/*
    PCT2075.h - Library for managing PCT2075 sensors.
    Created by IoT_P2021_Equipo2:
        José Eduardo Amaya Zendejas​
        Isaac Cabrera Cortés​
        Javier Camino Treviño​
        Pedro Javier Herrera Soto
    March 23, 2021.
    Released for academic purposes.
*/


#include <Arduino.h>
#include "PCT2075.h"


PCT2075_Mngmt::PCT2075_Mngmt(void)
{
    /* Initialize constants */
    _I2C_timeout_deadline = PCT2075_I2C_TIMEOUT;
    _nan = sqrt(-1);
    
    /* Initialize last reading with an invalid value */
    _last_temp_reg_read = PCT2075_TEMP_READ_CC_MASK;
}


uint8_t PCT2075_Mngmt::point2TempReg(uint8_t PCT2075_I2C_device_address)
{
    /* Wait for any I2C activity to settle */
    delay(1);
    
    /* Select I2C device */
    Wire.beginTransmission(PCT2075_I2C_device_address);
    
    /* Select Temperature register */
    Wire.write( byte(PCT2075_TEMP_REG_ADDR) );
    
    /* Return I2C transmission retcode */
    return Wire.endTransmission();
}

int16_t PCT2075_Mngmt::readTempReg(uint8_t PCT2075_I2C_device_address)
{
    /* Request reading of register pointed by pointer register */
    Wire.requestFrom(PCT2075_I2C_device_address, PCT2075_TEMP_REG_SIZE);
    
    /* Set I2C timeout deadline */
    _I2C_timeout_deadline = PCT2075_I2C_TIMEOUT + millis();
    
    /* Start countdown */
    while( millis() != _I2C_timeout_deadline );
    
    /* Validate reading */
    if(Wire.available() < PCT2075_TEMP_REG_SIZE)
    {
        _last_temp_reg_read = (-1) | PCT2075_TEMP_READ_CC_MASK;
    }
    else
    {
        _last_temp_reg_read  = Wire.read();
        _last_temp_reg_read  = _last_temp_reg_read << 8;
        _last_temp_reg_read |= Wire.read();
        _last_temp_reg_read &= ~PCT2075_TEMP_READ_CC_MASK;
    }
    
    /* Return stored reading */
    return _last_temp_reg_read;
}

float PCT2075_Mngmt::decodeTempReg(int16_t PCT2075_Temp_reg_reading)
{
    float temperature_in_celsius = 0.0;
    
    /* Check CC */
    if( (PCT2075_TEMP_READ_CC_MASK & PCT2075_Temp_reg_reading) == 0 )
    {
        temperature_in_celsius = (float)((PCT2075_Temp_reg_reading >> PCT2075_TEMP_READ_PL_SHIFT) * PCT2075_TEMP_READ_RES_CELCIUS);
    }
    else
    {
        temperature_in_celsius = _nan;
    }
    
    return temperature_in_celsius;
}