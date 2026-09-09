#pragma once
#include <Arduino.h>

class CompassPointer {
public:
    CompassPointer(int stepPin, int dirPin, int enPin, int stepsPerRev = 2048);

    void init();
    void setEnabled(bool enabled);
    void updateTargetAngle(float targetAngleDeg);
    void tick(); // Non-blocking step generation, call frequently in loop()

private:
    int _stepPin;
    int _dirPin;
    int _enPin;
    int _stepsPerRev;

    long _currentStep;
    long _targetStep;
    unsigned long _lastStepMicros;
    unsigned long _stepIntervalMicros;

    long angleToSteps(float deg);
    float normalizeDeg(float deg);
};