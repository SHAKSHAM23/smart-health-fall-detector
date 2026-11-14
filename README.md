# Smart Health & Fall Detection Wearable

An ESP32-based wearable project that combines motion-based fall detection with basic heart-rate monitoring. The project is being developed in stages, beginning with an online simulation in Wokwi before physical sensor integration.

## Current Development Status

<details>
<summary><strong>Milestone 1 — Online simulation completed</strong></summary>

The initial ESP32 simulation has been implemented and tested in **Wokwi**.

### Implemented and tested in simulation

- ESP32 initialization and I²C communication
- MPU6050 accelerometer readings
- MPU6050 gyroscope readings
- Acceleration and rotation magnitude calculations
- SSD1306 OLED display for live readings and status
- Potentiometer-based simulated heart-rate input
- Illustrative low/high heart-rate alerts with a persistence delay
- Sequential fall-detection prototype:
  - Free-fall detection
  - Impact detection
  - Rotation-based fall confirmation
  - Automatic recovery to normal status

**Simulation note:** The potentiometer represents a simulated BPM input. It is not a real pulse sensor. The fall-detection logic uses experimental thresholds and is a prototype demonstration, not a medically validated safety system.

</details>

## Project Overview

The ESP32 acts as the main controller. Motion data from the MPU6050 is processed to identify a possible fall sequence. A simulated heart-rate value is currently used to exercise the display and alert logic. An OLED provides a compact view of sensor readings and system status.

## Features and Planned Integrations

| Component / feature | Purpose | Status |
|---|---|---|
| ESP32 | Main microcontroller | Implemented in Wokwi |
| MPU6050 | Motion sensing and fall-detection prototype | Simulated and tested |
| SSD1306 OLED | Display sensor values and status | Simulated and tested |
| Potentiometer | Temporary simulated BPM input | Simulated and tested |
| MAX30100 | Real pulse/heart-rate sensing | Planned |
| GPS module | Location tracking | Planned |
| Emergency SMS/location alert | Share alert and location | Planned |
| Battery and power management | Portable wearable operation | Planned |

## Online Simulation

The current prototype is developed and tested in **Wokwi**.

**Run the live simulation:** [Open the Smart Health & Fall Detection Wearable on Wokwi](https://wokwi.com/projects/480657816233260033)

Simulation validates the initial sensor-reading, display, simulated heart-rate, and fall-state logic. It does not yet validate real-world fall detection, physiological measurements, GPS tracking, or emergency messaging.

## Fall-Detection Prototype Logic

The current demonstration uses this sequence:

```text
NORMAL
  |
  v
FREE FALL
  |
  v
IMPACT DETECTED
  |
  v
ROTATION DETECTED
  |
  v
FALL CONFIRMED
  |
  v
RECOVERY TO NORMAL
```

The thresholds and timing windows are experimental values for simulation. Real-world falls may not follow this exact sequence and require further testing and refinement.

## System Architecture

### Current Wokwi simulation

```text
MPU6050 ───────┐
               │
Potentiometer ─┼──> ESP32 ───> SSD1306 OLED
               │       │
               │       └─────> Serial Monitor
               │
               └──> Fall detection + simulated HR alerts
```

### Planned physical integration

```text
MAX30100 ──────┐
MPU6050 ───────┤
GPS Module ────┼──> ESP32 ───> OLED Display
               │       │
               │       └─────> Emergency location alert
Battery ───────┘
```

## Development Roadmap

- [x] Set up ESP32 project in Wokwi
- [x] Connect MPU6050 and SSD1306 OLED
- [x] Read acceleration and gyroscope values
- [x] Display sensor data on OLED and Serial Monitor
- [x] Add potentiometer-based simulated BPM input
- [x] Add illustrative heart-rate alert logic
- [x] Implement and test sequential fall-state detection
- [x] Test automatic recovery to normal state
- [ ] Integrate the MAX30100 physical pulse sensor
- [ ] Integrate GPS module
- [ ] Implement and test SMS with location link
- [ ] Build and test battery-powered hardware prototype
- [ ] Validate detection performance under controlled, safe test conditions

## Repository Note

This is an actively developing student prototype. Simulation milestones are identified separately from physical hardware features so that project progress remains transparent.
