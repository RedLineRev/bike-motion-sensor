# Bike Motion Sensor — Claude Context

## What This Project Is
A wearable bike motion sensor that captures 9-axis IMU data at 200 Hz to a microSD card during trail rides. Goal: build a labeled dataset of MTB tricks (bunny hops, drops, manual sections) that can later be used to train a classifier. Phase 1 is pure data capture — no ML on-device yet.

## Decisions Log

### 2026-05-16 — platformio.ini must include USB CDC build flags
**Decision:** `platformio.ini` must include both `-DARDUINO_USB_MODE=1` and `-DARDUINO_USB_CDC_ON_BOOT=1` under `build_flags` for any sketch that uses `Serial`.

**Context:** Diagnosed at Dad's office Mac. Without these flags the ESP32-S3 compiles and uploads cleanly, but `Serial.println()` output routes to the chip's UART pins instead of the USB port — so the Serial Monitor shows nothing. Adding the flags routes Serial through the native USB CDC interface and output appears immediately.

**I2C scan results from same session:** ICM-20948 confirmed at address `0x69`. Onboard MAX17048 LiPo fuel gauge confirmed at address `0x36` — this is normal for this revision of the Feather ESP32-S3 and can be ignored.

## Hardware Stack
- **MCU:** Adafruit Feather ESP32-S3 (PID 5323) — Wi-Fi + BLE, USB-C, Li-Po charging, Stemma QT
- **IMU:** Adafruit ICM-20948 (PID 4554) — accel + gyro + magnetometer, 200+ Hz, I²C via Stemma QT
- **Cable:** Adafruit PID 4397 — Stemma QT JST SH 4-pin plug on the IMU end, 4 female socket headers on the other end (150 mm). **Note:** an earlier version of this file incorrectly described PID 4397 as "Stemma QT to Stemma QT" — that was wrong. The female sockets slip over the Feather's male pin headers at SDA, SCL, 3V, and GND. Cable works correctly for bench testing. Upgrade to a true Stemma QT to Stemma QT cable (snap connection on both ends, more vibration-resistant) is under consideration for the production assembly.
- **Storage:** microSD breakout (PID 254) — SPI, binary record writes
- **Power:** 1200 mAh Li-Po (PID 258) + SPDT slide switch inline on battery positive
- **UI:** single status LED + 330 Ω resistor

## Current State
- **Latest milestone (2026-05-16):** Hardware verified working. `platformio.ini` USB CDC fix applied. I2C scan confirmed ICM-20948 at 0x69 and MAX17048 at 0x36. Live IMU values printing via Adafruit ICM20948 example sketch.
- **Next milestone:** Log IMU data to the SD card (binary record format, start/stop via switch, LED feedback).
- **Blockers:** Cable upgrade decision pending (PID 4397 vs. Stemma-to-Stemma for production assembly).

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

## Critical Behaviors
- **Read component documentation first.** Before writing code for any hardware component or library, fetch the official docs. Never assume pin numbers, I2C addresses, or library defaults.
- **Sync at ride start and end.** Three taps write a sync marker; GoPro footage is the ground truth for labeling.
- **Update CLAUDE.md at session end.** Every session that produces a decision, a milestone change, a new piece of hardware, a corrected inventory item, or a new blocker must update the relevant section (Decisions Log, Current state, Hardware) before ending. Commit the change with a brief git message and push to origin so the next session and any other contributor stay in sync.

## What Is Out of Scope for Phase 1
- Live feedback / display
- BLE or Wi-Fi data streaming
- On-device ML inference
- Phone app
- Automatic trick detection
