#pragma once
#include <Arduino.h>
#include <esp_now.h>

namespace ESPNOW
{
    void setupEspNow();
    void onRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len);
    void onSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    String macAddrString(const uint8_t * const mac_addr);
    uint8_t* macAddrString(const String mac_addr);
    esp_err_t addSlave(const uint8_t* const address);
    esp_err_t sendData(String message, const uint8_t* const dest, const uint8_t* const relay);
}