#pragma once
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "SensorData.h"

class WeatherApi {
public:
    void init();
    bool fetch(double lat, double lon, LiveSnapshot& snap);
};
