// src/TripManager.h
#pragma once
#include "SensorData.h"

class TripManager {
public:
    void setStartPoint(double lat, double lon);
    float getBearingToStart(double currentLat, double currentLon) const;
    float getDistanceToStartMeters(double currentLat, double currentLon) const;

    bool hasStartFix() const { return startFixSet; }
    double getStartLat() const { return anchorLat; }
    double getStartLon() const { return anchorLon; }

private:
    double anchorLat = 0.0;
    double anchorLon = 0.0;
    bool startFixSet = false;
};