## **Autonomous Rover monitor via ESP32 and ESP-32 CAM**

This repository contains the design, code, and documentation for my **Autonomous Rover** project.  The rover uses sensors, embedded control, and decision-making algorithms to navigate environments without human intervention.

 
---

## Project Overview:

![Rover Overview](docs/images/front_view.png)
![Rover Overview](docs/images/back_view.png)


![Rover Overview](docs/images/top_view.png)
![Rover Overview](docs/images/side_view.png)


The rover is built with:
- **Chassis:** 4-wheel or tracked platform.
- **Sensors:** Ultrasonic HCSR-04,DHT11,MPU6050.
- **Controller:** ESP-32 and ESP-32 CAM.
- **Actuation:** DC motors with motor driver(L298N).
- **Power:** Li-ion battery pack and 2S-Li ion BMS.
- **converter:** Used 3.7v to 5V converter JX-885Y( To provide power supply to Microcontrollers)

---

## Features:
- **Autonomous Navigation** – Avoids obstacles without remote control.
- **Sensor Fusion** – Combines data from multiple sensors for better decisions.
- **Path Planning** – Algorithm to move toward goals.
- **Failsafe Stop** – Stops rover in case of critical errors.

---

## How to Run:
1. Clone the repository:
```bash
git clone https://github.com/kafkayash/Autonomous_rover_project.git
cd Autonomous_rover_project
```
2. Open the 
