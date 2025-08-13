## **Autonomous Rover monitor via ESP32 and ESP-32 CAM**

This repository contains the design, code, and documentation for my **Autonomous Rover** project.  The rover uses sensors, embedded control, and decision-making algorithms to navigate environments without human intervention.

 

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


## Features:
- **Autonomous Navigation** – Avoids obstacles without remote control.
- **Sensor Fusion** – Combines data from multiple sensors for better decisions.
- **Path Planning** – Algorithm to move toward goals.
- **Failsafe Stop** – Stops rover in case of critical errors.


## How to Run:
1. Clone the repository:
```bash
git clone https://github.com/kafkayash/Autonomous_rover_project.git
cd Autonomous_rover_project
```
2. Open the `iot_autonomous_rover.ino` file and select the ESP-32 board type you are using and make sure to set your baud rate to 1120000 on serial monitor after uploading the code to your board. After its uploaded you will see if its connecting to your wifi or not on the serial monitor. It should get connected within a minute ! but make sure your are using a 4G-connection and if using mobile hotspot set it to 2.4GHZ band instead of 5GHZ band in your settings. ESP-32 has a hard time connecting to 5G networks.

   
3. Next upload the ESP-32 cam webserver code onto your board. Although i couldn't put the whole Edge based model onto Github since its very large, you can remove the library and upload it and basic ESP-32 cam AI-Thinker has base level facial detection and recognition, for other objects you need to train it and upload the trained model. I used the Edge Impulse for this, although Tedious you get your imported Model without writing any code for it. The process it quite simple you can Refer to the following Video for reference

    [ESP-32 CAM AI-THINKER EDGE IMPULSE](https://youtu.be/HDRvZ_BYd08?si=7uLiEUAztkoOZQOL)

## Circuit Connections:
![Rover Overview](docs/images/circuit_connections.png)

These are the connections I have made but you can configure the connections as you wish just make sure to change the pin definitions in the `iot_autonomous_rover.ino` file. 




   
