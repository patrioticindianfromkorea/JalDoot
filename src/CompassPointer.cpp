#include "CompassPointer.h"
#include <math.h>

CompassPointer::CompassPointer(int stepPin, int dirPin, int enPin, int stepsPerRev)
    : _stepPin(stepPin), _dirPin(dirPin), _enPin(enPin), _stepsPerRev(stepsPerRev),
      _currentStep(0), _targetStep(0), _lastStepMicros(0), _stepIntervalMicros(1200) {}

void CompassPointer::init() {
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);
    pinMode(_enPin, OUTPUT);

    digitalWrite(_stepPin, LOW);
    digitalWrite(_dirPin, LOW);
    setEnabled(true);
}

void CompassPointer::setEnabled(bool enabled) {
    // Most stepper drivers (A4988, DRV8825, MP6500) are Active LOW for enable
    digitalWrite(_enPin, enabled ? LOW : HIGH);
}

float CompassPointer::normalizeDeg(float deg) {
    while (deg < 0.0f) deg += 360.0f;
    while (deg >= 360.0f) deg -= 360.0f;
    return deg;
}

long CompassPointer::angleToSteps(float deg) {
    float norm = normalizeDeg(deg);
    return (long)((norm / 360.0f) * _stepsPerRev);
}

void CompassPointer::updateTargetAngle(float targetAngleDeg) {
    long target = angleToSteps(targetAngleDeg);

    // Calculate shortest path around the circle
    long diff = (target - (_currentStep % _stepsPerRev));
    if (diff > (_stepsPerRev / 2)) {
        diff -= _stepsPerRev;
    } else if (diff < -(_stepsPerRev / 2)) {
        diff += _stepsPerRev;
    }

    _targetStep = _currentStep + diff;
}

void CompassPointer::tick() {
    if (_currentStep == _targetStep) return;

    unsigned long now = micros();
    if (now - _lastStepMicros >= _stepIntervalMicros) {
        _lastStepMicros = now;

        if (_targetStep > _currentStep) {
            digitalWrite(_dirPin, HIGH);
            _currentStep++;
        } else {
            digitalWrite(_dirPin, LOW);
            _currentStep--;
        }

        // Pulse the step pin
        digitalWrite(_stepPin, HIGH);
        delayMicroseconds(4);
        digitalWrite(_stepPin, LOW);
    }
}