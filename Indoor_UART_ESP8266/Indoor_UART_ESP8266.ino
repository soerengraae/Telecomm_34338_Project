void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Read incoming data
    // Find positions of commas
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);

    // Split data into three parts
    int humidity = data.substring(0, firstComma).toInt();
    int temperature = data.substring(firstComma + 1, secondComma).toInt();
    int wind_speed = data.substring(secondComma + 1).toInt();

    // Print received variables
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Wind Speed: ");
    Serial.println(wind_speed);
  }
}
