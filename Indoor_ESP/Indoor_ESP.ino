#ifdef ESP32
  #include <WiFi.h>
  #include <esp_wifi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <ThingSpeak.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include "my_bluetooth.h"


LiquidCrystal_I2C lcd(0x27, 16, 2);

#define FIELD_RAIN 1
#define FIELD_TEMP 2
#define FIELD_HUMID 3
#define FIELD_WIND 4

typedef struct struct_message {
    char a[32];
    int b;
    float c;
    bool d;
} struct_message;

struct_message myData;


const char* ssid = "iPhone (5)";
const char* pass = "carl1234";
WiFiClient client;
unsigned long channelID = 2810374;  //your TS channal
const char* APIKey = "MZUH6YO66N4RRRP6";   //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 2 * 1000;  //post data every 20

int temperature;
int rain_level;
int humidity;
int wind_speed;
int counter = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

 esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // initialize the LCD
	lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  // Print a message on both lines of the LCD.
  
  read_Thingspeak();
}



float data;  //measured data


void loop() {
  display_data_on_lcd(counter);
  delay(postDelay);
  if(counter == 100){
  read_Thingspeak();
  counter = 0;
  }
  counter++;

  
}


void read_Thingspeak(){
data = 42.0;
  ThingSpeak.begin(client);
  client.connect(server, 80);    //connect(URL, Port)
  temperature = ThingSpeak.readIntField(channelID, FIELD_TEMP);
  humidity = ThingSpeak.readIntField(channelID, FIELD_HUMID);
  rain_level = ThingSpeak.readIntField(channelID, FIELD_RAIN);
  wind_speed = ThingSpeak.readIntField(channelID, FIELD_WIND);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("rain level: ");
  Serial.println(rain_level);
  Serial.print("windspeed: ");
  Serial.println(wind_speed);
  Serial.println();

  /*ThingSpeak.setField(1, data);
  ThingSpeak.setField(2, data);
  ThingSpeak.setField(3, data);
  ThingSpeak.setField(4, data);
  ThingSpeak.setField(5, data);
  ThingSpeak.setField(6, data); */ //set data on the X
  ThingSpeak.writeFields(channelID, APIKey);  //post everything to TS
  client.stop();
}

void display_data_on_lcd(int input){
    switch(input % 2){
    case 0:
      lcd.clear();
      // Temperature on LCD
      lcd.setCursor(0,0);
      lcd.print("Temperature:");
      lcd.setCursor(12, 0);
      lcd.print(temperature);
      lcd.setCursor(14, 0);
      lcd.write(223);
      lcd.setCursor(15, 0);
      lcd.print("C");
      // Temperature on LCD
      lcd.setCursor(0,1);
      lcd.print("Humidity:");
      lcd.setCursor(9, 1);
      lcd.print(humidity);
      lcd.setCursor(12, 1);
      lcd.print("%");
      break;
      case 1:
      lcd.clear();
      // Temperature on LCD
      lcd.setCursor(0,0);
      lcd.print("Rain:");
      lcd.setCursor(5, 0);
      lcd.print(rain_level);
      lcd.setCursor(7, 0);
      lcd.print("mm");
      // Temperature on LCD
      lcd.setCursor(0,1);
      lcd.print("Wind::");
      lcd.setCursor(5, 1);
      lcd.print(wind_speed);
      lcd.setCursor(7, 1);
      lcd.print("m/s");
      break;
  }
}


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
  Serial.print("Bool: ");
  Serial.println(myData.d);
  Serial.println();
}
