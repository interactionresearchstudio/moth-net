# MoTH Net
ESP32-based more-than-human network for wildlife sensors, cameras, and actuators. Events from the local network are posted on Adafruit IO feeds.

## How it works
The network topology is based on a main ESP32 acting as a "hub", listening to incoming ESP-Now messages and forwarding them on to Adafruit IO with MQTT. A captive portal on the hub allows for WiFi configuration and for providing AIO credentials.

## Types of sensors currently supported
Here's a list of the sensors currently supported:
- Simple switch
- Capacitive sensor
- Movement detection with ESP-Cam

## Types of Actions currently supported
Here's a list of the actions currently supported:
- Photo capture with ESP-Cam

## Installation
Make sure to install all libraries required through the Library manager. One ESP-32 board must be present, acting as the hub in order for messages to be forwarded to Adafruit IO. Currently, all ESP-32 sensors must be configured to run on the same WiFi channel as the hub (which in turn uses the internet router's channel).

## Networking Diagram

![diagram](https://github.com/interactionresearchstudio/moth-net/blob/main/Moth-net%20diagram.jpeg)

## How to get started 

- Create an Account at https://io.adafruit.com/
- Once you're a member, you can create a feed to collect the sensor data. Each sensor will need it's own feed.
- You can view your feed data at https://io.adafruit.com/USERNAME/feeds/FEEDNAME
- Copy your Adafruit IO key and Username. These are case sensitive. 
- Download the Repository and flash program moth-net-hub to an ESP32. This will now be your Device Hub.
- Power up your Device Hub and connect to the capitve portal using.
- Enter your Adafruit IO username and key in the correct fields. Also enter your Home Wi-Fi network SSID and Password in the correct fields.
- Enter the name of your Adafruit IO feed that you have created for each sensor.
- Press save to store your credentials.
- Program a new ESP32 with the appropriate Firmware from the repository. This will now be your sensor.
- Follow the wiring diagram for each sensor to connect the correct peripherals for your sensor.
- **BUG:** Ensure your Home Wi-Fi network channel is the same as listed in the firmware of the Sensors and Device Hub.
- Power up your devices and place then in the field. We recommend the Device hub be powered close to your Home network Router inside, but within line of sight of the sensors up to 50m. We recommend trial testing sending sensor data from the sensor to the hub and checking if it has been recorded on your Adafruit IO feed before leaving devices to collect data.

## Tests

- To test your connection from Adafruit IO to hub by creating a button on a Adafruit IO dashboard connected to a feed called moth-hub-led. Make sure the button sends value 1 on press, and 0 on release

![GIF](https://media0.giphy.com/media/EEayYfkQa362ftdd3D/giphy.gif)

## To Do
 - Visual connection interface on captive portal
 - "On the Fly" Sensor and Action connections
 - Automatic ESP-NOW Channel configuration based on the Home Network Wi-Fi channel
 - Ability to deploy without home network connection
 - Change captive portal to YoYo Machines Wi-Fi captive portal named "moth-net" password "badgersandfoxes"
