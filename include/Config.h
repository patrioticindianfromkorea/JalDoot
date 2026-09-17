#pragma once
#include <Arduino.h>

namespace Pins {
    inline constexpr int I2C_SDA = 8;
    inline constexpr int I2C_SCL = 9;

    inline constexpr int GPS_RX  = 20;
    inline constexpr int GPS_TX  = 21;

    inline constexpr int DHT_PIN    = 7;
    inline constexpr int BUTTON_PIN = 6;
    inline constexpr int BUZZER_PIN = 10;

    inline constexpr int LIGHT_ADC = 2; // ADC1_CH2
    inline constexpr int RAIN_ADC  = 3; // ADC1_CH3
    inline constexpr int WATER_ADC = 4; // ADC1_CH4
}

namespace Thresholds {
    inline constexpr int BILGE_HIGH_ABOVE = 1800;
    inline constexpr int RAIN_WET_BELOW   = 2500;
}

namespace NetConfig {
    inline constexpr char WIFI_SSID[]     = "BOAT_WIFI_HOTSPOT";
    inline constexpr char WIFI_PASSWORD[] = "BOAT_WIFI_PASSWORD";
}
