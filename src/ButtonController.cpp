#include "ButtonController.h"

void ButtonController::init(int pin) {
    _pin = pin;
    pinMode(_pin, INPUT_PULLUP);
}

ButtonEvent ButtonController::update() {
    bool raw = digitalRead(_pin);
    unsigned long now = millis();
    ButtonEvent result = ButtonEvent::NONE;

    if (_lastState == HIGH && raw == LOW) {
        _downTime = now;
        _longPressHandled = false;
    } else if (_lastState == LOW && raw == LOW) {
        if (!_longPressHandled && (now - _downTime >= 1500)) {
            _longPressHandled = true;
            _clickCount = 0;
            result = ButtonEvent::LONG_PRESS;
        }
    } else if (_lastState == LOW && raw == HIGH) {
        if (!_longPressHandled && (now - _downTime < 1500)) {
            _clickCount++;
            _lastReleaseTime = now;
        }
    }

    if (_clickCount > 0 && (now - _lastReleaseTime > 300)) {
        if (_clickCount == 1) result = ButtonEvent::SHORT_PRESS;
        else if (_clickCount >= 2) result = ButtonEvent::DOUBLE_CLICK;
        _clickCount = 0;
    }

    _lastState = raw;
    return result;
}
