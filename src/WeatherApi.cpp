#include "WeatherApi.h"
#include "Config.h"

void WeatherApi::init() {
    WiFi.begin(NetConfig::WIFI_SSID, NetConfig::WIFI_PASSWORD);
}

bool WeatherApi::fetch(double lat, double lon, LiveSnapshot& snap) {
    if (WiFi.status() != WL_CONNECTED) return false;

    HTTPClient http;
    char url[256];

    snprintf(url, sizeof(url),
        "http://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current=wind_speed_10m",
        lat, lon);

    http.begin(url);
    if (http.GET() == HTTP_CODE_OK) {
        JsonDocument doc;
        deserializeJson(doc, http.getString());
        if (doc.containsKey("current")) {
            snap.windSpeedKmh = doc["current"]["wind_speed_10m"] | 0.0f;
            snap.apiValid = true;
        }
    }
    http.end();

    snprintf(url, sizeof(url),
        "http://marine-api.open-meteo.com/v1/marine?latitude=%.4f&longitude=%.4f&current=wave_height",
        lat, lon);

    http.begin(url);
    if (http.GET() == HTTP_CODE_OK) {
        JsonDocument doc;
        deserializeJson(doc, http.getString());
        if (doc.containsKey("current")) {
            snap.waveHeightM = doc["current"]["wave_height"] | 0.0f;
        }
    }
    http.end();

    return snap.apiValid;
}
