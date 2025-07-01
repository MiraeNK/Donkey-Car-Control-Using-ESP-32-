# 🏎️ Donkey Car ESP32 RC Controller (ESC 1625 Brushed + E6001 Servo)

This project provides a simple yet powerful web-based remote control system for a **Donkey Car** using an **ESP32**, **1625 brushed ESC**, and **E6001 servo**.

## 🚀 Features

- 📡 **ESP32 Access Point**  
  Control the car directly via Wi-Fi without needing an external router.

- 🎮 **Web UI Controller**  
  Responsive web interface with buttons for:
  - Forward (`MAJU`)
  - Reverse (`MUNDUR`)
  - Left (`KIRI`)
  - Right (`KANAN`)
  - Speed slider (20%–100%)

- 🧠 **PWM-based ESC & Servo Control**
  - 1650 Brushed ESC (Throttle control)
  - E6001 Servo (Steering control)

## 🛠️ Hardware Used

- ESP32 Dev Board
- ESC 1625 (Brushed DC Motor Controller)
- Servo E6001
- Donkey Car Chassis (or any RC platform)
- Power source (e.g., LiPo battery)

## 🌐 How It Works

- The ESP32 creates its own Wi-Fi hotspot (`ESP32-Mobil-RC`)
- Hosts a local web server at `http://192.168.4.1`
- User accesses a custom web interface through any browser
- Web buttons send HTTP GET requests to control throttle and steering
- ESP32 maps commands to PWM signals sent to ESC and Servo

## 🧩 Controls

| Button       | Action            |
|--------------|-------------------|
| **MAJU**     | Move forward       |
| **MUNDUR**   | Move backward      |
| **KIRI**     | Steer left         |
| **KANAN**    | Steer right        |
| **Slider**   | Adjust speed (20%–100%) |

## 📦 Getting Started

1. Upload the Arduino code to your ESP32 using the Arduino IDE or PlatformIO.
2. Power the ESP32 and connect to its Wi-Fi network:
   - **SSID**: `ESP32-Mobil-RC`
   - **Password**: `password123`
3. Open your web browser and go to:  
   [http://192.168.4.1](http://192.168.4.1)
4. Use the web interface to control your car!

## 📁 Code Highlights

- Uses these libraries:
  - `WiFi.h`
  - `WebServer.h`
  - `ESP32Servo.h`
- All HTML/JavaScript is embedded via `PROGMEM`
- Static IP: `192.168.4.1` with local AP configuration
- Includes basic request handling and response for all motion and steering commands

## 📸 Screenshots / Demo



## 🧠 Credits

Developed as a personal robotics and IoT project.  
Easily adaptable for other RC car builds using brushed ESCs and standard servos.

---

Feel free to fork, modify, and expand the project.
