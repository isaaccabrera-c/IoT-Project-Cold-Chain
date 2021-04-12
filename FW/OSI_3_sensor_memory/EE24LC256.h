/*
    EE24LC256.h - Library for managing EE24LC256 memories.
    Created by IoT_P2021_Equipo2:
        José Eduardo Amaya Zendejas​
        Isaac Cabrera Cortés​
        Javier Camino Treviño​
        Pedro Javier Herrera Soto
    March 23, 2021.
    Released for academic purposes.
*/
#ifndef EE24LC256_h
#define EE24LC256_h




#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>


/* Base I2C timeout (us) for every transaction */
#define EE24LC256_I2C_TIMEOUT_BASE      250
/* I2C timeout (us) for the specified transaction size */
#define EE24LC256_I2C_TIMEOUT_FOR(x)    ( EE24LC256_I2C_TIMEOUT_BASE * (x + 3) )

/* Logical value on WP that enables writting operation */
#define EE24LC256_WP_DIS                LOW
/* Logical value on WP that disables writting operation */
#define EE24LC256_WP_EN                 (~EE24LC256_WP_DIS)

/* Memory size in bytes */
#define EE24LC256_MEM_SIZE              0x8000
/* Maximum memory address */
#define EE24LC256_MEM_MAX_ADDR          (EE24LC256_MEM_SIZE - 1)

/* Buffer size for I2C transactions */  //NOTE: Must be a power of 2 because EE24LC256 only increments the lower 6 bytes (no carry) of address pointer on write operations!
#define EE24LC256_I2C_BUFFER_SIZE       16
/* State the need for last reading compensation if memory size is not divisible by buffer size */
#define EE24LC256_NEED_LAST_READ_COMP   ( 0 != (EE24LC256_MEM_SIZE % EE24LC256_I2C_BUFFER_SIZE) )

/* Blank value to erase memory with */
#define EE24LC256_ERASE_VALUE           0xEE

/* EE24LC256 error codes */
#define EE24LC256_ERROR_PASS            0x00    //Successful request
#define EE24LC256_ERROR_OUT_OF_RANGE    0x10    //Request out of memory range
#define EE24LC256_ERROR_TOO_LARGE       0x20    //Request is too large to be processed
#define EE24LC256_ERROR_BAD_TXN         0x30    //Failed I2C transaction
#define EE24LC256_ERROR_BAD_ERASE_W     0x40    //EEPROM could not be erased because one writting failed
#define EE24LC256_ERROR_BAD_ERASE_R     0x50    //EEPROM could not be erased because one reading failed
#define EE24LC256_ERROR_BAD_ERASE_X     0x60    //EEPROM could not be erased because reading and writting did not match



/* Macro to read the low byte of a 16 bit word */
#define LO_BYTE(x)  ( (uint8_t) (0x00FF & (x)) )
/* Macro to read the high byte of a 16 bit word */
#define HI_BYTE(x)  ( (uint8_t) (0x00FF & (x >> 8)) )




class EE24LC256
{
public:
   /*!
    * @brief Object constructor initializes attributes
    * @param EE24LC256_I2C_device_address I2C address of EE24LC256 device
    * @param WP_pin ESP32 pin number that will be used for WP on EE24LC256
    * @return Object handler
    */
    EE24LC256(uint8_t EE24LC256_I2C_device_address, uint8_t WP_pin);
    
    /*!
    * @brief Read the content of continous cells from a EE24LC256 device
    * @param EE24LC256_cell_address memory cell to begin at (addresses will increase)
    * @param data_size number of bytes to be read from EE24LC256 device
    * @param pointer_to_data adress to store the reading into
    * @return Returns:
                Wire.endTransmission completion code as stated at https://www.arduino.cc/en/Reference/WireEndTransmission
                OR'ed with
                EE24LC256 lib error signal
    */
    uint8_t read(uint16_t EE24LC256_cell_address, uint8_t data_size, uint8_t* pointer_to_data);
    
   /*!
    * @brief Write into continous cells of a EE24LC256 device
    * @param EE24LC256_cell_address memory cell to begin at (addresses will increase)
    * @param data_size number of bytes to be written into the EE24LC256 device
    * @param pointer_to_data adress where de data to be written is located
    * @return Returns:
                Wire.endTransmission completion code as stated at https://www.arduino.cc/en/Reference/WireEndTransmission
                OR'ed with
                EE24LC256 lib error signal
    */
    uint8_t write(uint16_t EE24LC256_cell_address, uint8_t data_size, uint8_t* pointer_to_data);
    
   /*!
    * @brief Dump the entire content of EEPROM into serial console
    * @return Returns:
                Wire.endTransmission completion code as stated at https://www.arduino.cc/en/Reference/WireEndTransmission
                OR'ed with
                EE24LC256 lib error signal
                Note: When an invalid completion code is received, execution is aborted
    */    
    uint8_t dump(void);
    
    /*!
    * @brief Erase whole memory into EE24LC256_ERASE_VALUE
    * @return Returns:
                Wire.endTransmission completion code as stated at https://www.arduino.cc/en/Reference/WireEndTransmission
                OR'ed with
                EE24LC256 lib error signal
                Note: When an invalid completion code is received, execution is aborted
    */    
    uint8_t erase(void);


private:
    /* I2C device address */
    uint8_t _I2C_device_address;
    /* Pin for WP */
    uint8_t _WP_pin;
    /* Buffer to swipe memory */
    uint8_t _TXN_buffer[EE24LC256_I2C_BUFFER_SIZE];
};




#endif
