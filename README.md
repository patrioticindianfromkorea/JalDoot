# JalDoot — ESP32-C3 Marine Telemetry & Fishing Station

A live boat instrument console and navigation station powered by an ESP32-C3. Built as a zero-storage, real-time telemetry snapshot system, JalDoot monitors atmospheric pressure trends, wave kinematics, GNSS fixes, and marine forecasts, displaying critical safety warnings and navigational guidance on an SSD1306 OLED and an external stepper pointer.

---

## Hardware Architecture & Pin Map

The ESP32-C3 utilizes native ADC1 lines to prevent interference with active Wi-Fi operations, sharing high-speed sensors across I2C and UART.

| Peripheral / Sensor | Interface / Pins | Operating Role |
| :--- | :--- | :--- |
| **OLED Display (SSD1306)** | Shared I2C (`SDA=8`, `SCL=9`) | Live system snapshot rendering & vector arrow |
| **BMP280** | Shared I2C (`SDA=8`, `SCL=9`) | Barometric pressure & squall/storm trend tracking |
| **MPU-6500 IMU** | Shared I2C (`SDA=8`, `SCL=9`) | Pitch/roll angles, heel safety & wave rocking index |
| **HMC/QMC5883L** | Shared I2C (`SDA=8`, `SCL=9`) | Tilt-compensated magnetic heading |
| **u-blox NEO-6M GPS** | UART1 (`RX=20`, `TX=21`) | Position fix, ground speed, UTC date & solunar clock |
| **DHT22** | 1-Wire Digital (`GPIO 7`) | Ambient air temperature & relative humidity |
| **TEMT6000 Daylight** | ADC1_CH2 (`GPIO 2`) | Ambient lux detection for display auto-contrast |
| **Raindrop Sensor** | ADC1_CH0 (`GPIO 0`) | Sea-spray filtered precipitation detection |
| **Water Level Sensor** | ADC1_CH1 (`GPIO 1`) | Bilge water ingress & rate-of-rise monitoring |
| **Stepper Driver (MP6500)** | Digital (`STEP=0`, `DIR=1`, `EN=18`)| Physical heading / anchor steering needle |
| **Button / Active Buzzer** | Digital (`GPIO 6`, `10`) | Multifunction UI input & acoustic alarm alerts |

---

## Key Firmware Features

* **Barometric Storm Warning Engine:** Maintains a rolling 3-hour barometric buffer (sampled every 5 minutes). A rapid pressure drop ($\le -3\text{ hPa}$) triggers an audible squall alarm and on-screen alert.
* **Bilge Ingress Rate-of-Rise Alarm:** Evaluates water level changes dynamically ($1\text{ Hz}$). Alerts if depth crosses the threshold alongside a rapid positive rate-of-rise, preventing slow-accumulation false triggers.
* **Capsize & Severe Heel Detection:** Sustained listing or roll exceeding $\pm 35^\circ$ for longer than 2 seconds fires an urgent emergency tone and full-screen capsize warning.
* **Dead Reckoning Fallback:** When GPS fix drops, the system estimates position displacement using the last recorded ground speed, forward vector integration, and magnetometer heading.
* **Adaptive Display Contrast:** Dynamically modulates SSD1306 register contrast from low-light red/dim settings to high-noon direct sunlight visibility.
* **Anti-Spray Rain Filtering:** Implements a 5-second persistence filter on moisture readings to differentiate real rain from transient wave splashes.
* **Solunar Feeding Rating:** Uses GPS UTC calendar calculations to estimate lunar cycle phases and displays an active feeding rating (1 to 4 stars) in Fishing Mode.

---

## Single-Button UI Interaction (GPIO 6)

### Logging Mode
* **Single Click:** Cycles display pages (`ATMOSPHERE` $\to$ `NAV` $\to$ `SEA DYNAMICS`).
* **Double Click:** Forces immediate Open-Meteo marine & wind forecast refresh over Wi-Fi.
* **Long Press (1.5s):** Switches into **Fishing Mode** and locks current coordinates as the anchor point.

### Fishing Mode
* **Single Click:** Toggles needle guidance between **Magnetic North** and **Return-to-Anchor**.
* **Double Click:** Re-anchors home position to current GPS location.
* **Long Press (1.5s):** Exits Fishing Mode and returns to Logging Mode.

---

## Project Structure

```text
JalDoot/
├── platformio.ini         # Board configuration & library dependencies
├── .gitignore             # Git ignore patterns (.pio, .vscode)
├── include/
│   ├── Config.h           # GPIO map, network credentials, alert thresholds
│   └── SensorData.h       # Shared state structures, snapshots & mode enums
└── src/
    ├── Buzzer.h           # Acoustic alerts & safety siren patterns
    ├── ButtonController.h/.cpp  # Debounced single, double, and long-press engine
    ├── CompassPointer.h/.cpp    # Non-blocking stepper needle positioner
    ├── DisplayManager.h/.cpp    # OLED multi-page UI & auto-contrast driver
    ├── SensorHub.h/.cpp   # Sensor fusion, trend analysis & debounce filters
    ├── TripManager.h/.cpp # Bearing, distance calculations & dead reckoning
    ├── WeatherApi.h/.cpp  # Open-Meteo REST API marine & wind fetcher
    └── main.cpp           # Loop timing, scheduler & peripheral integration
