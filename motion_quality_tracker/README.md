# Firmware – Motion Quality Tracker

This folder contains the **firmware source code** for the Motion Quality Tracker project. The firmware runs on the ESP32-WROVER and handles IMU data collection, on-device AI inference, and MQTT communication with the feedback web app.

---

## Overview

The firmware workflow is built around four key modules:

1. **IMU Driver (`IMU_driver.c/h`)**  
   Interfaces with the motion sensor over SPI to collect acceleration and gyroscopic data.

2. **AI Inference (Edge Impulse SDK)**  
   Runs a TensorFlow Lite model using the `edge-impulse-sdk/` directory, classifying squats in real time.

3. **MQTT Module (`mqtt_module.c/h`)**  
   Connects to a local MQTT broker over Wi-Fi and publishes feedback results (e.g., `Correct Squat`, `Incorrect Squat`).

4. **Main Logic (`main.cpp`)**  
   - Initializes all modules  
   - Loops: collects features → classifies → sends results via MQTT

---

## File Structure

```bash
main/
├── inc/                  # Header files
│   ├── IMU_driver.h
│   └── mqtt_module.h
├── src/                  # Source files
│   ├── IMU_driver.c
│   └── mqtt_module.c
├── main.cpp              # Application entry point (app_main)
├── CMakeLists.txt        # Component build config
```
---

## Build & Flash Instructions (ESP-IDF)

Before building, make sure you have [ESP-IDF installed](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) and sourced in your terminal.

---

### 1. Configure Wi-Fi and MQTT Settings

Before building the project, you must **set your Wi-Fi network credentials** and **MQTT broker IP address** in `main.cpp`:

```cpp
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASS "YourWiFiPassword"
#define MQTT_BROKER_URI "mqtt://192.168.x.x:1883"
````

Detailed setup instructions can be found in this repository: [MQTT-webpage-example – GitHub](https://github.com/LovroSantek/MQTT-webpage-example)

---

### 2. Build and Flash the Firmware

From the root of your project (where `main/` is located), run:

```bash
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

* Replace `/dev/ttyUSB0` with the actual port your ESP32 is connected to (check with `ls /dev/tty.*` on macOS or `Device Manager` on Windows).
* Use `idf.py monitor` to view real-time logs after flashing.

---

### Notes

* The firmware connects to your Wi-Fi and starts publishing classification results to the MQTT broker.
* You can verify message reception using the companion web app above.
* Ensure your MQTT broker is running and accessible from the ESP32 (i.e., same local network).

---
---

### Running Wirelessly

Once the firmware is flashed and the system is verified to be working over USB:

- Disconnect the ESP32 from your computer
- Connect a charged Li-ion battery (e.g. 3.7V 900mAh) via the BQ24295 charger board
- The device will automatically boot and connect to Wi-Fi

**When everything works, the ESP32 can be powered by battery and is fully ready for standalone wireless use**  
No computer connection is required after initial flashing.



