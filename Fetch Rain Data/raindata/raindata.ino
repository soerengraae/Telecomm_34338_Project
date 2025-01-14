#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

// Denmark: 55.729686000861264, 12.405854498013536
// Lithuania: 55.97884006934981, 23.49979213930799
#define weatherURL "http://api.openweathermap.org/data/2.5/weather?lat=55.97884006934981&lon=23.49979213930799&units=metric&appid=IDHERE"

uint8_t rain = 0;

void setup() {
  // put your setup code here, to run once:
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

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(weatherURL);

    // get httpCode and check for error; code will be negative on error
    if (http.GET() > 0) {
      // Store data as a JSON string
      String JSON_Data = http.getString();

      // Uncomment for debugging purposes
      //Serial.println(JSON_Data);

      // Allocate 2KB of stack memory to the document
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);  // Get data
      JsonObject obj = doc.as<JsonObject>();

      // Poll for rain or drizzle (very light rain, but still)
      // Should prolly make this prettier
      rain = (!strcmp(obj["weather"][0]["main"].as<const char*>(), "Rain") || !strcmp(obj["weather"][0]["main"].as<const char*>(), "Drizzle")) ? 1 : 0;
      
      Serial.print("Rain: ");
      if (rain)
        Serial.println("Beep boop it's raining");
      else
        Serial.println("Beep boop no");

    } else
      Serial.println("Error!");

    http.end();
  }
  
  //Wait for 10 seconds
  delay(10000);
}
