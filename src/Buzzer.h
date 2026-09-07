#pragma once
#include <Arduino.h>
#include "Config.h"

class Buzzer {
public:
    static void init() {
        pinMode(Pins::BUZZER_PIN, OUTPUT);
        digitalWrite(Pins::BUZZER_PIN, LOW);
    }

    static void shortBeep() {
        digitalWrite(Pins::BUZZER_PIN, HIGH);
        delay(60);
        digitalWrite(Pins::BUZZER_PIN, LOW);
    }

    static void doubleBeep() {
        shortBeep();
        delay(60);
        shortBeep();
    }
};