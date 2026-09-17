#pragma once
#include <Arduino.h>

enum class ButtonEvent { NONE, SHORT_PRESS, DOUBLE_CLICK, LONG_PRESS };

class ButtonController {
public:
    void init(int pin);
    ButtonEvent update();

private:
    int _pin = 6;
    bool _lastState = true;
    unsigned long _downTime = 0;
    unsigned long _lastReleaseTime = 0;
    int _clickCount = 0;
    bool _longPressHandled = false;
};
