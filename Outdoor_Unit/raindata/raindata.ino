/**
 * @file raindata.ino
 * @brief Code to fetch rain forecast from OpenWeatherMap and upload the results to ThingSpeak.
 *
 * This program connects to WiFi, fetches a forecast from OpenWeatherMap, checks the first two
 * forecast intervals for rain, categorizes the status, and then uploads this info to ThingSpeak.
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ThingSpeak.h>
#include "secrets.h"         /**< @brief Contains SSID and PASS for WiFi (plus any other private credentials). */

/** 
 * @brief The OpenWeatherMap Forecast API URL. 
 * Replace "APIKEYHERE" with your actual OpenWeatherMap API key.
 */
#define forecastWeatherURL "http://api.openweathermap.org/data/2.5/forecast?lat=5.671401156455272&lon=101.41105533517317&units=metric&appid=APIKEYHERE"

/**
 * @brief WiFi client object used for both fetching data and communicating with ThingSpeak.
 */
WiFiClient client;

/**
 * @brief ThingSpeak Write API Key (fetched from secrets.h).
 */
const char* APIKey = ThingSpeakWriteKey;

/**
 * @brief ThingSpeak server endpoint.
 */
const char* server = "api.thingspeak.com";

/**
 * @brief The ThingSpeak channel ID where data is sent.
 */
unsigned long channelID = ThingSpeakChannelID;

/**
 * @brief Variable to store the rain status.
 *
 * Value can be:
 *   - 0: No rain in the first 6 hours.
 *   - 1: Rain in the first interval only.
 *   - 2: Rain in the second interval only.
 *   - 3: Rain in both intervals.
 */
int rainStatus = 0;

/**
 * @brief Arduino setup function.
 *
 * Initializes serial communication at 9600 baud, connects to WiFi,
 * and prints the IP address upon successful connection.
 */
void setup() {
  Serial.begin(9600);

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * @brief Main Arduino loop function.
 *
 * - If WiFi is connected, fetch the forecast from the OpenWeatherMap API.
 * - Parse the JSON to check for rain in the first two forecast intervals.
 * - Update the ThingSpeak channel with the determined rainStatus.
 * - Wait 10 minutes (180,000 ms) before fetching again.
 */
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(forecastWeatherURL);

    int httpCode = http.GET();
    if (httpCode > 0) {
      String JSON_Data = http.getString();

      // JSON deserialization
      DynamicJsonDocument doc(16384);
      DeserializationError error = deserializeJson(doc, JSON_Data);
      if (error) {
        Serial.print("JSON deserialization failed: ");
        Serial.println(error.c_str());
        http.end();
        return;
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
      }

      // Print the rain status
      Serial.println(rainStatus);

    } else {
      Serial.println("HTTP request failed!");
    }

    http.end();
  }

  // Initialize and connect to ThingSpeak
  ThingSpeak.begin(client);
  client.connect(server, 80);

  // Send the rain status to ThingSpeak
  ThingSpeak.setField(6, rainStatus);
  Serial.println("Writing to ThingSpeak");
  ThingSpeak.writeFields(channelID, APIKey);

  // Wait 10 minutes before refreshing the data again
  delay(180000);
}