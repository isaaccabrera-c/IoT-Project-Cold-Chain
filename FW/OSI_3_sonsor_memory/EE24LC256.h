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
#define EE24LC256_I2C_TIMEOUT             5


class EE24LC256
{
public:
   /*!
    * @brief Object constructor initializes attributes
    * @return Object handler
    */
    EE24LC256_Mngmt(uint8_t EE24LC256_I2C_device_address);
    
   /*!
    * @brief Object constructor initializes attributes
    * @return Object handler
    */
    EE24LC256_Mngmt(uint8_t EE24LC256_I2C_device_address, uint8_t WP_pin);
    
   /*!
    * @brief Set inner pointer register of selected EE24LC256 device towards temperature register
    * @param EE24LC256_I2C_device_address I2C address of selected EE24LC256 device
    * @return Returns Wire.endTransmission completion code as stated at https://www.arduino.cc/en/Reference/WireEndTransmission
    */
    uint8_t write(uint16_t EE24LC256_cell_address, uint8_t data_size, uint8_t* pointer_to_data);
    


private:
    /* Pin */
    uint8_t _WP_pin;
};




#endif
