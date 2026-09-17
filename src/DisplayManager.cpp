#include "DisplayManager.h"
#include <math.h>

void DisplayManager::init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();
}

void DisplayManager::setAutoContrast(int ambientLightRaw) {
    uint8_t contrast = map(constrain(ambientLightRaw, 200, 3800), 200, 3800, 5, 255);
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(contrast);
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

    if (snap.capsizeAlarm) {
        display.setTextSize(2);
        display.setCursor(10, 20);
        display.println("CAPSIZE!!");
        display.setTextSize(1);
        display.setCursor(10, 45);
        display.printf("Roll: %.0f Deg", snap.rollDeg);
        display.display();
        return;
    }

    if (mode == SystemMode::LOGGING) {
        switch (page % 3) {
            case 0:
                drawHeader("LIVE: ATMOSPHERE");
                display.setCursor(0, 13);
                display.printf("T:%.1fC  H:%.0f%%\n", snap.temperatureC, snap.humidity);
                display.printf("P:%.1fhPa (%.1f)\n", snap.pressureHpa, snap.pressureDelta3h);
                display.printf("Rain:%s\n", snap.isRainingConfirmed ? "CONFIRMED" : "CLEAR");
                if (snap.baroState == BaroTendency::STORM_WARNING) {
                    display.println(">> STORM WARNING <<");
                } else if (snap.bilgeAlarm) {
                    display.println(">> BILGE WATER RISE <<");
                }
                break;

            case 1:
                drawHeader(snap.isDeadReckoning ? "NAV: *EST DEAD-RECK" : "NAV: GNSS ACTIVE");
                display.setCursor(0, 13);
                display.printf("Lat: %.4f\n", snap.latitude);
                display.printf("Lon: %.4f\n", snap.longitude);
                display.printf("Spd: %.1f kn Sats: %d\n", snap.speedKnots, snap.satellites);
                display.printf("Head: %.0f deg\n", snap.headingDeg);
                break;

            case 2:
                drawHeader("LIVE: SEA DYNAMICS");
                display.setCursor(0, 13);
                display.printf("Pitch/Roll: %.0f/%.0f\n", snap.pitchDeg, snap.rollDeg);
                display.printf("Rock Index: %.1f\n", snap.rockingIndex);
                display.printf("Wind: %.1f km/h\n", snap.windSpeedKmh);
                display.printf("Wave: %.2f m\n", snap.waveHeightM);
                break;
        }
    } else {
        drawHeader(pointToNorth ? "FISH: TO NORTH" : "FISH: TO ANCHOR");
        float targetAngle = 0.0f;

        if (pointToNorth) {
            targetAngle = 360.0f - snap.headingDeg;
        } else if (trip.hasStartFix()) {
            float bearing = trip.getBearingToStart(snap.latitude, snap.longitude);
            targetAngle = bearing - snap.headingDeg;
        }

        drawCompassArrow(28, 38, targetAngle);
        display.setCursor(56, 14);
        if (!pointToNorth && trip.hasStartFix()) {
            display.printf("Dst:%.0fm\n", trip.getDistanceToStartMeters(snap.latitude, snap.longitude));
        } else {
            display.printf("Hdg:%.0f\n", snap.headingDeg);
        }

        display.setCursor(56, 28);
        const char* phases[] = {"New", "1stQ", "Full", "3rdQ"};
        display.printf("Moon: %s\n", phases[snap.moonPhaseIndex]);

        display.setCursor(56, 42);
        display.print("Bite: ");
        for (int i = 0; i < snap.biteRating; i++) display.print("*");

        display.setCursor(56, 54);
        display.printf("SeaRock: %.1f", snap.rockingIndex);
    }

    display.display();
}
