#include <Arduino.h>
#include "Config.h"
#include "SensorData.h"
#include "Buzzer.h"
#include "SensorHub.h"
#include "WeatherApi.h"
#include "TripManager.h"
#include "DisplayManager.h"
#include "ButtonController.h"
#include "CompassPointer.h"

static SensorHub        sensors;
static WeatherApi       weather;
static TripManager      trip;
static DisplayManager   gui;
static ButtonController button;
static CompassPointer   pointer(Pins::STEPPER_STEP, Pins::STEPPER_DIR, Pins::STEPPER_EN, 2048);

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
    pointer.init();
}

void loop() {
    unsigned long now = millis();

    // 1. High-frequency stepper step generator
    pointer.tick();

    // 2. Button Control Events
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
            if (snapshot.gpsValid || snapshot.isDeadReckoning) {
                trip.setStartPoint(snapshot.latitude, snapshot.longitude);
            }
        } else {
            currentMode = SystemMode::LOGGING;
        }
    } else if (evt == ButtonEvent::DOUBLE_CLICK) {
        Buzzer::shortBeep();
        if (currentMode == SystemMode::LOGGING && (snapshot.gpsValid || snapshot.isDeadReckoning)) {
            weather.fetch(snapshot.latitude, snapshot.longitude, snapshot);
        } else if (currentMode == SystemMode::FISHING && (snapshot.gpsValid || snapshot.isDeadReckoning)) {
            trip.setStartPoint(snapshot.latitude, snapshot.longitude);
        }
    }

    // 3. Sensor Polling Loop (10Hz)
    if (now - lastSensorUpdate >= 100) {
        float dt = (now - lastSensorUpdate) / 1000.0f;
        lastSensorUpdate = now;

        sensors.update(snapshot);
        trip.updateDeadReckoning(snapshot, dt);

        // Adjust OLED contrast based on ambient lux
        gui.setAutoContrast(snapshot.lightRaw);

        // Acoustic Alarms
        if (snapshot.capsizeAlarm) {
            Buzzer::emergencyTone();
        } else if (snapshot.baroState == BaroTendency::STORM_WARNING || snapshot.bilgeAlarm) {
            Buzzer::stormAlarm();
        }

        // Stepper Target Heading
        if (currentMode == SystemMode::LOGGING || pointerToNorth) {
            pointer.updateTargetAngle(360.0f - snapshot.headingDeg);
        } else if (trip.hasStartFix()) {
            float bearing = trip.getBearingToStart(snapshot.latitude, snapshot.longitude);
            pointer.updateTargetAngle(bearing - snapshot.headingDeg);
        }

        gui.render(currentMode, oledPage, snapshot, trip, pointerToNorth);
    }

    // 4. Periodic Weather Fetch (60s in Logging Mode)
    if (currentMode == SystemMode::LOGGING && (snapshot.gpsValid || snapshot.isDeadReckoning)) {
        if (now - lastApiQuery >= 60000 || lastApiQuery == 0) {
            lastApiQuery = now;
            weather.fetch(snapshot.latitude, snapshot.longitude, snapshot);
        }
    }
}
