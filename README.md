# Smart Health & Fall Detection Wearable

An ESP32-based wearable system designed to monitor basic health parameters and detect falls using multiple sensors.

## Project Overview

This project combines health monitoring and fall detection into a compact wearable device.

The system uses an ESP32 as the main controller and integrates sensors for pulse monitoring, motion detection, location tracking, and display output.

## Features

- ❤️ Pulse and heart-rate monitoring
- 🚨 Fall detection
- 📍 GPS location tracking
- 📊 OLED display for real-time information
- 📡 ESP32-based processing
- 🔋 Portable wearable design

## Hardware

| Component | Purpose |
|---|---|
| ESP32 | Main microcontroller |
| MAX30100 | Pulse/heart-rate sensing |
| MPU6050 | Motion & fall detection |
| GPS Module | Location tracking |
| OLED Display | Displaying information |


## System Architecture

```text
MAX30100 ──┐
           │
MPU6050 ───┤
           ├──> ESP32 ──> OLED Display
GPS ───────┘
              │
              └──> Fall / Health Monitoring
```