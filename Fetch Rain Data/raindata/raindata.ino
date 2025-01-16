#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include <ThingSpeak.h>

// Replace with the OpenWeatherMap Forecast API URL
#define forecastWeatherURL "http://api.openweathermap.org/data/2.5/forecast?lat=5.671401156455272&lon=101.41105533517317&units=metric&appid=APIKEYHERE"

void setup() {
  Serial.begin(9600);

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

WiFiClient client;

const char* APIKey = "APIWRITEKEY";
const char* server = "api.thingspeak.com";
unsigned long channelID = 0;

int rainStatus = 0;


void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(forecastWeatherURL);

    // Check HTTP response code
    int httpCode = http.GET();
    if (httpCode > 0) {
      String JSON_Data = http.getString();

      // Allocate memory for the JSON document
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

      for (JsonObject forecast : forecastArray) {
        if (count >= 2) break; // Process only the first 2 entries (6 hours)

        const char* weather = forecast["weather"][0]["main"];
        bool isRaining = (!strcmp(weather, "Rain") || !strcmp(weather, "Drizzle"));

        // Check which interval the rain falls into
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

      // Output the rain status
      Serial.println(rainStatus);

    } else {
      Serial.println("HTTP request failed!");
    }

    http.end();
  }

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Connect to ThingSpeak server
  client.connect(server, 80);

  // Set fields and write them
  ThingSpeak.setField(6, rainStatus);
  Serial.println("Writing to ThingSpeak");
  ThingSpeak.writeFields(channelID, APIKey);

  // Wait for 10 minutes before refreshing the data
  delay(180000);
}