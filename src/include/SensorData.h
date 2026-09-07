#pragma once
#include <Arduino.h>

enum class SystemMode { LOGGING, FISHING };

struct LiveSnapshot {
    // Environment
    float temperatureC = 0.0f;
    float humidity     = 0.0f;
    float pressureHpa  = 0.0f;
    int   lightRaw     = 0;
    int   rainRaw      = 0;
    int   waterRaw     = 0;

    // Navigation & Motion (MPU-6500 + Mag + GPS)
    float headingDeg   = 0.0f;
    float pitchDeg     = 0.0f;
    float rollDeg      = 0.0f;
    float rockingIndex = 0.0f;

    // GPS Fix
    bool   gpsValid    = false;
    double latitude    = 0.0;
    double longitude   = 0.0;
    float  speedKnots  = 0.0f;
    int    satellites  = 0;

    // Online Marine Forecast
    bool  apiValid     = false;
    float windSpeedKmh = 0.0f;
    float waveHeightM  = 0.0f;
};