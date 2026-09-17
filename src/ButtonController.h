#pragma once
#include <Arduino.h>

enum class ButtonEvent { NONE, SHORT_PRESS, DOUBLE_CLICK, LONG_PRESS };

class ButtonController {
public:
    void init(int pin);
    ButtonEvent update();

private:
    int pin = -1;
    bool lastState = HIGH;
    unsigned long pressStartTime = 0;
    unsigned long lastReleaseTime = 0;
    int clickCount = 0;
};
