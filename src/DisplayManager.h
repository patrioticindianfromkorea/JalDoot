// src/DisplayManager.h
#pragma once
#include <Adafruit_SSD1306.h>
#include "SensorData.h"
#include "TripManager.h"

class DisplayManager {
public:
    void init();
    void render(SystemMode mode, int page, const LiveSnapshot& snap, const TripManager& trip, bool pointToNorth);

private:
    Adafruit_SSD1306 display{128, 64, &Wire, -1};
    void drawHeader(const char* title);
    void drawCompassArrow(int cx, int cy, float angleDeg);
};