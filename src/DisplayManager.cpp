// src/DisplayManager.cpp
#include "DisplayManager.h"
#include <math.h>

void DisplayManager::init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();
}

void DisplayManager::drawHeader(const char* title) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(title);
    display.drawFastHLine(0, 9, 128, SSD1306_WHITE);
}

void DisplayManager::drawCompassArrow(int cx, int cy, float angleDeg) {
    float rad = (angleDeg - 90.0f) * M_PI / 180.0f;
    int tipX = cx + cos(rad) * 20;
    int tipY = cy + sin(rad) * 20;
    display.drawCircle(cx, cy, 22, SSD1306_WHITE);
    display.drawLine(cx, cy, tipX, tipY, SSD1306_WHITE);
    display.fillCircle(tipX, tipY, 2, SSD1306_WHITE);
}

void DisplayManager::render(SystemMode mode, int page, const LiveSnapshot& snap, const TripManager& trip, bool pointToNorth) {
    display.clearDisplay();

    if (mode == SystemMode::LOGGING) {
        switch (page % 3) {
            case 0: // Weather
                drawHeader("LIVE SNAPSHOT: ENV");
                display.setCursor(0, 14);
                display.printf("Temp: %.1f C\n", snap.temperatureC);
                display.printf("Humidity: %.0f %%\n", snap.humidity);
                display.printf("Press: %.1f hPa\n", snap.pressureHpa);
                display.printf("Rain ADC: %d\n", snap.rainRaw);
                break;

            case 1: // GPS
                drawHeader("LIVE SNAPSHOT: NAV");
                display.setCursor(0, 14);
                if (snap.gpsValid) {
                    display.printf("Lat: %.4f\n", snap.latitude);
                    display.printf("Lon: %.4f\n", snap.longitude);
                    display.printf("Spd: %.1f kn Sats:%d\n", snap.speedKnots, snap.satellites);
                    display.printf("Head: %.0f deg\n", snap.headingDeg);
                } else {
                    display.println("Acquiring GPS fix...");
                }
                break;

            case 2: // Sea & Marine
                drawHeader("LIVE SNAPSHOT: SEA");
                display.setCursor(0, 14);
                display.printf("Pitch/Roll: %.0f/%.0f\n", snap.pitchDeg, snap.rollDeg);
                display.printf("Rock Index: %.1f\n", snap.rockingIndex);
                display.printf("Wind: %.1f km/h\n", snap.windSpeedKmh);
                display.printf("Wave: %.2f m\n", snap.waveHeightM);
                break;
        }
    } else { // FISHING MODE
        drawHeader(pointToNorth ? "FISHING: TO NORTH" : "FISHING: TO START");
        float targetAngle = 0.0f;

        if (pointToNorth) {
            targetAngle = 360.0f - snap.headingDeg;
        } else if (snap.gpsValid && trip.hasStartFix()) {
            float bearing = trip.getBearingToStart(snap.latitude, snap.longitude);
            targetAngle = bearing - snap.headingDeg;
        }

        drawCompassArrow(32, 38, targetAngle);
        display.setCursor(64, 18);
        if (!pointToNorth && trip.hasStartFix()) {
            display.printf("Dist:\n  %.0fm\n", trip.getDistanceToStartMeters(snap.latitude, snap.longitude));
        } else {
            display.printf("Head:\n  %.0f deg\n", snap.headingDeg);
        }
        display.setCursor(64, 46);
        display.printf("Rock: %.1f", snap.rockingIndex);
    }

    display.display();
}