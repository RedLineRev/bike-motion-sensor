# Bike Motion Sensor — Claude Context

## What This Project Is
A wearable bike motion sensor that captures 9-axis IMU data at 200 Hz to a microSD card during trail rides. Goal: build a labeled dataset of MTB tricks (bunny hops, drops, manual sections) that can later be used to train a classifier. Phase 1 is pure data capture — no ML on-device yet.

## Hardware Stack
- **MCU:** Adafruit Feather ESP32-S3 (PID 5323) — Wi-Fi + BLE, USB-C, Li-Po charging, Stemma QT
- **IMU:** Adafruit ICM-20948 (PID 4554) — accel + gyro + magnetometer, 200+ Hz, I²C via Stemma QT
- **Storage:** microSD breakout (PID 254) — SPI, binary record writes
- **Power:** 1200 mAh Li-Po (PID 258) + SPDT slide switch inline on battery positive
- **UI:** single status LED + 330 Ω resistor

## Data Flow
```
[ICM-20948] → I²C → [ESP32-S3] → SPI → [microSD card]
                          +
                    [GoPro footage]  ← synced via 3-tap at ride start
```

## Architecture Decision
**Option B (Log to SD, process offline in Python).** Chosen over on-device TinyML (Option A) and BLE streaming (Option C). Raw data preserved for re-processing; lowest build complexity; real chance to ship in two weeks; hardware path unchanged when BLE is added in Phase 3.

## Firmware Approach
- PlatformIO + Arduino framework in VS Code
- Main loop: sample ICM-20948 at 200 Hz, write binary records to SD
- Triple-tap detection: 3 sharp taps within ~1 second → write sync marker to log file
- Status LED: solid = idle/ready, fast blink = logging, slow blink = error

## Binary Record Format (to be finalized)
Each record: timestamp (ms, uint32) + ax, ay, az (float32) + gx, gy, gz (float32) + mx, my, mz (float32) = 44 bytes per sample

## Python / Analysis
- Jupyter notebooks in `analysis/`
- Load binary log, plot traces, annotate events, eventually train classifier
- Libraries: pandas, numpy, matplotlib, scipy

## Repository Layout
```
bike-motion-sensor/
  firmware/          PlatformIO project (src/, include/, platformio.ini)
  analysis/          Jupyter notebooks and helper scripts
  data/              .gitignored — raw SD card dumps go here
  docs/              Project brief HTML and any reference PDFs
  decisions.md       Running log of design choices and why
  README.md
  CLAUDE.md          (this file)
  .gitignore
```

## Phase 1 Deliverables
1. Device powers on from battery, samples IMU at 200 Hz, writes binary records to SD
2. Switch to start/stop logging; LED status feedback
3. Triple-tap sync event reliably written to log
4. 3D-printed enclosure on bottle cage bolts of a full-suspension frame
5. At least one real trail ride captured with GoPro footage
6. Python notebook that loads, plots, and annotates a ride

## Key Constraints
- **Budget:** ~$80 total BOM
- **Timeline:** 2 weeks for Phase 1
- **File format:** `.env` and `config.h` are gitignored — never commit secrets
- **Commit habit:** every time something new works, commit. Describe what it does, not "wip".

## What Is Out of Scope for Phase 1
- Live feedback / display
- BLE or Wi-Fi data streaming
- On-device ML inference
- Phone app
- Automatic trick detection
