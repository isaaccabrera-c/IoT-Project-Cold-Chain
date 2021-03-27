#include <WiFi.h>
#include "time.h"

const char* SSID_NAME = "Isaac";
const char* PASSWORD   = "12345678";
const char* NTP_SERVER = "time.google.com";//"pool.ntp.org";
const int UTC_OFFSET = -6;
const long  UTC_OFFSET_IN_SEC = UTC_OFFSET * 60 * 60;
const int   DAY_LIGHT_OFFSET_SEC = 0 * 60 * 60;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", SSID_NAME);
  WiFi.begin(SSID_NAME, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(UTC_OFFSET_IN_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();
}
