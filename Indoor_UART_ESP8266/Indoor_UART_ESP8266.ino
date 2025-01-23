#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

/*!
  @brief The pin that should have been used for driving the fan.

  We wanted to drive a fan when the temperature got to high, to try to cool of the room, where the indoor system is.
  We had problems with giving enough power to the fan with the esp, without getting noice, so for prrof of concept 
  an LED lights up, when the temperature is above the threshold.
*/
#define MOTOR_PIN D0
#define TEMPERATURE_THRESHOLD 20

WiFiClient client;


//! Every 10 seconds switch data on lcd and read data if possible.
const int readDataDelay = 4000;


LiquidCrystal_I2C lcd(0x27, 16, 2);
/*!
  Sets up wifi, the serialport and initializes the LCD.
*/
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);



  pinMode(MOTOR_PIN, OUTPUT);    // Set relay pin as OUTPUT
  digitalWrite(MOTOR_PIN, LOW);  // Ensure the relay is off initially

  lcd.init();
  lcd.clear();
  lcd.backlight();
}


int humidity = 0;
int temperature = 20;
int wind_speed = 0;
int rain_level = 0;
int sun_up = 0;
int lcd_controller = 0;
/*!
  @brief Loop function for displaying, fetching and uploading data.

  The loop function fetches data through the UART from a ESP 32,
  Uploads the data to ThingSpeak and displays it on the LCD.
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
    Serial.print("Sun-up: ");
    Serial.println(sun_up);
    Serial.print("rain_level: ");
    Serial.println(rain_level);

    write_ThingSpeak();
    
    if(temperature > TEMPERATURE_THRESHOLD){
      digitalWrite(MOTOR_PIN, HIGH);
    }else{
      digitalWrite(MOTOR_PIN, LOW);
    }
  }
  write_lcd();
  delay(readDataDelay);

  
}


/*!
  @brief Function for lcd

  This function has 4 states, that show the data from the weateher station:
  1: temperature and humidity.
  2: wind and light outside
  3: Rain in 0-3 hrs
  4. Rain in 4-6 hrs
*/
void write_ThingSpeak(){
  ThingSpeak.begin(client);
  client.connect(server, 80);
 
  
  ThingSpeak.setField(6, rain_level);
  ThingSpeak.setField(2, temperature);
  ThingSpeak.setField(3, humidity);
  ThingSpeak.setField(4, wind_speed);

  ThingSpeak.writeFields(channelID, APIWriteKey);

}

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
      lcd.setCursor(0,1);
      lcd.print("Sun up:");
      lcd.setCursor(8, 1);
      if(sun_up == 1){
        lcd.print("No");
      }else{
        lcd.print("Yes");
      }
      
      break;
      case 2:
        lcd.clear();
        // Rain LCD
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
        // Rain LCD
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

