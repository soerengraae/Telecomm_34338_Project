/**
 * @file Indoor_Receiver_ESPNow.ino
 * @brief ESP-NOW receiver for receiving and processing sensor data.
 *
 * This program sets up an ESP32 device as an ESP-NOW receiver to receive sensor data from another ESP32 device. 
 * The data includes humidity, temperature, and wind speed, which are extracted and printed to the Serial Monitor.
 */  

#include <esp_now.h>
#include <WiFi.h>

/**
 * @struct struct_message
 * @brief Structure to hold sensor data received via ESP-NOW.
 *
 * This structure defines the format of the data sent by the sender and must match the sender's structure.
 */
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int humidity;
  int temperature;
  int wind_speed;
} struct_message;

// Create a struct_message called myData
struct_message myData;

/**
* @brief Callback function that executes when data is received via ESP-NOW.
*
* This function is triggered when data is received from a sender device. 
* It extracts the data, updates the `myData` structure, and prints the values to the Serial Monitor.
*
* @param mac Pointer to the MAC address of the sender device.
* @param incomingData Pointer to the data received.
* @param len Length of the received data.
*/
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  //Serial.print(",");
  Serial.print(myData.humidity);
  Serial.print(",");
  Serial.print(myData.temperature);
  Serial.print(",");
  Serial.println(myData.wind_speed);
  

  delay(1000);
}

/**
* @brief Setup function to initialize the ESP-NOW receiver.
*
* This function sets up the ESP32 device as a Wi-Fi station, initializes ESP-NOW, 
* and registers the callback function for receiving data.
*/
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {

}