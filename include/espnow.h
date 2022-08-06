#pragma once
#include <Arduino.h>
#include <esp_now.h>

namespace ESPNOW
{
    void setupEspNow();
    void onRecv(const uint8_t *data, int len, uint64_t from, uint32_t replyPrt);
    esp_err_t sendData(String message, const String dest);
}