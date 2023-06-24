![Asset 29@1000x](https://github.com/mohamadlounnas/project_usdb_2023/assets/22839194/826debf8-6d75-4acd-bb86-a3efabb3cee8)

# DAMS (Project USDB -Blida 1- 2023)

This repository contains the code and documentation for Project USDB 2023, which focuses on developing a cup handling and material dispensing system using Arduino and ESP8266 microcontrollers. The project involves both mechanical and electronic components, and the code is written in C++ for Arduino and MicroPython for ESP8266.

## Table of Contents
- [General Introduction](#general-introduction)
- [Chapter 01: Mechanical Part](#chapter-01-mechanical-part)
- [Chapter 02: Electronic Part](#chapter-02-electronic-part)
- [Chapter 03: Coding Part](#chapter-03-coding-part)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## General Introduction
The Project USDB 2023 aims to create an arm equipped with a scale and a cup holder to autonomously handle cups and dispense materials into them. The arm retrieves cups placed on a tall structure, transfers them under a reservoir, pours the desired material, and evaluates the balance using the scale. The system incorporates an ultrasonic sensor to detect the presence of cups and ensures accurate measurements. The ESP8266 microcontroller provides network connectivity and allows remote control and monitoring of the system.

## Chapter 01: Mechanical Part
This chapter discusses the mechanical components of the cup handling system. It covers the arm mechanism used in the project, along with pictures showcasing its realization. The highly customizable arm model allows for easy integration with other components and efficient adaptation to specific requirements.

## Chapter 02: Electronic Part
The electronic components used in the cup handling and material dispensing system are explained in this chapter. It includes an introduction to the essential components, such as the TM1637 Display, HX711 ADC, ultrasonic sensor, Arduino Uno, and ESP8266 microcontroller. The chapter describes their roles and functionalities within the system, emphasizing their contributions to control, monitoring, and user interaction.

## Chapter 03: Coding Part
This chapter focuses on the coding aspect of the project. It introduces the various techniques and technologies employed, including MicroPython, I2C communication protocol, Flutter framework for mobile app development, microservices architecture, design patterns, and the "no delay" approach using millis(). The logical diagram of the system's operation is presented, highlighting the flow and interactions between different components.

## Installation
To use the code in this repository, follow these steps:
1. Download the Arduino sketch file (`arduino.ino`) from [here](https://github.com/mohamadlounnas/project_usdb_2023/arduino.ino).
2. Download the MicroPython script file (`esp8266.py`) from [here](https://github.com/mohamadlounnas/project_usdb_2023/esp8266.py).
3. Upload the Arduino sketch to the Arduino Uno board.
4. Flash the MicroPython script onto the ESP8266 microcontroller.

## Usage
After installation, you can control and monitor the cup handling system through the user interface or application developed using Flutter. The ESP8266 microcontroller enables network connectivity, allowing remote operation and oversight. You can send commands to initiate specific actions, such as cup retrieval, material pouring, or cup discarding. Real-time updates and monitoring of system status can be accessed through the user interface or displayed on a server.

## Contributing
Contributions to this project are welcome. If you encounter any issues or have suggestions for improvements, please open an issue or submit a pull request.

## License
This project is licensed under the [MIT License](LICENSE).
