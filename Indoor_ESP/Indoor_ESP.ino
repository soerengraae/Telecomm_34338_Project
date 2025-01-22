/**
 * @file Indoor_ESP.ino
 * @brief Handles Bluetooth communication between ESP32 devices, receiving JSON commands to control GPIO pins.
 *
 * This program sets up the ESP32 as a slave device, waiting for a master ESP32 device to connect via Bluetooth. 
 * Once connected, it receives JSON data to control specific GPIO pins and sends back acknowledgments.
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>

// Create a BluetoothSerial object to handle Bluetooth communication
BluetoothSerial SerialBT;

/**
 * @brief Setup function to initialize serial communication, Bluetooth, and onboard LED.
 *
 * This function sets up the serial communication for debugging, initializes Bluetooth communication with 
 * the name "ESP32_Slave", and sets up the onboard LED to indicate connection status.
 */
void setup()
{
  // Initialize serial communication at 115200 baud rate for debugging
  Serial.begin(115200);

  // Set the built-in LED pin as output
  pinMode(BUILTIN_LED, OUTPUT);

  // Start Bluetooth communication and set the device name to "ESP32_Slave"
  SerialBT.begin("ESP32_Slave");
  Serial.println("ESP32 Slave Bluetooth Serial Started. Waiting for Master to connect...");

  // Wait until the ESP32 Master device connects to this ESP32 Slave
  while (!SerialBT.connected())
  {
    delay(1000);  // Wait for 1 second before checking again
    Serial.println("Waiting for ESP32_Master to be connected...");
  }

  // Once connected, print a confirmation message
  Serial.println("Connected to ESP32_Master");

  // Turn on the built-in LED to indicate a successful connection
  digitalWrite(BUILTIN_LED, HIGH);
}
/**
 * @brief Main loop function to handle incoming JSON commands via Bluetooth.
 *
 * This function checks for incoming Bluetooth data, parses it as JSON, extracts GPIO pin and status information, 
 * and executes the specified command. It also sends an acknowledgment back to the master device.
 */
void loop()
{
  // Check if data is received from the master device via Bluetooth
  if (SerialBT.available())
  {
    // Read the incoming data as a string
    String incomingData = SerialBT.readString();
    Serial.print("Received JSON: ");
    Serial.println(incomingData);

    // Create a JSON document to hold the parsed data
    JsonDocument jsonDoc; // Adjust the size as needed

    // Attempt to deserialize the JSON data from the incoming string
    DeserializationError error = deserializeJson(jsonDoc, incomingData);

    // Check if deserialization was successful
    if (error)
    {
      Serial.print("JSON Deserialization failed: ");
      Serial.println(error.c_str());  // Print the error message
      return;  // Exit the loop if there's an error
    }

    // Extract the pin number and status from the JSON document
    int pin_number = jsonDoc["pin_number"];
    bool pin_status = jsonDoc["pin_status"];

    // Set the specified pin as output
    pinMode(pin_number, OUTPUT);

    // Write the received status to the specified pin (HIGH or LOW)
    digitalWrite(pin_number, pin_status);

    // Send a confirmation back to the master device
    SerialBT.println("Command executed successfully");
  }

  delay(50);  // Small delay to avoid overwhelming the loop
}