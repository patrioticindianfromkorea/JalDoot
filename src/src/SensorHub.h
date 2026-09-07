// src/SensorHub.h
#pragma once
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <TinyGPSPlus.h>
#include <MPU6050.h>
#include "SensorData.h"

class SensorHub {
public:
    void init();
    void update(LiveSnapshot& snap);
    void setMagnetometerCalibration(float xOffset, float yOffset);

private:
    Adafruit_BMP280 bmp;
    DHT dht{7, DHT22};
    MPU6050 mpu;
    TinyGPSPlus gps;
    HardwareSerial gpsSerial{1};

    uint8_t magAddress = 0;
    float magOffsetX = 0.0f;
    float magOffsetY = 0.0f;

    // Rolling Pressure Buffer (3 hours: 1 sample every 5 min = 36 samples)
    static constexpr int PRESSURE_SAMPLES = 36;
    float pressureHistory[PRESSURE_SAMPLES];
    int pressureIndex = 0;
    bool historyFull = false;
    unsigned long lastPressureLogTime = 0;

    // Bilge Rate of Rise Detection
    int waterHistory[10];
    int waterIndex = 0;
    unsigned long lastWaterLogTime = 0;

    // Rain Anti-Spray Debouncer
    unsigned long rainDetectStart = 0;

    // Capsize Persistence Timer
    unsigned long heelStartTimer = 0;

    void initMagnetometer();
    bool readRawMag(int16_t& mx, int16_t& my, int16_t& mz);
    void processBarometerTrend(LiveSnapshot& snap);
    void processBilgeTrend(LiveSnapshot& snap);
    void processRainDebounce(LiveSnapshot& snap);
    void processSolunar(LiveSnapshot& snap);
};