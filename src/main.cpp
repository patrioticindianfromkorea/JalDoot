#include <Arduino.h>
#include "Config.h"
#include "SensorData.h"
#include "Buzzer.h"
#include "SensorHub.h"
#include "WeatherApi.h"
#include "TripManager.h"
#include "DisplayManager.h"
#include "ButtonController.h"

static SensorHub        sensors;
static WeatherApi       weather;
static TripManager      trip;
static DisplayManager   gui;
static ButtonController button;

static LiveSnapshot snapshot;
static SystemMode currentMode = SystemMode::LOGGING;
static int oledPage = 0;
static bool pointerToNorth = true;

static unsigned long lastApiQuery = 0;
static unsigned long lastSensorUpdate = 0;

void setup() {
    Serial.begin(115200);
    Buzzer::init();
    Buzzer::shortBeep();

    sensors.init();
    gui.init();
    button.init(Pins::BUTTON_PIN);
    weather.init();
}

void loop() {
    unsigned long now = millis();

    // 1. Button Handling
    ButtonEvent evt = button.update();
    if (evt == ButtonEvent::SHORT_PRESS) {
        Buzzer::shortBeep();
        if (currentMode == SystemMode::LOGGING) {
            oledPage = (oledPage + 1) % 3;
        } else {
            pointerToNorth = !pointerToNorth;
        }
    } else if (evt == ButtonEvent::LONG_PRESS) {
        Buzzer::doubleBeep();
        if (currentMode == SystemMode::LOGGING) {
            currentMode = SystemMode::FISHING;
            if (snapshot.gpsValid) {
                trip.setStartPoint(snapshot.latitude, snapshot.longitude);
            }
        } else {
            currentMode = SystemMode::LOGGING;
        }
    } else if (evt == ButtonEvent::DOUBLE_CLICK) {
        Buzzer::shortBeep();
        if (currentMode == SystemMode::LOGGING && snapshot.gpsValid) {
            weather.fetch(snapshot.latitude, snapshot.longitude, snapshot);
        } else if (currentMode == SystemMode::FISHING && snapshot.gpsValid) {
            trip.setStartPoint(snapshot.latitude, snapshot.longitude);
        }
    }

    // 2. High-Frequency Sensor Update (10Hz)
    if (now - lastSensorUpdate >= 100) {
        lastSensorUpdate = now;
        sensors.update(snapshot);

        // Water alarm check
        if (snapshot.waterRaw > Thresholds::BILGE_HIGH_ABOVE) {
            Buzzer::shortBeep();
        }

        gui.render(currentMode, oledPage, snapshot, trip, pointerToNorth);
    }

    // 3. Open-Meteo Fetch (Every 60s in Logging Mode)
    if (currentMode == SystemMode::LOGGING && snapshot.gpsValid) {
        if (now - lastApiQuery >= 60000 || lastApiQuery == 0) {
            lastApiQuery = now;
            weather.fetch(snapshot.latitude, snapshot.longitude, snapshot);
        }
    }
}