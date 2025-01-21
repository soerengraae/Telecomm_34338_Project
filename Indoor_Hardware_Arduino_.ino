const int relayPin = 7;    // Pin connected to the relay IN pin
const int motionPin = 8;   // Pin connected to the PIR sensor OUT pin
float temperature = 0.0;   // Variable to hold the temperature value
unsigned long motorTimer = 0; // Timer to track motor on duration
const unsigned long motorDelay = 5000; // Minimum motor ON time in milliseconds

bool motionDetected = false;
bool motorOn = false;

void setup() {
  pinMode(relayPin, OUTPUT);    // Set relay pin as OUTPUT
  pinMode(motionPin, INPUT);    // Set motion pin as INPUT
  digitalWrite(relayPin, LOW);  // Ensure the relay is off initially

  Serial.begin(9600);           // Start the Serial communication
  Serial.println("Enter the temperature value:");
}

void loop() {
  // Check if there is data available in the Serial Monitor
  if (Serial.available() > 0) {
    // Read the input as a string and convert to a float
    String input = Serial.readStringUntil('\n');
    temperature = input.toFloat();
    Serial.print("Temperature set to: ");
    Serial.println(temperature);
  }

  // Check if motion is detected
  motionDetected = digitalRead(motionPin);

  // Check if conditions are met to turn on the motor
  if (temperature > 20 && motionDetected) {
    motorOn = true;                      // Set motor on flag
    motorTimer = millis();               // Reset the timer
    Serial.println("Motion detected. Motor ON.");
  }

  // Turn off the motor if the timer expires
  if (motorOn && (millis() - motorTimer > motorDelay)) {
    motorOn = false;                     // Reset motor on flag
    digitalWrite(relayPin, LOW);         // Turn off the relay
    Serial.println("Motor OFF. Timer expired.");
  }

  // If the motor is ON, keep the relay ON
  if (motorOn) {
    digitalWrite(relayPin, HIGH);
  }
}
