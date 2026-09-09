// src/SensorHub.cpp
#include "SensorHub.h"
#include "Config.h"
#include <math.h>

void SensorHub::init() {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
    Wire.setClock(400000);

    bmp.begin(0x76);
    dht.begin();
    mpu.initialize();
    initMagnetometer();

    gpsSerial.begin(9600, SERIAL_8N1, Pins::GPS_RX, Pins::GPS_TX);
    analogReadResolution(12);

    for (int i = 0; i < PRESSURE_SAMPLES; i++) pressureHistory[i] = 1013.25f;
    for (int i = 0; i < 10; i++) waterHistory[i] = 0;
}

void SensorHub::setMagnetometerCalibration(float xOffset, float yOffset) {
    magOffsetX = xOffset;
    magOffsetY = yOffset;
}

void SensorHub::initMagnetometer() {
    Wire.beginTransmission(0x0D);
    if (Wire.endTransmission() == 0) {
        magAddress = 0x0D;
        Wire.beginTransmission(0x0D);
        Wire.write(0x0B); Wire.write(0x01);
        Wire.endTransmission();
        Wire.beginTransmission(0x0D);
        Wire.write(0x09); Wire.write(0x1D);
        Wire.endTransmission();
        return;
    }
    Wire.beginTransmission(0x1E);
    if (Wire.endTransmission() == 0) {
        magAddress = 0x1E;
        Wire.beginTransmission(0x1E);
        Wire.write(0x02); Wire.write(0x00);
        Wire.endTransmission();
    }
}

bool SensorHub::readRawMag(int16_t& mx, int16_t& my, int16_t& mz) {
    if (magAddress == 0x0D) {
        Wire.beginTransmission(0x0D);
        Wire.write(0x00);
        Wire.endTransmission(false);
        if (Wire.requestFrom((uint8_t)0x0D, (uint8_t)6) == 6) {
            mx = (int16_t)(Wire.read() | (Wire.read() << 8));
            my = (int16_t)(Wire.read() | (Wire.read() << 8));
            mz = (int16_t)(Wire.read() | (Wire.read() << 8));
            return true;
        }
    } else if (magAddress == 0x1E) {
        Wire.beginTransmission(0x1E);
        Wire.write(0x03);
        Wire.endTransmission(false);
        if (Wire.requestFrom((uint8_t)0x1E, (uint8_t)6) == 6) {
            mx = (int16_t)((Wire.read() << 8) | Wire.read());
            mz = (int16_t)((Wire.read() << 8) | Wire.read());
            my = (int16_t)((Wire.read() << 8) | Wire.read());
            return true;
        }
    }
    return false;
}

void SensorHub::processBarometerTrend(LiveSnapshot& snap) {
    unsigned long now = millis();
    if (now - lastPressureLogTime >= 300000 || lastPressureLogTime == 0) { // Every 5 min
        lastPressureLogTime = now;
        pressureHistory[pressureIndex] = snap.pressureHpa;
        pressureIndex = (pressureIndex + 1) % PRESSURE_SAMPLES;
        if (pressureIndex == 0) historyFull = true;
    }

    int oldestIndex = historyFull ? pressureIndex : 0;
    snap.pressureDelta3h = snap.pressureHpa - pressureHistory[oldestIndex];

    if (snap.pressureDelta3h <= -3.0f) {
        snap.baroState = BaroTendency::STORM_WARNING;
    } else if (snap.pressureDelta3h <= -1.0f) {
        snap.baroState = BaroTendency::FALLING;
    } else if (snap.pressureDelta3h >= 1.0f) {
        snap.baroState = BaroTendency::RISING;
    } else {
        snap.baroState = BaroTendency::STEADY;
    }
}

void SensorHub::processBilgeTrend(LiveSnapshot& snap) {
    unsigned long now = millis();
    if (now - lastWaterLogTime >= 1000) { // 1 Hz rate of rise evaluation
        lastWaterLogTime = now;
        int prev = waterHistory[waterIndex];
        waterHistory[waterIndex] = snap.waterRaw;
        waterIndex = (waterIndex + 1) % 10;

        // Alarm if water exceeds base threshold AND rises by >200 ADC units in 10s
        if (snap.waterRaw > Thresholds::BILGE_HIGH_ABOVE && (snap.waterRaw - prev) > 200) {
            snap.bilgeAlarm = true;
        } else if (snap.waterRaw <= Thresholds::BILGE_HIGH_ABOVE) {
            snap.bilgeAlarm = false;
        }
    }
}

void SensorHub::processRainDebounce(LiveSnapshot& snap) {
    // Rain sensor pulls LOW on moisture
    if (snap.rainRaw < Thresholds::RAIN_WET_BELOW) {
        if (rainDetectStart == 0) {
            rainDetectStart = millis();
        } else if (millis() - rainDetectStart >= 5000) { // 5s sustained wetness = real rain
            snap.isRainingConfirmed = true;
        }
    } else {
        rainDetectStart = 0;
        snap.isRainingConfirmed = false;
    }
}

void SensorHub::processSolunar(LiveSnapshot& snap) {
    if (!gps.date.isValid()) return;

    int y = gps.date.year();
    int m = gps.date.month();
    int d = gps.date.day();

    // Conway's Lunar Phase Formula
    if (m < 3) { y--; m += 12; }
    int a = y / 100;
    int b = a / 4;
    int c = 2 - a + b;
    int e = (int)(365.25 * (y + 4716));
    int f = (int)(30.6001 * (m + 1));
    double jd = c + d + e + f - 1524.5;
    double daysSinceNew = fmod(jd - 2451549.5, 29.530588853);
    if (daysSinceNew < 0) daysSinceNew += 29.530588853;

    // Approximate Phase & Feeding Window
    if (daysSinceNew < 3.7 || daysSinceNew > 25.8) {
        snap.moonPhaseIndex = 0; // New Moon
        snap.biteRating = 4;     // Major feeding window
    } else if (daysSinceNew >= 11.0 && daysSinceNew <= 18.5) {
        snap.moonPhaseIndex = 2; // Full Moon
        snap.biteRating = 4;     // Major feeding window
    } else {
        snap.moonPhaseIndex = (daysSinceNew < 14.7) ? 1 : 3;
        snap.biteRating = 2;     // Minor feeding window
    }
}

void SensorHub::update(LiveSnapshot& snap) {
    snap.temperatureC = dht.readTemperature();
    snap.humidity     = dht.readHumidity();
    snap.pressureHpa  = bmp.readPressure() / 100.0F;

    snap.rainRaw  = analogRead(Pins::RAIN_ADC);
    snap.waterRaw = analogRead(Pins::WATER_ADC);
    snap.lightRaw = analogRead(Pins::LIGHT_ADC);

    processBarometerTrend(snap);
    processBilgeTrend(snap);
    processRainDebounce(snap);

    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float pitch = atan2((float)-ax, sqrt((float)ay * ay + (float)az * az));
    float roll  = atan2((float)ay, (float)az);
    snap.pitchDeg = pitch * 180.0f / M_PI;
    snap.rollDeg  = roll * 180.0f / M_PI;

    // Capsize / Dangerous Listing Angle (>35 deg sustained for 2s)
    if (fabs(snap.rollDeg) > 35.0f || fabs(snap.pitchDeg) > 35.0f) {
        if (heelStartTimer == 0) heelStartTimer = millis();
        else if (millis() - heelStartTimer >= 2000) snap.capsizeAlarm = true;
    } else {
        heelStartTimer = 0;
        snap.capsizeAlarm = false;
    }

    float gyroMag = sqrt((float)gx * gx + (float)gy * gy + (float)gz * gz) / 131.0f;
    snap.rockingIndex = (snap.rockingIndex * 0.85f) + (gyroMag * 0.15f);

    int16_t rawMx, rawMy, rawMz;
    if (readRawMag(rawMx, rawMy, rawMz)) {
        float mx = (float)rawMx - magOffsetX;
        float my = (float)rawMy - magOffsetY;
        float mz = (float)rawMz;
        float Xh = mx * cos(pitch) + mz * sin(pitch);
        float Yh = mx * sin(roll) * sin(pitch) + my * cos(roll) - mz * sin(roll) * cos(pitch);
        float heading = atan2(-Yh, Xh) * 180.0f / M_PI;
        if (heading < 0.0f) heading += 360.0f;
        snap.headingDeg = heading;
    }

    while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());

    if (gps.location.isValid() && gps.location.age() < 2000) {
        snap.gpsValid   = true;
        snap.latitude   = gps.location.lat();
        snap.longitude  = gps.location.lng();
        snap.speedKnots = gps.speed.knots();
        snap.satellites = gps.satellites.value();
        processSolunar(snap);
    } else {
        snap.gpsValid = false;
    }
}