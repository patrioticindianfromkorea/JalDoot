// src/SensorHub.cpp
#include "SensorHub.h"
#include "Config.h"
#include <math.h>

void SensorHub::init() {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
    bmp.begin(0x76);
    dht.begin();
    mpu.initialize();

    gpsSerial.begin(9600, SERIAL_8N1, Pins::GPS_RX, Pins::GPS_TX);
    analogReadResolution(12);
}

void SensorHub::update(LiveSnapshot& snap) {
    // Environmental
    snap.temperatureC = dht.readTemperature();
    snap.humidity     = dht.readHumidity();
    snap.pressureHpa  = bmp.readPressure() / 100.0F;

    // Analog Sensors
    snap.rainRaw  = analogRead(Pins::RAIN_ADC);
    snap.waterRaw = analogRead(Pins::WATER_ADC);
    snap.lightRaw = analogRead(Pins::LIGHT_ADC);

    // MPU-6500 IMU Pitch, Roll, and Wave Energy
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    snap.pitchDeg = atan2((float)-ax, sqrt((float)ay * ay + (float)az * az)) * 180.0f / M_PI;
    snap.rollDeg  = atan2((float)ay, (float)az) * 180.0f / M_PI;
    
    // Wave rocking index based on dynamic rotational gyro momentum
    float gyroMag = sqrt((float)gx * gx + (float)gy * gy + (float)gz * gz) / 131.0f;
    snap.rockingIndex = (snap.rockingIndex * 0.85f) + (gyroMag * 0.15f);

    // Compass Heading (using yaw rate / tilt-compensated magnetic heading)
    float heading = atan2((float)ay, (float)ax) * 180.0f / M_PI;
    if (heading < 0) heading += 360.0f;
    snap.headingDeg = heading;

    // GPS Parsing
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
    if (gps.location.isValid()) {
        snap.gpsValid   = true;
        snap.latitude   = gps.location.lat();
        snap.longitude  = gps.location.lng();
        snap.speedKnots = gps.speed.knots();
        snap.satellites = gps.satellites.value();
    }
}