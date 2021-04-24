#include "myCredentials.h"
#include <stdint.h>
#include <time.h>
#include <WiFi.h>

/* NTP server URL */
const char ntpServer [] = "time.google.com";//"pool.ntp.org";
/* Variable to save current epoch time */
uint32_t epochTime; 


/* Function prototypes */
void printLocalTime(void);


void setup()
{
    /* Initialize serial communicaion */
    Serial.begin(115200);

    //connect to WiFi
    Serial.printf("Connecting to %s ", mySSID);
    WiFi.begin(mySSID, myPSWD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" CONNECTED");
    
    /* Configure ntp server */
    configTime(0, 0, ntpServer);
}

void loop()
{
    delay(1000);
    printLocalTime();
    
    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    
    /* EOP */
    while(-1);
}




void printLocalTime(void)
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
    Serial.println("Failed to obtain time");
    }
    else
    {
        time(&now);
        Serial.print("Epoch: 0x");
        Serial.print(now, HEX);
        Serial.print(" (original)");
        Serial.print("    0x");
        Serial.print( ((uint32_t)(now)), HEX);
        Serial.println(" (casted)");
    }
    
}

