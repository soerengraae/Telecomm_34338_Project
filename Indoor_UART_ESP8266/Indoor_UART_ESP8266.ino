/**
 * @file Indoor_UART_ESP8266.ino
 * @brief Code for receiving weather data from outdoor for monitoring and displaying on the indoor unit.
 *
 * This program reads transmitted weather data (humidity, temperature, wind speed, and rain level) via serial input,
 * sends the data to ThingSpeak, and displays it on an LCD. It also controls a motor based on temperature.
 */
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

#define MOTOR_PIN D0
WiFiClient client;

//! Every 10 seconds switch data on lcd and read data if possible.
const int readDataDelay = 10 * 1000;


LiquidCrystal_I2C lcd(0x27, 16, 2);

/**
 * @brief Initializes the weather station components.
 *
 * This function sets up the Serial communication, Wi-Fi connection, motor pin, and LCD display.
 */
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  lcd.init();
  lcd.clear();
  lcd.backlight();
}

//! Humidity is initialized to 0. It is between 0% and 100%.
int humidity = 0;
int temperature = 20;
int wind_speed = 0;
int rain_level = 0;
int sun_up = 0;
int lcd_controller = 0;

/**
 * @brief Main loop for processing weather data.
 *
 * This function reads weather data via Serial, updates ThingSpeak, controls the motor, 
 * and updates the LCD display based on the received data.
 */
void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Read incoming data
    // Find positions of commas
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);
    int thirdComma = data.indexOf(',', secondComma + 1);
    int fourthComma = data.indexOf(',', thirdComma + 1);
    // Split data into four parts
    humidity = data.substring(0, firstComma).toInt();
    temperature = data.substring(firstComma + 1, secondComma).toInt();
    wind_speed = data.substring(secondComma + 1, thirdComma).toInt();
    rain_level = data.substring(thirdComma + 1, fourthComma).toInt();
    sun_up = data.substring(fourthComma + 1).toInt();

    // Print received variables
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Wind Speed: ");
    Serial.println(wind_speed);

    write_ThingSpeak();
    
    if(temperature > 10){
      digitalWrite(MOTOR_PIN, HIGH);
    }else{
      digitalWrite(MOTOR_PIN, LOW);
    }
  }
  write_lcd();
  delay(readDataDelay);

}

/**
 * @brief Sends weather data to ThingSpeak.
 *
 * This function initializes ThingSpeak communication and updates its fields with 
 * humidity, temperature, wind speed, and rain level.
 */
void write_ThingSpeak(){
  ThingSpeak.begin(client);
  client.connect(server, 80);
  //Also read
 
  Serial.print("rain_level: ");
  Serial.println(rain_level);
  ThingSpeak.setField(6, rain_level);
  ThingSpeak.setField(2, temperature);
  ThingSpeak.setField(3, humidity);
  ThingSpeak.setField(4, wind_speed);

  ThingSpeak.writeFields(channelID, APIWriteKey);

}

/**
 * @brief Updates the LCD display with weather data.
 *
 * This function switches between different screens on the LCD to display 
 * temperature, humidity, wind speed, and rain level predictions.
 */
void write_lcd(){
  lcd_controller++;
    switch(lcd_controller % 4){
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
      // Humidity on LCD
      lcd.setCursor(0,1);
      lcd.print("Humidity:");
      lcd.setCursor(9, 1);
      lcd.print(humidity);
      lcd.setCursor(12, 1);
      lcd.print("%");
      break;
      case 1:
      lcd.clear();
      // wind on LCD
      lcd.setCursor(0,0);
      lcd.print("Wind:");
      lcd.setCursor(5, 0);
      lcd.print(wind_speed);
      lcd.setCursor(7, 0);
      lcd.print("m/s");
      break;
      case 2:
        lcd.clear();
        // Temperature on LCD
        lcd.setCursor(0,0);
        lcd.print("Rain in 0-3 hrs:");
        lcd.setCursor(0, 1);
        if(rain_level == 1 || rain_level == 3){
          lcd.print("Yes");
        }else{
          lcd.print("No");
        }
      break;
      case 3:
        lcd.clear();
        // Temperature on LCD
        lcd.setCursor(0,0);
        lcd.print("Rain in 4-6 hrs:");
        lcd.setCursor(0, 1);
        if(rain_level == 2 || rain_level == 3){
          lcd.print("Yes");
        }else{
          lcd.print("No");
        }
      break;
  }
}

