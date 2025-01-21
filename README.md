This project is a vehicle speed monitoring system built using an Arduino Uno R4 WiFi, a 20x4 I2C LCD screen, a button, a buzzer, an MPU6050 accelerometer, a NEO-6M GPS module, and a Raspberry Pi camera. The system connects to a Wi-Fi hotspot to track your location, measure acceleration, monitor speed, and stream video. The data is sent to a ThingsBoard server for visualization, which displays the GPS data on a map and provides speed insights. If the speed exceeds 50 km/h, the buzzer sounds an alarm unless the button is pressed to stop it. The LCD displays real-time information, including GPS coordinates, satellite count, and speed.
Also while it searches for wifi or if connection is lost it plays the star wars music in the buzzer
## Features

- **GPS Location Tracking**: Displays real-time longitude and latitude.
- **Speed Monitoring**: Monitors vehicle speed using GPS data.
- **Over-speed Alarm**: Sounds a buzzer when speed exceeds 50 km/h. 
- **Manual Override**: Press a button to stop the buzzer.
- **LCD Display**:
  - Line 1: Longitude
  - Line 2: Latitude
  - Line 3: Number of satellites
  - Line 4: Current speed (km/h)
- **Data Visualization**:
  - Sends and visualizes GPS location on a map on the thingsboard server.
  - Displays speed insights in real-time.
- **Video Streaming**:
  - Streams live video from a Raspberry Pi camera for driver monitoring.

## Hardware Components

- Arduino Uno R4 WiFi
- 20x4 I2C LCD screen
- Button
- Buzzer
- MPU6050 accelerometer module
- NEO-6M GPS module
- Raspberry Pi with camera module
- Jumper wires
- Breadboard (optional for prototyping)


## Circuit Diagram


### NEO-6M GPS Module:
- VCC -> 5V
- GND -> GND
- TX -> RX (Pin 0)
- RX -> TX (Pin 1)

### MPU6050 Accelerometer:
- VCC -> 5V
- GND -> GND
- SDA -> A4 (I2C SDA)
- SCL -> A5 (I2C SCL)

### 20x4 I2C LCD Screen:
- VCC -> 5V
- GND -> GND
- SDA -> A4
- SCL -> A5

- ### Buzzer:
- Positive -> Pin 2
- Negative -> GND

### Button:
- One terminal -> GND
- Other terminal -> Pin 5(with a pull-up resistor)

### Raspberry Pi Camera:
- Connect the camera module to the Raspberry Pi's camera interface.

## Software Requirements
- Arduino IDE (version 1.8.19 or later)
- Required Libraries:
  - [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus)
  - [Wire](https://www.arduino.cc/en/Reference/Wire)
  - [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)
  - [Adafruit MPU6050](https://github.com/adafruit/Adafruit_MPU6050)
  - [ArduinoJson](https://arduinojson.org/)
  - [WiFiS3](https://github.com/arduino-libraries/WiFi)
  - [MPU6050_light](https://github.com/rfetick/MPU6050_light)
  - [Pitches](https://github.com/hibit-dev/buzzer/blob/master/lib/pitches.zip)
- ThingsBoard instance for data visualization
- Raspberry Pi OS with camera streaming software (e.g., MJPG-Streamer or similar)

- ## Usage
0. After you succesfuly wired everything together
1. Power on the Arduino Uno R4 WiFi and Raspberry Pi.
2. Connect the system to a Wi-Fi hotspot (with the credentials written in code).
3. Observe the LCD for real-time GPS data.
4. Access the ThingsBoard dashboard to view the GPS map and speed insights.
5. Use the Raspberry Pi camera stream to monitor the driver.
6. Drive the vehicle:
   - If the speed exceeds 50 km/h, the buzzer will beep.
   - Press the button to stop the buzzer.


