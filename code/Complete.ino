const int trigPin = D5; 
const int echoPin = D6; 
const int relayInput = D4; // the input to the relay pin


long duration;
int distance;

void setup() {
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode(relayInput, OUTPUT); // initialize pin as OUTPUT
Serial.begin(9600); // Starts the serial communication
}
void loop() {

// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

if (distance <= 3) {
  digitalWrite(relayInput, HIGH); // turn relay off
  Serial.print("Motor turn OFF");
} else if (distance >= 7) {
  digitalWrite(relayInput, LOW); // turn relay on
  Serial.print("Motor turn ON      ");
}
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
delay(1000);
}
