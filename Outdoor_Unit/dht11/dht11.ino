/**
 * @file dht11_draft.ino
 * @brief First draft for reading temperature and humidity from the DHT11 sensor using the DHT library.
 *
 * This program reads temperature and humidity values from a DHT11 sensor and outputs them
 * via the serial monitor. The readings occur every 5 seconds in the main loop.
 */

#include <DHT.h>

/** 
 * @brief Pin number where the DHT11 data line is connected.
 */
#define dht11Pin 4

/**
 * @brief Instantiate a DHT object on the specified pin, using DHT11 sensor type.
 */
DHT dht11(dht11Pin, DHT11);

/**
 * @brief Arduino setup function.
 *
 * Initializes the serial communication at 9600 baud and
 * begins communication with the DHT11 sensor.
 */
void setup() {
  Serial.begin(9600);
  dht11.begin();
}

/**
 * @brief Main Arduino loop function.
 *
 * Reads humidity and temperature from the DHT11 sensor, then prints those values
 * to the Serial Monitor. A 5-second delay is used between readings.
 */
void loop() {
  float humidity  = dht11.readHumidity();      /**< @brief Stores the humidity value read from the DHT11 sensor. */
  float temp = dht11.readTemperature();        /**< @brief Stores the temperature value read from the DHT11 sensor. */

  Serial.println(temp);
  Serial.println(humidity);
  Serial.println(""); // Empty new line

  delay(5000);
}