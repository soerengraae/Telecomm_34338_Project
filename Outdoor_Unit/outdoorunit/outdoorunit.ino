/**
 * @file main.ino
 * @brief Reads temperature and humidity from a DHT11 sensor, fetches weather forecasts from the OpenWeatherMap API,
 *        determines rain status for the next few hours, and sends data to ThingSpeak.
 *
 * This program interfaces with a DHT11 sensor to measure temperature and humidity, then periodically calls the OpenWeatherMap
 * API to get forecast information. It interprets the JSON response to decide whether it will rain in the next two intervals
 * (6 hours). The rain status is posted to a ThingSpeak channel along with the sensor readings. A 10-minute delay is used
 * before the next cycle.
 */

#include "DHT.h"

/**
 * @brief Pin to which the DHT11 sensor data line is connected.
 */
#define DHT11Pin 5

/// Instance of DHT sensor, configured for DHT11 type.
DHT dht11(DHT11Pin, DHT11);

/**
 * @brief Holds the most recent temperature reading from the DHT11 sensor (in °C).
 */
uint8_t temp = 0;

/**
 * @brief Holds the most recent humidity reading from the DHT11 sensor (%).
 */
uint8_t humidity = 0;

#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "ThingSpeak.h"
#include "secrets.h"

/**
 * @brief URL for obtaining the 5-day/3-hour weather forecast from OpenWeatherMap.
 * Replace "APIKEYHERE" with your actual API key.
 */
#define forecastWeatherURL "http://api.openweathermap.org/data/2.5/forecast?lat=5.671401156455272&lon=101.41105533517317&units=metric&appid=APIKEYHERE"

/// WiFi client object for making HTTP requests and connecting to ThingSpeak.
WiFiClient client;

/**
 * @brief ThingSpeak channel write API key.
 */
const char* APIKey = ThingSpeakWriteKey;

/**
 * @brief Hostname for the ThingSpeak API.
 */
const char* server = "api.thingspeak.com";

/**
 * @brief The unique channel ID for the ThingSpeak channel.
 */
unsigned long channelID = ThingSpeakChannelID;

/**
 * @brief Stores the computed rain status based on the forecast data.
 * 
 * The mapping used is:
 *  - 0: No rain expected in the next two intervals (6 hours).
 *  - 1: Rain expected in the first interval (first 3 hours).
 *  - 2: Rain expected in the second interval (next 3 hours).
 *  - 3: Rain expected in both intervals (first and second).
 *  - Negative values returned by fetchRainStatus() indicate errors connecting or parsing data.
 */
int8_t rainStatus = 0;

/**
 * @brief Arduino setup function. Initializes serial communication, DHT sensor, and connects to WiFi.
 */
void setup() {
  Serial.begin(9600);
  dht11.begin();

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
}

/**
 * @brief Fetches and interprets the weather forecast to determine the rain status.
 *
 * Uses the OpenWeatherMap API forecast data for the next two intervals (3-hour blocks). Checks if
 * "Rain" or "Drizzle" is mentioned in the first or second forecast blocks, and assigns a status code accordingly.
 *
 * @return An integer representing the success or failure of the operation.
 *         - 0,1,2,3: Valid status for expected rain (see @ref rainStatus).
 *         - -1: WiFi not connected.
 *         - -2: HTTP GET request failed.
 *         - -3: JSON deserialization failed.
 */
int8_t fetchRainStatus() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;

  HTTPClient http;
  http.begin(forecastWeatherURL);

  int httpCode = http.GET();
  if (httpCode <= 0)
    return -2;

  String JSON_Data = http.getString();

  // JSON deserialization
  DynamicJsonDocument doc(16384); // Sizing can be optimized if needed
  DeserializationError error = deserializeJson(doc, JSON_Data);
  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
    http.end();
    return -3;
  }

  JsonArray forecastArray = doc["list"];
  int count = 0;
  bool rainFirstInterval = false;
  bool rainSecondInterval = false;

  // Process first two forecast entries (6 hours)
  for (JsonObject forecast : forecastArray) {
    if (count >= 2) break;

    const char* weather = forecast["weather"][0]["main"];
    bool isRaining = (!strcmp(weather, "Rain") || !strcmp(weather, "Drizzle"));

    if (count == 0 && isRaining) {
      rainFirstInterval = true;
    } else if (count == 1 && isRaining) {
      rainSecondInterval = true;
    }

    count++;
  }

  // Determine the rain status
  if (rainFirstInterval && rainSecondInterval) {
    rainStatus = 3;
  } else if (rainFirstInterval) {
    rainStatus = 1;
  } else if (rainSecondInterval) {
    rainStatus = 2;
  } else {
    rainStatus = 0;
  }

  http.end();
  return rainStatus;
}

/**
 * @brief Reads temperature and humidity values from the DHT11 sensor and updates global variables.
 *
 * Temperature is stored in @ref temp, and humidity is stored in @ref humidity.
 */
void collectRealWeather() {
  temp = dht11.readTemperature();
  humidity = dht11.readHumidity();
}

/**
 * @brief Arduino main loop function. Reads sensor data, retrieves forecast, sends data to ThingSpeak, and waits 10 minutes.
 *
 * The cycle is:
 *  - Delay 3 seconds to ensure the DHT sensor refreshes data.
 *  - Collect real-world weather data using collectRealWeather().
 *  - Retrieve rain status from fetchRainStatus().
 *  - Initialize ThingSpeak, connect, update fields, and disconnect.
 *  - Delay 10 minutes before the next cycle.
 */
void loop() {
  delay(3000);  // 3 seconds for the DHT to collect data
  collectRealWeather();
  Serial.println(humidity);
  Serial.println(temp);
  Serial.println(); // New line

  rainStatus = fetchRainStatus();
  // Print the rain status
  Serial.println(rainStatus);

  // Initialize and connect to ThingSpeak
  ThingSpeak.begin(client);
  client.connect(server, 80);

  // Send the rain status to ThingSpeak (for example, using field 6)
  ThingSpeak.setField(6, rainStatus);
  Serial.println("Writing to ThingSpeak");
  ThingSpeak.writeFields(channelID, APIKey);

  client.stop();

  // Wait 10 minutes
  delay(600000);
}