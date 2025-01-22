/**
 * @file outdoorunit.ino
 * @brief Outdoor unit for weather monitoring and data transmission.
 * 
 * This file collects real-time weather data using sensors and fetches forecast data from
 * OpenWeatherMap API. The collected data is then sent via ESP-NOW to a designated peer.
 */

#include "DHT.h"

#define DHT11Pin 5 /**< GPIO pin connected to DHT11 sensor */

DHT dht11(DHT11Pin, DHT11); /**< DHT11 sensor instance */

uint8_t temp = 0; /**< Current temperature reading */
uint8_t humidity = 0; /**< Current humidity reading */

#define windPin 35 /**< GPIO pin connected to wind speed sensor */

uint16_t windSpeed = 0; /**< Current wind speed reading */

#define lightPin 26 /**< GPIO pin connected to light sensor */
uint8_t lightStatus = 0; /**< Current light status */

#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "ThingSpeak.h"
#include "secrets.h"

#define forecastWeatherURL "http://api.openweathermap.org/data/2.5/forecast?lat=55.78598981728918&lon=12.522723007973173&units=metric&appid=APIHERE" /**< URL for fetching weather forecast */

WiFiClient client; /**< WiFi client for HTTP requests */

#include "esp_now.h"
uint8_t broadcastAddress[] = { 0x08, 0x3A, 0xF2, 0x8E, 0xEC, 0x58 }; /**< Broadcast address for ESP-NOW communication */

/**
 * @struct packet
 * @brief Structure to hold weather data for transmission.
 */
typedef struct {
  uint8_t humidity; /**< Humidity percentage */
  uint8_t temperature; /**< Temperature in Celsius */
  uint8_t wind_speed; /**< Wind speed in m/s */
  uint8_t light; /**< Light status (e.g., on/off) */
  uint8_t rain; /**< Rain status */
} packet;

esp_now_peer_info_t peerInfo; /**< ESP-NOW peer information */

int8_t rainStatus = 0; /**< Current rain status */

/**
 * @brief Callback function called when data is sent via ESP-NOW.
 * 
 * @param mac_addr MAC address of the receiver.
 * @param status Status of the send operation.
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

/**
 * @brief Setup function initializes serial communication, DHT11 sensor, and light sensor pin.
 */
void setup() {
  Serial.begin(9600);
  dht11.begin();

  pinMode(lightPin, INPUT);
}

/**
 * @brief Fetches the rain status from the OpenWeatherMap forecast API.
 * 
 * Connects to WiFi, retrieves forecast data, parses JSON response, and determines rain status.
 * 
 * @return int8_t Rain status:
 *               - 0: No rain
 *               - 1: Rain in the first interval
 *               - 2: Rain in the second interval
 *               - 3: Rain in both intervals
 */
int8_t fetchRainStatus() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  uint8_t wifiFlag = 0;
  // Wait for WiFi connection and timeout after 25 seconds
  for (size_t i = 0; i < 50; i++) { // Timeout after 25 seconds
    wifiFlag = WiFi.status();
    if (wifiFlag == WL_CONNECTED)
      break;

    Serial.print(".");
    delay(500);
  }
  if (wifiFlag = 0)
    Serial.println("WiFi for Rain Data: Timeout");
  else
    Serial.println("WiFi for Rain Data: OK");

  HTTPClient http;
  http.begin(forecastWeatherURL);

  int httpCode = http.GET();
  if (httpCode <= 0) {
    http.end();
    return 0;
  }

  String JSON_Data = http.getString();

  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, JSON_Data);
  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
    http.end();
    return 0;
  }

  JsonArray forecastArray = doc["list"];
  int count = 0;
  bool rainFirstInterval = false;
  bool rainSecondInterval = false;

  // Check rain status for the first two forecast intervals
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

  // Determine overall rain status based on intervals
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
  WiFi.disconnect(1);
  WiFi.mode(WIFI_OFF);
  return rainStatus;
}

/**
 * @brief Collects real-time weather data from sensors.
 * 
 * Reads temperature, humidity, and wind speed from respective sensors.
 */
void collectRealWeather() {
  temp = dht11.readTemperature();
  humidity = dht11.readHumidity();
  windSpeed = averageWindSpeed();
  lightStatus = digitalRead(lightPin);
}

/**
 * @brief Calculates the average wind speed over a sampling period.
 * 
 * Samples the wind speed sensor at regular intervals and computes the average.
 * 
 * @return int Average wind speed.
 */
int averageWindSpeed() {
  int sum = 0;
  int samplingInterval = 100; /**< Sampling interval in milliseconds */
  int loopLength = 5000 / samplingInterval; /**< Number of samples */

  for (int i = 0; i < loopLength; i++) {
    int voltage = analogRead(windPin);
    sum += voltage;
    delay(samplingInterval);
  }

  windSpeed = sum / loopLength;
  /** Values calibrated from Arduino changed from 10 to 12 bit, equations updated accordingly*/
  if (windSpeed  > 28)
    windSpeed = 0.0576 * windSpeed + 3.3923; /**< Convert voltage to wind speed */
  else
    windSpeed *= 0.1714;

  return windSpeed;
}

/**
 * @brief Fills the packet structure with current weather data.
 * 
 * @param pck Pointer to the packet structure to be filled.
 */
void fillPacket(packet *pck) {
  pck->humidity = humidity;
  pck->temperature = temp;
  pck->wind_speed = windSpeed;
  pck->light = lightStatus;
  pck->rain = rainStatus;
}

/**
 * @brief Sends the weather data packet via ESP-NOW.
 * 
 * Initializes ESP-NOW, registers the send callback, adds the peer, and sends the packet.
 * 
 * @param pck The packet structure containing weather data to be sent.
 */
void sendPacket(packet pck) {
  WiFi.mode(WIFI_STA);

  esp_now_deinit();
  // Initialize ESP-NOW
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(500);
  }

  esp_now_register_send_cb(OnDataSent);

  // Configure peer information
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  while (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    delay(500);
  }

  // Send packet
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&pck, sizeof(pck));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

/**
 * @brief Main loop function.
 * 
 * Periodically collects weather data, fetches rain status, fills the packet, and sends it.
 */
void loop() {
  delay(3000); /**< Wait before starting data collection */

  collectRealWeather();
  rainStatus = fetchRainStatus();

  packet dataTransmit;
  fillPacket(&dataTransmit);

  // Print collected data to Serial Monitor
  Serial.print(dataTransmit.humidity);
  Serial.print(", ");
  Serial.print(dataTransmit.temperature);
  Serial.print(", ");
  Serial.print(dataTransmit.wind_speed);
  Serial.print(", ");
  Serial.print(dataTransmit.rain);
  Serial.print(", ");
  Serial.println(dataTransmit.light);
  Serial.println("Wait...");

  delay(2000); /**< Wait before sending data */

  sendPacket(dataTransmit);
  Serial.println("-------------");

  delay(3000); /**< Wait before next loop iteration */
}