// src/TripManager.cpp
#include "TripManager.h"
#include <math.h>

void TripManager::setStartPoint(double lat, double lon) {
    anchorLat = lat;
    anchorLon = lon;
    startFixSet = true;
}

float TripManager::getDistanceToStartMeters(double lat, double lon) const {
    if (!startFixSet) return 0.0f;
    double dLat = (anchorLat - lat) * M_PI / 180.0;
    double dLon = (anchorLon - lon) * M_PI / 180.0;
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat * M_PI / 180.0) * cos(anchorLat * M_PI / 180.0) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    return 6371000.0f * (2.0f * atan2(sqrt(a), sqrt(1.0 - a)));
}

float TripManager::getBearingToStart(double lat, double lon) const {
    if (!startFixSet) return 0.0f;
    double y = sin((anchorLon - lon) * M_PI / 180.0) * cos(anchorLat * M_PI / 180.0);
    double x = cos(lat * M_PI / 180.0) * sin(anchorLat * M_PI / 180.0) -
               sin(lat * M_PI / 180.0) * cos(anchorLat * M_PI / 180.0) * cos((anchorLon - lon) * M_PI / 180.0);
    float bearing = atan2(y, x) * 180.0f / M_PI;
    if (bearing < 0.0f) bearing += 360.0f;
    return bearing;
}

void TripManager::updateDeadReckoning(LiveSnapshot& snap, float dtSeconds) {
    if (snap.gpsValid) {
        lastKnownLat = snap.latitude;
        lastKnownLon = snap.longitude;
        lastKnownSpeedMps = snap.speedKnots * 0.514444f;
        snap.isDeadReckoning = false;
        return;
    }

    // Dead Reckoning fallback: Displace last position by speed & heading
    if (lastKnownLat != 0.0 && lastKnownLon != 0.0) {
        snap.isDeadReckoning = true;
        float distanceTraveled = lastKnownSpeedMps * dtSeconds;
        double headingRad = snap.headingDeg * M_PI / 180.0;

        // Approx displacement over small steps
        double deltaLat = (distanceTraveled * cos(headingRad)) / 111139.0;
        double deltaLon = (distanceTraveled * sin(headingRad)) / (111139.0 * cos(lastKnownLat * M_PI / 180.0));

        lastKnownLat += deltaLat;
        lastKnownLon += deltaLon;

        snap.latitude = lastKnownLat;
        snap.longitude = lastKnownLon;
    }
}