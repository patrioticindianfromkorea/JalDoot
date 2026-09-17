#pragma once
#include "SensorData.h"

class TripManager {
public:
    void setStartPoint(double lat, double lon);
    float getBearingToStart(double lat, double lon) const;
    float getDistanceToStartMeters(double lat, double lon) const;
    void updateDeadReckoning(LiveSnapshot& snap, float dtSeconds);

    bool hasStartFix() const { return startFixSet; }
    double getStartLat() const { return anchorLat; }
    double getStartLon() const { return anchorLon; }

private:
    double anchorLat = 0.0;
    double anchorLon = 0.0;
    bool startFixSet = false;

    double lastKnownLat = 0.0;
    double lastKnownLon = 0.0;
    float lastKnownSpeedMps = 0.0f;
};
