/**
 * @file main.ino
 * @brief Reads temperature and humidity from a DHT11 sensor, measures wind speed via analog input,
 *        fetches weather forecasts from the OpenWeatherMap API, determines rain status,
 *        and sends data to ThingSpeak.
 *
 * This program interfaces with a DHT11 sensor to measure temperature (in °C) and humidity (%), and uses an analog pin
 * to read wind speed. It periodically calls the OpenWeatherMap API to get forecast information and determine whether
 * it will rain in the next two forecast intervals (6 hours). The data are posted to a ThingSpeak channel. After each
 * cycle, the program waits 10 minutes before repeating.
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

/**
 * @brief Pin used for reading wind speed via analog input.
 */
#define windPin 36

/**
 * @brief Holds the most recent wind speed measurement from the analog pin in m/s.
 *
 * @note Currently just stores the raw analog reading. A user-defined conversion
 *       to an actual speed in m/s may be implemented in @ref collectRealWeather().
 */
uint16_t windSpeed = 0;

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

#include "esp_now.h"
uint8_t broadcastAddress[] = { 0x08, 0x3A, 0xF2, 0x8E, 0xEC, 0x58 };

// Structure example to send data
// Must match the receiver structure
typedef struct packet {

  int humidity;
  int temperature;
  int wind_speed;

};

// Create a packet called dataTransmit
packet dataTransmit;

esp_now_peer_info_t peerInfo;

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

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

/**
 * @brief Arduino setup function. Initializes serial communication, DHT sensor, and connects to WiFi.
 *
 * Sets up a baud rate of 9600 for the serial monitor. Waits until WiFi is successfully connected
 * before proceeding.
 */
void setup() {
  Serial.begin(9600);
  dht11.begin();

  WiFi.mode(WIFI_STA);
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(500);
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

    // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  while (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    delay(500);
  }

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
  if (httpCode <= 0) {
    http.end();
    return -2;
  }

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

void transmitThingspeak() {
  // Initialize and connect to ThingSpeak
  ThingSpeak.begin(client);
  client.connect(server, 80);

  // Send the rain status to ThingSpeak (for example, using field 6)
  ThingSpeak.setField(6, rainStatus);
  Serial.println("Writing to ThingSpeak");
  ThingSpeak.writeFields(channelID, APIKey);

  client.stop();
}

/**
 * @brief Reads temperature, humidity, and wind speed values, storing them in global variables.
 *
 * - Temperature is stored in @ref temp (°C).
 * - Humidity is stored in @ref humidity (%).
 * - Wind speed is stored in @ref windSpeed, based on an analog read from @ref windPin.
 *
 * @note The current implementation only stores the raw analog reading as the wind speed.
 *       A user-defined formula should be applied to convert this to a real speed in m/s.
 */
void collectRealWeather() {
  temp = dht11.readTemperature();
  humidity = dht11.readHumidity();

  windSpeed = analogRead(windPin);
  // Convert analog reading to a wind speed in m/s (placeholder for user-defined calculation).
}

/**
 * @brief Arduino main loop function. Reads sensor data, retrieves forecast, sends data to ThingSpeak, and waits 10 minutes.
 *
 * The cycle is:
 *  1. Delay 3 seconds to ensure the DHT sensor refreshes data.
 *  2. Collect real-world weather data using collectRealWeather().
 *  3. Retrieve rain status from fetchRainStatus().
 *  4. Initialize ThingSpeak, connect, update fields, and disconnect.
 *  5. Delay 10 minutes before the next cycle.
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

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataTransmit, sizeof(dataTransmit));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  // Wait 10 minutes
  delay(600000);
}