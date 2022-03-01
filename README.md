# MoTH Net
ESP32-based more-than-human network for wildlife sensors, cameras, and actuators. Events from the local network are posted on Adafruit IO feeds.

## How it works
The network topology is based on a main ESP32 acting as a "hub", listening to incoming ESP-Now messages and forwarding them on to Adafruit IO with MQTT. A captive portal on the hub allows for WiFi configuration and for providing AIO credentials.

## Types of sensors currently supported
Here's a list of the sensors currently supported:
- Simple switch
- Capacitive sensor
- Movement detection with ESP-Cam

## Installation
Make sure to install all libraries required through the Library manager. One ESP-32 board must be present, acting as the hub in order for messages to be forwarded to Adafruit IO. Currently, all ESP-32 sensors must be configured to run on the same WiFi channel as the hub (which in turn uses the internet router's channel).