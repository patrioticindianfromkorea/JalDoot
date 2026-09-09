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
        delay(50);
        digitalWrite(Pins::BUZZER_PIN, LOW);
    }

    static void doubleBeep() {
        shortBeep();
        delay(60);
        shortBeep();
    }

    static void stormAlarm() {
        for (int i = 0; i < 3; i++) {
            digitalWrite(Pins::BUZZER_PIN, HIGH);
            delay(120);
            digitalWrite(Pins::BUZZER_PIN, LOW);
            delay(80);
        }
    }

    static void emergencyTone() {
        for (int i = 0; i < 5; i++) {
            digitalWrite(Pins::BUZZER_PIN, HIGH);
            delay(250);
            digitalWrite(Pins::BUZZER_PIN, LOW);
            delay(100);
        }
    }
};