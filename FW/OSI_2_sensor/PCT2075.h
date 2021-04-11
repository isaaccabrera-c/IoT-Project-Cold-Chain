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
#ifndef PCT2075_h
#define PCT2075_h




#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>


/* I2C timeout (ms) */
#define PCT2075_I2C_TIMEOUT             1

/* Temperature register address inside sensor */
#define PCT2075_TEMP_REG_ADDR           0x00
/* Temperature register size in bytes */
#define PCT2075_TEMP_REG_SIZE           2

/* Temperature reading resoluton in bits */
#define PCT2075_TEMP_READ_RES_BITS      11
/* Temperature reading resolution in °C */
#define PCT2075_TEMP_READ_RES_CELCIUS   0.125
/* Temperature reading mask for completion code */
#define PCT2075_TEMP_READ_CC_MASK       0x1F
/* Temperature reading shift for payload */
#define PCT2075_TEMP_READ_PL_SHIFT      5


class PCT2075_Mngmt
{
public:
   /*!
    * @brief Object constructor initializes attributes
    * @return Object handler
    */
    PCT2075_Mngmt(void);
    
   /*!
    * @brief Set inner pointer register of selected PCT2075 device towards temperature register
    * @param PCT2075_I2C_device_address I2C address of selected PCT2075 device
    * @return Returns Wire.endTransmission completion code as stated at https://www.arduino.cc/en/Reference/WireEndTransmission
    */
    uint8_t point2TempReg(uint8_t PCT2075_I2C_device_address);
    
   /*!
    * @brief Read temperature register of selected PCT2075 device
    * @param PCT2075_I2C_device_address I2C address of selected PCT2075 device
    * @return Returns 
                on sucess:  Content of selected device's temeprature register & ~PCT2075_TEMP_READ_CC_MASK
                on failure: 0xXXXX | PCT2075_TEMP_READ_CC_MASK
    */
    int16_t readTempReg(uint8_t PCT2075_I2C_device_address);
    
   /*!
    * @brief Decode a temperature reading from PCT2075 device
    * @param PCT2075_Temp_reg_reading 16-bit temperature reading as retrieved by readTempReg
    * @return Returns 
                valid reading:      Parsed temperature in °C
                invalid reading:    nan
    */
    float decodeTempReg(int16_t PCT2075_Temp_reg_reading);

private:
    /* Set the defult invalid temperature value */
    float _nan;
    /* Store the last temperature register reading */
    int16_t _last_temp_reg_read;
};




#endif
