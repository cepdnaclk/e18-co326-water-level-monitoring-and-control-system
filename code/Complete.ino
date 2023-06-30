const int trigPin = 14; 
const int echoPin = 12; 
const int relayInput = 2; // the input to the relay pin
const int fullIndicator = 6;
const int midIndicator = 5;
const int lowIndicator = 4;

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

//
if(distance >= 3 && distance < 5){
  //full
  digitalWrite(fullIndicator, HIGH); 
}else if(distance >=5 && distance < 6){
  //mid
  digitalWrite(midIndicator, HIGH); 
}else if(distance >=6 && distance < 7){
  //low
  digitalWrite(lowIndicator, HIGH); 
}

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
