#pragma once
#include <Arduino.h>

namespace NetConfig {
    inline constexpr char WIFI_SSID[]     = "BOAT_WIFI_HOTSPOT";
    inline constexpr char WIFI_PASSWORD[] = "BOAT_WIFI_PASSWORD";
}

namespace Pins {
    // Shared Hardware I2C (OLED, BMP280, MPU-6500, QMC/HMC5883L)
    // GPIO 8 & 9 are pulled HIGH by external I2C resistors (safe boot state)
    inline constexpr int I2C_SDA      = 8;
    inline constexpr int I2C_SCL      = 9;

    // GPS NEO-6M (Hardware UART1)
    inline constexpr int GPS_RX       = 20; 
    inline constexpr int GPS_TX       = 21; 

    // Digital & 1-Wire
    inline constexpr int DHT_PIN      = 7;
    inline constexpr int BUTTON_PIN   = 6;   // Active LOW (internal pullup)
    inline constexpr int BUZZER_PIN   = 10;  // Active buzzer trigger

    // MP6500 / Stepper Driver Pins (Safely isolated from GPIO 0 and 1)
    inline constexpr int STEPPER_STEP = 5;
    inline constexpr int STEPPER_DIR  = 18;
    inline constexpr int STEPPER_EN   = 19;  // Active LOW

    // Native ADC1 Inputs (WiFi-safe on ESP32-C3; GPIO 0 and 1 avoided)
    inline constexpr int LIGHT_ADC    = 2;   // ADC1_CH2 (TEMT6000)
    inline constexpr int RAIN_ADC     = 3;   // ADC1_CH3
    inline constexpr int WATER_ADC    = 4;   // ADC1_CH4

    // EXPLICITLY RESERVED / UNUSED BOOTSTRAPPING PINS:
    // GPIO 0 : Disconnected (Boot safety)
    // GPIO 1 : Disconnected (Boot safety)
}

namespace Thresholds {
    inline constexpr int RAIN_WET_BELOW       = 2200; // ADC threshold for wet plate
    inline constexpr int BILGE_HIGH_ABOVE     = 1800; // Bilge water level warning
    inline constexpr float HEEL_ALARM_DEG     = 35.0f;// Critical list/roll angle
}
