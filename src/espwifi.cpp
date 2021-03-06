#include <espwifi.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <constants_wifi.h>

void WIFI::setupWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(WIFI_NAME);
  esp_wifi_set_ps(WIFI_PS_NONE);
  
  WiFi.begin(SSID_Router, PASSWD_Router);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    #ifdef DEBUG
      Serial.print(".");
    #endif
  }
  #ifdef DEBUG
    Serial.println("\n" + WiFi.macAddress() + " " + WiFi.localIP().toString());
  #endif
}