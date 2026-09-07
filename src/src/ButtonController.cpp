// src/ButtonController.cpp
#include "ButtonController.h"

void ButtonController::init(int p) {
    pin = p;
    pinMode(pin, INPUT_PULLUP);
}

ButtonEvent ButtonController::update() {
    bool current = digitalRead(pin);
    unsigned long now = millis();
    ButtonEvent evt = ButtonEvent::NONE;

    if (current == LOW && lastState == HIGH) {
        pressStartTime = now;
    } else if (current == HIGH && lastState == LOW) {
        unsigned long duration = now - pressStartTime;
        if (duration >= 1500) {
            evt = ButtonEvent::LONG_PRESS;
            clickCount = 0;
        } else {
            clickCount++;
            lastReleaseTime = now;
        }
    }

    if (clickCount > 0 && (now - lastReleaseTime > 300)) {
        evt = (clickCount == 1) ? ButtonEvent::SHORT_PRESS : ButtonEvent::DOUBLE_CLICK;
        clickCount = 0;
    }

    lastState = current;
    return evt;
}