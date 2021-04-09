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


#include <Arduino.h>
#include "EE24LC256.h"


EE24LC256::EE24LC256(uint8_t EE24LC256_I2C_device_address, uint8_t WP_pin)
{
    /* Store device I2C address */
    _I2C_device_address = EE24LC256_I2C_device_address;
    
    /* Store the pin number that will be used for WP */
    _WP_pin = WP_pin;
    
    /* Initialize WP pin */
    pinMode(_WP_pin, OUTPUT);
    
    /* Block writting operation */
    digitalWrite(_WP_pin, EE24LC256_WP_EN);
}


uint8_t EE24LC256::read(uint16_t EE24LC256_cell_address, uint8_t data_size, uint8_t* pointer_to_data)
{
    /* Variable to store the overall return code */
    uint8_t retVal = 0;
    
    /* Check reading size */
    if(EE24LC256_I2C_BUFFER_SIZE < data_size) return EE24LC256_ERROR_TOO_LARGE;
    /* Check memory scope */
    // if( EE24LC256_MEM_MAX_ADDR < EE24LC256_cell_address ) return EE24LC256_ERROR_OUT_OF_RANGE;
    if( EE24LC256_MEM_SIZE < (EE24LC256_cell_address + data_size) ) return EE24LC256_ERROR_OUT_OF_RANGE;

    /* Point to selected memory cell */
    Wire.beginTransmission(_I2C_device_address);            //Point to device
    Wire.write( byte(HI_BYTE(EE24LC256_cell_address)) );    //High byte of cell address
    Wire.write( byte(LO_BYTE(EE24LC256_cell_address)) );    //Low byte of cell address
    retVal |= Wire.endTransmission();                        //Stop transmission
    
    /* Request information to device */
    Wire.requestFrom(_I2C_device_address, data_size);
    
    /* Wait for response to arrive */
    delay( EE24LC256_I2C_TIMEOUT_FOR(data_size) );
    
    /* Check if the right amount of data was received */
    if( Wire.available() < data_size )
    {
        /* Signal error code */
        retVal |= EE24LC256_ERROR_BAD_TXN;   
    }
    else
    {
        /* Store the readings on the space provided */
        for(uint8_t i = 0; i < data_size; i++) pointer_to_data[i] = Wire.read();
    }
    
    return retVal;
}


uint8_t EE24LC256::write(uint16_t EE24LC256_cell_address, uint8_t data_size, uint8_t* pointer_to_data)
{
    /* Variable to store the overall return code */
    uint8_t retVal = 0;
    
    /* Check writting size */
    if(EE24LC256_I2C_BUFFER_SIZE < data_size) return EE24LC256_ERROR_TOO_LARGE;
    /* Check memory scope */
    // if( EE24LC256_MEM_MAX_ADDR < EE24LC256_cell_address ) return EE24LC256_ERROR_OUT_OF_RANGE;
    if( EE24LC256_MEM_SIZE < (EE24LC256_cell_address + data_size) ) return EE24LC256_ERROR_OUT_OF_RANGE;
    
    /* Disable Write Protect (WP) */
    digitalWrite(_WP_pin, EE24LC256_WP_DIS);    

    /* Point to selected memory cell */
    Wire.beginTransmission(_I2C_device_address);            //Point to device
    Wire.write( byte(HI_BYTE(EE24LC256_cell_address)) );    //High byte of cell address
    Wire.write( byte(LO_BYTE(EE24LC256_cell_address)) );    //Low byte of cell address
    
    /* Write data */
    for(uint8_t i = 0; i < data_size; i++) Wire.write( byte(pointer_to_data[i]) );
    
    /* Stop transmission and retrieve completion code */
    retVal |= Wire.endTransmission();
    
    /* Wait for response to arrive */
    delay( EE24LC256_I2C_TIMEOUT_FOR(data_size) );
    
    /* Re-enable Write Protect (WP) */
    digitalWrite(_WP_pin, EE24LC256_WP_EN);
    
    return retVal;
}


uint8_t EE24LC256::dump(void)
{
    /* Memory space to store each reading data */
    uint8_t reading_data[EE24LC256_I2C_BUFFER_SIZE] = {0};
    /* Reading size */
    uint8_t reading_size = EE24LC256_I2C_BUFFER_SIZE;
    /* Return code from I2C transaction */
    uint8_t retVal = 0;
    
    /* Auxiliary variable to hold the starting address of the last reading */
    static const uint16_t max_addr_minus_buff_size = EE24LC256_MEM_MAX_ADDR - EE24LC256_I2C_BUFFER_SIZE;
    
    /* Iterate over all memory cells */
    for(uint16_t start_addr = 0; start_addr < EE24LC256_MEM_SIZE; start_addr += EE24LC256_I2C_BUFFER_SIZE)
    {   
        /* Adjust for last reading */
        if(max_addr_minus_buff_size < start_addr )
        {
            reading_size = EE24LC256_MEM_SIZE - start_addr;
        }
    
        /* Read from device */
        retVal |= read(start_addr, reading_size, reading_data);
        
        /* Break if failure */
        if(0 != retVal) break;
        
        /* Print reading_data */
        for(uint8_t i = 0; i < reading_size; i++)
        {
            Serial.print("EEPROM @0x");
            Serial.print(start_addr + i, HEX);
            Serial.print(" = 0x");
            Serial.println(reading_data[i], HEX);
        }
    }
    
    return retVal;
}


uint8_t EE24LC256::erase(void)
{
    /* Loop iterator */
    uint8_t i = 0;
    
    /* Memory space to store an "erased" memory block */
    static uint8_t block_data[EE24LC256_I2C_BUFFER_SIZE] = {0};
    /* Initialize array */
    for(i = 0; i < EE24LC256_I2C_BUFFER_SIZE; i++) block_data[i] = EE24LC256_ERASE_VALUE;
    
    /* Block size */
    uint8_t block_size = EE24LC256_I2C_BUFFER_SIZE;
    /* Return code from I2C transaction */
    uint8_t retVal = 0;
    
    /* Auxiliary variable to hold the starting address of the last reading */
    static const uint16_t max_addr_minusblock_data_size = EE24LC256_MEM_MAX_ADDR - EE24LC256_I2C_BUFFER_SIZE;
    
    /* Iterate over all memory cells */
    for(uint16_t start_addr = 0; start_addr < EE24LC256_MEM_SIZE; start_addr += EE24LC256_I2C_BUFFER_SIZE)
    {   
        /* Adjust for last block */
        if(max_addr_minusblock_data_size < start_addr )
        {
            block_size = EE24LC256_MEM_SIZE - start_addr;
        }
        
        /* Write into device */
        retVal |= write(start_addr, block_size, block_data);
        
        /* Handle bad writtings */
        if(0 != retVal)
        {
            /* Add EE24LC256 error code */
            retVal |= EE24LC256_ERROR_BAD_ERASE_W;
            /* Abort immediately */
            return retVal;
        }
    
        /* Read from device */
        retVal |= read(start_addr, block_size, block_data);
        
        /* Handle bad readings */
        if(0 != retVal)
        {
            /* Add EE24LC256 error code */
            retVal |= EE24LC256_ERROR_BAD_ERASE_R;
            /* Abort immediately */
            return retVal;
        }
        
        /* Verify reading */
        for(i = 0; i < block_size; i++)
        {
            /* Handle missmatches */
            if(EE24LC256_ERASE_VALUE != block_data[i])
            {
                /* Add EE24LC256 error code */
                retVal |= EE24LC256_ERROR_BAD_ERASE_X;
                /* Abort immediately */
                return retVal;
            }
        }
    }
    
    return retVal;
}

