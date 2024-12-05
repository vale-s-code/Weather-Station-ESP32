/*
 * File:   Home_Challange_003_VS.ino
 * Author: Valentin S. 
 *
 * About:
 * This program is a weather station using an ESP32 with a BMP180 sensor for room temperature and pressure, and it fetches weather data from OpenWeather API. 
 * The data is displayed on an OLED screen, and the LED color changes based on the temperature. 
 * Users can configure the API key, city, and country via a web interface hosted on the device. 
 * Before you execute the code and load it onto your device, fill in the placeholders with your data.
 *
 * The API for open weather can be created via a free account here: 
 * https://home.openweathermap.org/users/sign_up
 *
 * Created on 02.12.2024, 18:35
 * 
 * Extra:
 * You can easily change the location of the displayed weather.
 * To do this, you only need to edit the Location via the Web interface.
 * 
 * You can check the function of your API calls with these links:
 * https://api.openweathermap.org/data/2.5/weather?q=[default location]&APPID=[API - KEY]
 *
 * https://api.openweathermap.org/data/2.5/weather?q=berlin,de&APPID=swgb3590t6hergbu
 * https://api.openweathermap.org/data/2.5/weather?q=London,uk&APPID=swgb3590t6hergbu
 *
 */
 
#include <FS.h>
#include <WiFi.h>
#include <WiFiManager.h> 
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "Adafruit_NeoPixel.h"
#include <Preferences.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiManager wifiManager;
Preferences prefs;

Adafruit_BMP085 bmp; 
#define LED_PIN 7
#define NUM_LEDS 1
Adafruit_NeoPixel LEDRing(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

String apiKey = "";
String city = "";
String country = "";
const String baseApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=";

WebServer server(8080);

unsigned long lastSwitchTime = 0;
bool showWeatherScreen = true;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  LEDRing.setBrightness(40);
  LEDRing.begin();

  if (!bmp.begin()) {
    Serial.println("BMP180 Sensor nicht gefunden!");
    while (1) {}
  }

  if (!wifiManager.autoConnect("ESP32-Config", "password")) {
    Serial.println("Failed to connect, restarting...");
    delay(3000);
    ESP.restart();
  }

  prefs.begin("weather-config", false);
  apiKey = prefs.getString("apiKey", "");
  city = prefs.getString("city", "");
  country = prefs.getString("country", "");

  setupWebServer();
  server.begin();
  Serial.println("Server gestartet");
}

void loop() {
  server.handleClient();

  if (millis() - lastSwitchTime > 10000) {
    lastSwitchTime = millis();
    showWeatherScreen = !showWeatherScreen;
  }

  if (showWeatherScreen) {
    displayWeather();
  } else {
    displayRoomConditions();
  }
}

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = "<html><body><h2>Weather Configuration</h2>";
    html += "<form method='POST' action='/saveConfig'>";
    html += "API Key: <input type='text' name='apiKey' value='" + apiKey + "'><br>";
    html += "City: <input type='text' name='city' value='" + city + "'><br>";
    html += "Country Code: <input type='text' name='country' value='" + country + "'><br>";
    html += "<input type='submit' value='Save Configuration'>";
    if (city == "" || country == "" || apiKey == "") {
      html += "<p style='color:red;'>Error: Missing or invalid configuration!</p>";
    }
    html += "</form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/saveConfig", HTTP_POST, []() {
    String newApiKey = server.arg("apiKey");
    String newCity = server.arg("city");
    String newCountry = server.arg("country");

    String errorMessage = "";

    if (newApiKey.length() == 0) {
      errorMessage += "API Key is required.<br>";
    }
    if (newCity.length() == 0) {
      errorMessage += "City is required.<br>";
    }
    if (newCountry.length() != 2) {
      errorMessage += "Country Code must be 2 characters long.<br>";
    }

    if (errorMessage.length() > 0) {
      String message = "<html><body><h2>Error: Invalid Configuration!</h2>";
      message += "<p style='color:red;'>" + errorMessage + "</p>";
      message += "<a href='/'>Go back</a>";
      message += "</body></html>";
      server.send(400, "text/html", message);
    } else {
      String weatherData = getWeatherData(newApiKey, newCity, newCountry);
      if (weatherData == "Invalid API Key!" || weatherData == "City not found!" || weatherData == "HTTP Error!") {
        String message = "<html><body><h2>Error: Invalid Configuration!</h2>";
        message += "<p style='color:red;'>" + weatherData + "</p>";
        message += "<a href='/'>Go back</a>";
        message += "</body></html>";
        server.send(400, "text/html", message);
      } else {
        apiKey = newApiKey;
        city = newCity;
        country = newCountry;
        prefs.putString("apiKey", apiKey);
        prefs.putString("city", city);
        prefs.putString("country", country);

        String message = "<html><body><h2>Configuration Saved!</h2>";
        message += "<p>API Key, City, and Country have been saved.</p>";
        message += "<a href='/'>Go back</a>";
        message += "</body></html>";
        server.send(200, "text/html", message);
      }
    }
  });
}

void displayWeather() {
  String weatherData = getWeatherData(apiKey, city, country);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (weatherData == "Config missing!") {
    display.setCursor(0, 0);
    display.println("Error: Invalid Configuration!");
    display.println("Missing API Key, City or Country.");
    display.display();
    return;
  }

  if (weatherData == "Invalid API Key!" || weatherData == "City not found!" || weatherData == "HTTP Error!") {
    display.setCursor(0, 0);
    display.println("Error: Invalid Configuration!");
    display.println(weatherData);
    display.display();
    return;
  }

  display.setCursor(40, 20);
  display.println(city);
  display.setCursor(40, 30);
  display.println("Temp. (C):");
  display.setCursor(40, 40);
  display.println(weatherData);
  display.display();
}

String getWeatherData(String newApiKey, String newCity, String newCountry) {
  HTTPClient http;
  String weatherData = "Loading...";
  if (newCity == "" || newCountry == "" || newApiKey == "") {
    return "Config missing!";
  }

  String encodedCity = newCity;
  encodedCity.replace(" ", "%20");
  String apiUrl = baseApiUrl + encodedCity + "," + newCountry + "&appid=" + newApiKey + "&units=metric";
  
  http.begin(apiUrl);
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String response = http.getString();
    float temp = extractTemperature(response);
    if (temp != -999) {
      weatherData = String(temp) + " C";
    } else {
      weatherData = "Parsing error";
    }
  } else {
    if (httpResponseCode == 401) {
      weatherData = "Invalid API Key!";
    } else if (httpResponseCode == 404) {
      weatherData = "City not found!";
    } else {
      weatherData = "HTTP Error!";
    }
  }
  http.end();
  return weatherData;
}

float extractTemperature(String response) {
  int tempIndex = response.indexOf("\"temp\":");
  if (tempIndex != -1) {
    int start = tempIndex + 7;
    int end = response.indexOf(',', start);
    if (end != -1) {
      String tempStr = response.substring(start, end);
      return tempStr.toFloat();
    }
  }
  return -999;
}

void displayRoomConditions() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(40, 20);
  display.println("RoomTemp:");

  display.setCursor(40, 30);
  display.println(temperature);

  display.setCursor(40, 45);
  display.println("Druck:");

  display.setCursor(40, 55);
  display.println(pressure);
  display.display();
}

void updateLED(float temp) {
  if (temp < 10) {
    LEDRing.setPixelColor(0, LEDRing.Color(0, 0, 255)); // Blue
  } else if ( (temp < 20) && (temp > 10) ) {
    LEDRing.setPixelColor(0, LEDRing.Color(0, 255, 0)); // Green
  } else {
    LEDRing.setPixelColor(0, LEDRing.Color(255, 0, 0)); // Red
  }
  LEDRing.show();
}
