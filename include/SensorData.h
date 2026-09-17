#pragma once
#include <Arduino.h>

enum class SystemMode { LOGGING, FISHING };
enum class BaroTendency { STEADY, RISING, FALLING, STORM_WARNING };

struct LiveSnapshot {
    // Environmental
    float temperatureC     = 0.0f;
    float humidity         = 0.0f;
    float pressureHpa      = 0.0f;
    float pressureDelta3h  = 0.0f;
    BaroTendency baroState = BaroTendency::STEADY;

    // Analog & Filtered Detectors
    int   lightRaw           = 0;
    int   rainRaw            = 0;
    bool  isRainingConfirmed = false;
    int   waterRaw           = 0;
    bool  bilgeAlarm         = false;

    // Motion & Orientation (MPU-6500 + Magnetometer)
    float headingDeg   = 0.0f;
    float pitchDeg     = 0.0f;
    float rollDeg      = 0.0f;
    float rockingIndex = 0.0f;
    bool  capsizeAlarm = false;

    // GNSS & Dead Reckoning Fallback
    bool   gpsValid        = false;
    bool   isDeadReckoning = false;
    double latitude        = 0.0;
    double longitude       = 0.0;
    float  speedKnots      = 0.0f;
    int    satellites      = 0;

    // Solunar Activity
    int moonPhaseIndex = 0; // 0: New, 1: First Qtr, 2: Full, 3: Last Qtr
    int biteRating     = 1; // 1 to 4 Stars

    // Live Marine Forecast (Open-Meteo REST API)
    bool  apiValid     = false;
    float windSpeedKmh = 0.0f;
    float waveHeightM  = 0.0f;
};
