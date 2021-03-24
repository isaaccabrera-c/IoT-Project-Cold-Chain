/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/


#ifndef PCT2075_h
#define PCT2075_h


#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>


/* Temperature register address inside sensor */
#define PCT2075_TEMP_REGISTER       0x00
/* Temperature register size (bytes) */
#define PCT2075_TEMP_SIZE           2
/* Temperature regiter resolution (bits) */
#define PCT2075_TEMP_RESOLUTION     11

/* Mask for Temperature register reading completion code */
#define PCT2075_TEMP_READ_CC_MASK   0x1F

/* I2C timeout (ms) */
#define PCT2075_I2C_TIMEOUT         5


class PCT2075
{
public:
    /* Constructor */
    PCT2075(void);
    
    /* Point to Temperature register */
    uint8_t point2TempReg(uint8_t I2C_device_address);
    
    /* Read temperature register */
    int16_t readTempReg(uint8_t I2C_device_address);
    
    /* Decode temperature */
    float decodeTempReg(int16_t temp_reg_reading);

private:
    /* Set the moment at which an I2C timeout will be declared */
    uint32_t _I2C_timeout_deadline;
    /* Set defult invalid temperature value */
    float _nan;
    /* Store the last temperature reading */
    int16_t _last_temp_reg_read;
};


#endif





// #define TEMP_READ_SIZE 0x02
// #define I2C_clockFrequency 400000