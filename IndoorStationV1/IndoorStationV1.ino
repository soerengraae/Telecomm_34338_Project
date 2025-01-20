// --------------------- Receiver Node (ESP32-B) ---------------------

#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

#define FAN_PIN 5          // Fan connected to GPIO 5 (PWM-capable)
#define PIR_SENSOR_PIN 14  // PIR sensor connected to GPIO 14

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C address 0x27, 16 columns and 2 rows

// Wi-Fi credentials
const char* ssid = "iPhone (5)";
const char* password = "carl1234";

// ThingSpeak API Key
const char* thingspeakApiKey = "MZUH6YO66N4RRRP6";

// Structure for incoming data
typedef struct struct_message {
  float temperature;
  float humidity;
  float windSpeed;
  bool ackFlag;
} struct_message;

struct_message incomingData;

unsigned long previousMillis = 0;
unsigned long lastMotionTime = 0;
const long uploadInterval = 15000;
const long fanTimeout = 300000;  // 5 minutes

void onDataReceive(const uint8_t *mac, const uint8_t *incomingDataBuffer, int len) {
  memcpy(&incomingData, incomingDataBuffer, sizeof(incomingData));
  Serial.println("Data received.");
  displayDataOnLCD();
  controlFan();
  sendAck(mac);
}

void sendAck(const uint8_t *mac) {
  struct_message ackMessage = {};
  ackMessage.ackFlag = true;
  esp_now_send(mac, (uint8_t *)&ackMessage, sizeof(ackMessage));
  Serial.println("ACK sent back to Sender.");
}

void displayDataOnLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(incomingData.temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(incomingData.humidity);
  lcd.print("%");
}

void controlFan() {
  int motionDetected = digitalRead(PIR_SENSOR_PIN);
  if (incomingData.temperature >= 24 && motionDetected == HIGH) {
    lastMotionTime = millis();
    float fanSpeed = map(incomingData.temperature, 24, 40, 100, 255);
    fanSpeed = constrain(fanSpeed, 100, 255);
    ledcWrite(0, fanSpeed);
    Serial.print("Fan Speed (PWM): ");
    Serial.println(fanSpeed);
  }
  if (millis() - lastMotionTime > fanTimeout) {
    ledcWrite(0, 0);
    Serial.println("Fan OFF due to inactivity.");
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void initESPNow() {
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
  }
  esp_now_register_recv_cb(OnDataReceive);
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initESPNow();
  lcd.init();
  lcd.backlight();
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
  //ledcSetup(0, 5000, 8);
  //ledcAttachPin(FAN_PIN, 0);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= uploadInterval) {
    previousMillis = currentMillis;
  }
}
