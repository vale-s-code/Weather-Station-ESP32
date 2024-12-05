# Weather Station with ESP32 and BMP180 Sensor

This project turns an ESP32 microcontroller into a comprehensive weather station. The device uses a BMP180 sensor to measure room temperature and atmospheric pressure, and fetches real-time weather data from the OpenWeather API. All the data is displayed on an OLED screen, with an integrated LED ring that changes color based on the temperature.

## Features

- 🌡️ **Room Temperature & Pressure Monitoring**
  - Uses a BMP180 sensor to measure room temperature and atmospheric pressure, displaying the readings on an OLED screen.

- 🌐 **Real-Time Weather Data**
  - Fetches live weather data for a user-defined city and country from the OpenWeather API. The data includes the current temperature and other weather details.

- 🖥️ **OLED Display Integration**
  - Displays room temperature, atmospheric pressure, and weather data in a clean and readable format on an OLED screen using the Adafruit SSD1306 library.

- 💡 **Dynamic LED Indicator**
  - Uses a NeoPixel LED ring to display temperature changes, with blue for cold, green for moderate, and red for hot temperatures.

- 🌍 **Web Configuration Interface**
  - Configure your OpenWeather API key, city, and country code through a simple web interface hosted on the ESP32. No need to modify the code after the initial setup.

- 🚨 **Error Handling**
  - Displays error messages for invalid or missing configurations, failed API requests, or if the city is not found.

## Getting Started

### Hardware Requirements

- ESP32 microcontroller
- BMP180 sensor for temperature and pressure measurements
- NeoPixel LED ring (1 or more LEDs)
- SSD1306-compatible OLED display (e.g., 128x64 pixels)
- Breadboard and jumper wires

### Software Requirements

- Arduino IDE (with ESP32 board support installed)
- Required libraries:
  - WiFiManager
  - Adafruit SSD1306
  - Adafruit GFX
  - Adafruit BMP180
  - Adafruit NeoPixel
  - HTTPClient
  - Preferences

### Configuration Steps

1. 📥 **Clone or Download**  
   Clone this repository to your local machine.

2. 🛠️ **Open the Code**  
   Open the `.ino` file in the Arduino IDE.

3. 🔧 **Initial Setup**  
   Upload the code to your ESP32. Upon the first run, the device will host a Wi-Fi setup portal named `ESP32-Config`. Connect to this network using a smartphone or computer, and follow the web interface to input:
   - Wi-Fi credentials
   - OpenWeather API key
   - City and country code

4. 🌍 **Change Configuration Anytime**  
   You can always access the web interface later to update Wi-Fi credentials or change the city and country settings without re-uploading the code.
