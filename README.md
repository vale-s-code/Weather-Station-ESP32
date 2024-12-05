# Weather Station ESP32

This project turns an ESP32 into a weather station that displays room temperature and pressure using a BMP180 sensor, while also fetching real-time weather data from the OpenWeather API.

## Features
- Display room temperature and pressure on an OLED screen.
- Fetch current weather data for a specified city using the OpenWeather API.
- LED ring color changes based on temperature (blue for cold, green for moderate, red for hot).
- Web interface for configuring the API key, city, and country.

## Setup
1. Connect an ESP32 board and an OLED screen.
2. Install the required libraries:
   - Adafruit SSD1306
   - Adafruit BMP180
   - Adafruit NeoPixel
   - WiFiManager
   - HTTPClient
3. Configure your OpenWeather API key, city, and country via the web interface.

## Usage
- Access the web interface to input and save your OpenWeather API configuration.
- View real-time weather data on the OLED screen.
- The LED ring will change color based on the detected temperature.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
