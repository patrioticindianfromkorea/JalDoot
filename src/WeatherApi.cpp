// src/WeatherApi.cpp
#include "WeatherApi.h"
#include "Config.h"

void WeatherApi::init() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(NetConfig::WIFI_SSID, NetConfig::WIFI_PASSWORD);
}

bool WeatherApi::fetch(double lat, double lon, LiveSnapshot& snap) {
    if (WiFi.status() != WL_CONNECTED) return false;

    HTTPClient http;
    // 1. Wind Speed from Open-Meteo
    String weatherUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + 
                        String(lat, 4) + "&longitude=" + String(lon, 4) + 
                        "&current=wind_speed_10m";

    if (http.begin(weatherUrl)) {
        if (http.GET() == HTTP_CODE_OK) {
            JsonDocument doc;
            deserializeJson(doc, http.getString());
            snap.windSpeedKmh = doc["current"]["wind_speed_10m"].as<float>();
            snap.apiValid = true;
        }
        http.end();
    }

    // 2. Wave Height from Open-Meteo Marine
    String marineUrl = "https://marine-api.open-meteo.com/v1/marine?latitude=" + 
                       String(lat, 4) + "&longitude=" + String(lon, 4) + 
                       "&current=wave_height";

    if (http.begin(marineUrl)) {
        if (http.GET() == HTTP_CODE_OK) {
            JsonDocument marineDoc;
            deserializeJson(marineDoc, http.getString());
            snap.waveHeightM = marineDoc["current"]["wave_height"].as<float>();
        }
        http.end();
    }
    return snap.apiValid;
}