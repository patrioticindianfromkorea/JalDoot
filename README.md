# JalDoot — ESP32-C3 Marine Telemetry & Fishing Station

A live boat instrument console and navigation station powered by an ESP32-C3. Built as a zero-storage, real-time telemetry snapshot system, JalDoot monitors atmospheric pressure trends, wave kinematics, GNSS fixes, and marine forecasts, displaying critical safety warnings and navigational guidance on an SSD1306 OLED and an external stepper pointer.

---

## Hardware Architecture & Pin Map

> **SuperMini Power Notice:** Connect the **NEO-6M GPS** and **Active Buzzer** to the **`5V` / `VIN` pin** (USB bus rail), keeping only logic sensors on the `3.3V` rail to prevent LDO brownouts. Strapping pins **GPIO 0 & 1** are left disconnected for boot safety.

| Peripheral / Sensor | Interface / Pins | Operating Role |
| :--- | :--- | :--- |
| **OLED Display (SSD1306)** | Shared I2C (`SDA=8`, `SCL=9`)\* | Live system snapshot rendering & vector arrow |
| **BMP280 Barometer** | Shared I2C (`SDA=8`, `SCL=9`)\* | Atmospheric pressure & 3-hour squall tendency tracking |
| **MPU-6500 IMU** | Shared I2C (`SDA=8`, `SCL=9`)\* | Pitch/roll angles, heel safety & wave rocking index |
| **HMC/QMC5883L** | Shared I2C (`SDA=8`, `SCL=9`)\* | Tilt-compensated magnetic heading |
| **u-blox NEO-6M GPS** | UART1 (`RX=20`, `TX=21`) | Position fix, ground speed, UTC date & solunar clock |
| **DHT22 Sensor** | 1-Wire Digital (`GPIO 7`) | Cabin/ambient air temperature & relative humidity |
| **TEMT6000 Daylight** | ADC1_CH2 (`GPIO 2`) | Ambient lux detection for OLED auto-contrast |
| **Raindrop Sensor** | ADC1_CH3 (`GPIO 3`) | Sea-spray filtered precipitation detection |
| **Water Level Sensor** | ADC1_CH4 (`GPIO 4`) | Bilge water ingress & rate-of-rise monitoring |
| **Stepper Driver (MP6500)** | Digital (`STEP=5`, `DIR=18`, `EN=19`) | Shortest-path physical compass/anchor needle |
| **Tactile Pushbutton** | Digital Input (`GPIO 6`) | Multifunction single-button control interface |
| **5V Active Buzzer** | Digital Output (`GPIO 10`) | Multi-cadence acoustic alarms (storms, bilge, capsize) |

*\*Note: If GPIO 8 & 9 conflict with the SuperMini's onboard LED and Boot switch, remap I2C to `SDA=4, SCL=5` in `include/Config.h`.*

---

## Key Firmware Features

* **Barometric Storm Warning Engine:** Maintains a rolling 3-hour barometric buffer (sampled every 5 minutes). A rapid pressure drop ($\le -3.0\text{ hPa}$) triggers an audible squall alarm and on-screen storm warning.
* **Bilge Ingress Rate-of-Rise Alarm:** Evaluates water level changes dynamically at $1\text{ Hz}$. Alerts trigger only if depth crosses the threshold alongside a rapid positive rate-of-rise ($>180$ ADC units in 10s), filtering out sloshing waves while underway.
* **Capsize & Severe Heel Detection:** Sustained listing or roll exceeding $\pm 35^\circ$ for longer than 2 seconds sounds an emergency cadence and takes over the OLED with a high-priority `CAPSIZE!!` warning.
* **Dead Reckoning Fallback:** When GPS fix drops due to spray or superstructure obstruction, the system transitions to `*EST DEAD-RECK` mode, estimating position using last known ground speed, elapsed time ($\Delta t$), and compass heading.
* **Adaptive OLED Contrast:** Reads solar lux from the TEMT6000 and writes directly to the SSD1306 contrast register (`SSD1306_SETCONTRAST`) to maximize direct sunlight visibility by day and preserve night vision after dark.
* **Anti-Spray Rain Filtering:** Implements a 5-second persistence filter on moisture readings to differentiate genuine rainfall from transient sea-spray splashes.
* **Solunar Feeding Rating:** Uses GPS UTC calendar timestamps and Conway's lunar algorithm to compute moon phases (`New`, `1stQ`, `Full`, `3rdQ`) and displays a 1-to-4 star bite activity rating on the Fishing screen.

---

## Single-Button UI Interaction (GPIO 6)

### Logging Mode
* **Single Click:** Cycles display pages (`ATMOSPHERE` $\to$ `NAV` $\to$ `SEA DYNAMICS`).
* **Double Click:** Forces immediate Open-Meteo marine wave & wind forecast refresh over Wi-Fi.
* **Long Press (1.5s):** Switches into **Fishing Mode** and locks current coordinates as the anchor point.

### Fishing Mode
* **Single Click:** Toggles needle guidance between **Magnetic North** and **Return-to-Anchor**.
* **Double Click:** Re-anchors home position to current GPS location.
* **Long Press (1.5s):** Exits Fishing Mode and returns to Logging Mode.

---

## Project Structure

```text
JalDoot/
├── platformio.ini         # Board configuration & modern library dependencies
├── .gitignore             # Git ignore patterns (.pio, .vscode, local caches)
├── README.md              # System documentation
├── include/
│   ├── Config.h           # Safe GPIO pin map, Wi-Fi credentials, thresholds
│   └── SensorData.h       # LiveSnapshot state struct & system mode enums
└── src/
    ├── Buzzer.h           # Non-blocking acoustic alert cadences
    ├── ButtonController.h/.cpp  # Debounced single, double, and long-press engine
    ├── CompassPointer.h/.cpp    # Circular shortest-path stepper needle driver
    ├── DisplayManager.h/.cpp    # SSD1306 OLED pages & auto-contrast driver
    ├── SensorHub.h/.cpp   # Direct register I2C polling, ADC, 1-Wire & GPS
    ├── TripManager.h/.cpp # Haversine distance, bearing & dead reckoning
    ├── WeatherApi.h/.cpp  # Open-Meteo REST API marine & wind client
    └── main.cpp           # 10Hz scheduler loop & high-speed stepper generator
