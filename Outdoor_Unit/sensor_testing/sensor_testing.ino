/**
 * @file sensor_test.ino
 * @brief Minimal program to read temperature/humidity from a DHT11 sensor and
 *        measure wind speed from an analog pin (e.g., a DC motor or wind sensor).
 */

#include "DHT.h"

/** 
 * @brief Pin to which the DHT11 sensor data line is connected.
 */
#define DHT11Pin 5

/// Create a DHT instance with DHT11 sensor type
DHT dht11(DHT11Pin, DHT11);

/**
 * @brief Pin used for reading wind speed (analog).
 */
#define windPin 35

/**
 * @brief Global variables for storing sensor readings
 */
uint8_t temperature = 0;  // °C
uint8_t humidity    = 0;  // %
float   windSpeed  = 0.0; // Could store m/s

/**
 * @brief Setup function: Initialize Serial monitor and the DHT sensor.
 */
void setup() {
  Serial.begin(9600);
  dht11.begin();
}

/**
 * @brief Reads temperature, humidity, and wind speed.
 *
 * - Temperature from DHT11
 * - Humidity from DHT11
 * - windSpeed from an analog read (plus optional conversion)
 */
void collectSensorData() {
  // Read from DHT
  temperature = dht11.readTemperature();
  humidity    = dht11.readHumidity();

  // Optionally read 5-second average for wind speed
  windSpeed = averageWindSpeed();
}

/**
 * @brief Simple function to get a 5-second average from the wind pin.
 *
 * @return float The average wind speed (raw or converted).
 */
float averageWindSpeed() {
  long sum = 0;
  int samplingInterval = 100;
  int loopLength      = 5000 / samplingInterval; // 5 seconds total

  for (int i = 0; i < loopLength; i++) {
    sum += analogRead(windPin);
    delay(samplingInterval);
  }

  float averageRaw = sum / (float)loopLength;

  // Example "fudge factor" from the original code
  float converted = 0.2306 * 0.25 * averageRaw;// + 3.3923; 
  return converted;
}

/**
 * @brief Main loop: Collect sensor data and print it every few seconds.
 */
void loop() {
  collectSensorData();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Wind Speed (approx): ");
  Serial.print(windSpeed);
  Serial.println(" m/s (estimated)");

  Serial.println("----------------------");

  // Delay for clarity (e.g., 2 seconds)


  delay(2000);
}