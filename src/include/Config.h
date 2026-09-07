#pragma once
#include <Arduino.h>

namespace NetConfig {
    inline constexpr char WIFI_SSID[]     = "BOAT_WIFI_SSID";
    inline constexpr char WIFI_PASSWORD[] = "BOAT_WIFI_PASS";
}

namespace Pins {
    // Shared I2C: OLED (0x3C), BMP280 (0x76/0x77), MPU-6500 (0x68), QMC/HMC5883L (0x0D/0x1E)
    inline constexpr int I2C_SDA     = 8;
    inline constexpr int I2C_SCL     = 9;

    // GPS NEO-6M UART1
    inline constexpr int GPS_RX      = 20; 
    inline constexpr int GPS_TX      = 21; 

    // 1-Wire & Digital
    inline constexpr int DHT_PIN     = 7;
    inline constexpr int BUTTON_PIN  = 6;  // Active Low with internal pullup
    inline constexpr int BUZZER_PIN  = 10; // 5V active buzzer via transistor/direct

    // Native ADC1 Inputs (WiFi-safe on ESP32-C3)
    inline constexpr int RAIN_ADC    = 0;  // GPIO0 / ADC1_CH0
    inline constexpr int WATER_ADC   = 1;  // GPIO1 / ADC1_CH1
    inline constexpr int LIGHT_ADC   = 2;  // GPIO2 / ADC1_CH2 (TEMT6000)
}

namespace Thresholds {
    inline constexpr int RAIN_WET_BELOW       = 2000; // Raw ADC reading threshold
    inline constexpr int BILGE_HIGH_ABOVE     = 1500; // Water level threshold
    inline constexpr float ROCKING_ALARM_DEG  = 25.0f;// Significant tilt alarm
}