#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <constants_wifi.h>

#define WIFI_NAME "ESP32_NET"

namespace WIFI
{
    void setupWiFi();
}