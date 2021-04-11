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
    /* Initialize invalid temperature constant */
    _nan = sqrt(-1);
    
    /* Initialize last reading with an invalid value */
    _last_temp_reg_read = PCT2075_TEMP_READ_CC_MASK;
}


uint8_t PCT2075_Mngmt::point2TempReg(uint8_t PCT2075_I2C_device_address)
{
    /* Return code */
    uint8_t retVal = 0;
    
    /* Select I2C device */
    Wire.beginTransmission(PCT2075_I2C_device_address);
    
    /* Select Temperature register */
    Wire.write( byte(PCT2075_TEMP_REG_ADDR) );
    
    /* Send message and retrieve completion code */
    retVal |= Wire.endTransmission();
    
    /* Wait for TXN to complete */
    delay(PCT2075_I2C_TIMEOUT);
    
    /* Return retcode */
    return retVal;
}

int16_t PCT2075_Mngmt::readTempReg(uint8_t PCT2075_I2C_device_address)
{
    /* Request reading of register pointed by pointer register */
    Wire.requestFrom(PCT2075_I2C_device_address, PCT2075_TEMP_REG_SIZE);
    
    /* Wait for data to arrive */
    delay(PCT2075_I2C_TIMEOUT);
    
    /* Validate reading */
    if(Wire.available() < PCT2075_TEMP_REG_SIZE)
    {
        /* Store an invalid value into _last_temp_reg_read */
        _last_temp_reg_read = PCT2075_TEMP_READ_CC_MASK;
    }
    else
    {
        /* Retrieve reading from I2C buffer */
        _last_temp_reg_read  = Wire.read();
        _last_temp_reg_read  = _last_temp_reg_read << 8;
        _last_temp_reg_read |= Wire.read();
        /* Clear CC to signal it is a valid reading */
        _last_temp_reg_read &= ~PCT2075_TEMP_READ_CC_MASK;
    }
    
    /* Return stored reading */
    return _last_temp_reg_read;
}

float PCT2075_Mngmt::decodeTempReg(int16_t PCT2075_Temp_reg_reading)
{
    /* Temperature value in °C */
    float temperature_in_celsius = 0.0;
    
    /* Check CC */
    if( 0 == (PCT2075_TEMP_READ_CC_MASK & PCT2075_Temp_reg_reading) )
    {
        /* Parse temperature as specified on datasheet */
        temperature_in_celsius = (float)((PCT2075_Temp_reg_reading >> PCT2075_TEMP_READ_PL_SHIFT) * PCT2075_TEMP_READ_RES_CELCIUS);
    }
    else
    {
        /* Simply retrieve NaN value */
        temperature_in_celsius = _nan;
    }
    
    /* Return parsed temperature */
    return temperature_in_celsius;
}