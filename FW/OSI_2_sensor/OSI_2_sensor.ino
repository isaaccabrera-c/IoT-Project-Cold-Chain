#include "Wire.h"
#include "stdint.h"

#define SENSOR_BASE_ADDRESS 0x48
#define NUM_SENSORS 8
#define TEMP_REGISTER 0x00
#define TEMP_READ_SIZE 0x02
#define READING_PERIOD_ms 5000

int16_t reading = 0;
float temperature = 0.0;
uint8_t i = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  delay(1000);

  /* Select Temp Register in all sensors */
  for(i = 0; i < NUM_SENSORS; i++)
  {
    Wire.beginTransmission(SENSOR_BASE_ADDRESS | i);
    Wire.write( byte(TEMP_REGISTER) );
    Wire.endTransmission();
  }
}


void loop()
{
  /* Sample Period*/
  delay(READING_PERIOD_ms);

  /* Iterate over all sensors */
  for(i = 0; i < NUM_SENSORS; i++)
  {
    /* Request data */
    Wire.requestFrom(SENSOR_BASE_ADDRESS | i, TEMP_READ_SIZE);

    /* Blocking wait for response */
    while( TEMP_READ_SIZE > Wire.available() );

    /* Store response */
    reading = Wire.read();  // receive high byte (overwrites previous reading)
    reading = reading << 8;    // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    
    /* Calculate temperature */
    temperature = (float)((reading >> 5) * 0.125);

    /* Print */
    Serial.print("Sensor #");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(temperature);   // print the reading
    Serial.println(" °C"); 
  }

  for(i = 0; i < 64; i++) Serial.print("*");
  Serial.println("");
}
