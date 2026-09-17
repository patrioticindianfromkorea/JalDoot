#pragma once
#include <Arduino.h>

enum class SystemMode { LOGGING, FISHING };
enum class BaroTendency { STEADY, RISING, FALLING, STORM_WARNING };

struct LiveSnapshot {
    float temperatureC = 0.0f;
    float humidity     = 0.0f;
    float pressureHpa  = 0.0f;
    float pressureDelta3h = 0.0f;
    BaroTendency baroState = BaroTendency::STEADY;

    int   lightRaw     = 0;
    int   rainRaw      = 0;
    bool  isRainingConfirmed = false;
    int   waterRaw     = 0;
    bool  bilgeAlarm   = false;

    float headingDeg   = 0.0f;
    float pitchDeg     = 0.0f;
    float rollDeg      = 0.0f;
    float rockingIndex = 0.0f;
    bool  capsizeAlarm = false;

    bool   gpsValid       = false;
    bool   isDeadReckoning = false;
    double latitude       = 0.0;
    double longitude      = 0.0;
    float  speedKnots     = 0.0f;
    int    satellites     = 0;

    int moonPhaseIndex = 0;
    int biteRating     = 1;

    bool  apiValid     = false;
    float windSpeedKmh = 0.0f;
    float waveHeightM  = 0.0f;
};
