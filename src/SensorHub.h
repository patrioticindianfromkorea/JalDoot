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

private:
    Adafruit_BMP280 bmp;
    DHT dht{Pins::DHT_PIN, DHT22};
    MPU6050 mpu;
    TinyGPSPlus gps;
    HardwareSerial gpsSerial{1};
};