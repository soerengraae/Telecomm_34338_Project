#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const char* ssid = "iPhone (5)";
const char* pass = "carl1234";
WiFiClient client;
unsigned long channelID = 2810374;  //your TS channal
const char* APIKey = "MZUH6YO66N4RRRP6";   //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 2 * 1000;  //post data every 20

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
}

int humidity;
int temperature;
int wind_speed;

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Read incoming data
    // Find positions of commas
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);

    // Split data into three parts
    humidity = data.substring(0, firstComma).toInt();
    temperature = data.substring(firstComma + 1, secondComma).toInt();
    wind_speed = data.substring(secondComma + 1).toInt();

    // Print received variables
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Wind Speed: ");
    Serial.println(wind_speed);

    write_data();

  }
}

void write_data(){
  ThingSpeak.begin(client);
  client.connect(server, 80);

  ThingSpeak.setField(2, temperature);
  ThingSpeak.setField(3, humidity);
  ThingSpeak.setField(4, wind_speed);

  ThingSpeak.writeFields(channelID, APIKey);
}

