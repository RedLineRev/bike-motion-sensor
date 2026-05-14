# Bike Motion Sensor

A wearable data logger for mountain bike tricks. An ESP32-S3 samples a 9-axis IMU (ICM-20948) at 200 Hz and writes binary records to a microSD card. After a ride, a Python notebook loads the data, syncs it with GoPro footage, and lets you annotate maneuvers to build a labeled dataset.

## Phase 1 Goal
Ship a working data capture device in two weeks. No ML on-device yet — just clean, reliable logging at high sample rate with a sync mechanism for video alignment.

## Hardware
| Part | Description |
|------|-------------|
| Adafruit Feather ESP32-S3 | MCU — USB-C, Li-Po charging, Stemma QT |
| Adafruit ICM-20948 | 9-axis IMU — accel, gyro, magnetometer |
| microSD breakout (SPI) | Storage |
| 1200 mAh Li-Po | Power |
| SPDT slide switch | Power on/off |
| Status LED | Logging feedback |

## Architecture
Log raw IMU data to SD card → pull card after ride → process in Python.

```
[ICM-20948] → I²C → [ESP32-S3] → SPI → [microSD]
                         +
                   [GoPro footage]  ← synced via triple-tap
```

## Repo Layout
```
firmware/       PlatformIO project
analysis/       Jupyter notebooks
data/           Raw SD card dumps (gitignored)
docs/           Project brief and reference docs
decisions.md    Design choices and rationale
```

## Setup
1. Install VS Code + PlatformIO extension
2. `git clone` this repo
3. Open `firmware/` in PlatformIO to build and flash
4. `pip install -r analysis/requirements.txt` for the Python side

## Status
Phase 1 — in progress.
