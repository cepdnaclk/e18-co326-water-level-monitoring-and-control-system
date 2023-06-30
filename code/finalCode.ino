#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --------------------set global variables-------------------

// relay pins
const int trigPin = 14; 
const int echoPin = 12; 
const int relayInput = 2; 

// 3 leds
const int red = 10 ;
const int orange = 15 ;
const int blue = 13 ;

// water levels where motor starts and off
int high  = 5;
int low = 11;

// topics


// wifi and mqtt credentials

const char* ssid = "Jaya3_miA2lite";
const char* password = "abcd1234";
const char* mqtt_server = "91.121.93.94";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];


//---------------------------------SUB FUNCTIONS-------------------

// reset high and low water boundaries where the motor should start and off
void resetHigh(int higherLevel){
  high = higherLevel;

}

void resetLow(int lowerLevel){
  low = lowerLevel;

}

// set up wifi 
void setup_wifi() {

  delay(10);
  // start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// callback function when data is published by another publlisher
// to a topic i have subscribed
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println((char)payload[0]);
    
  if (strcmp(topic, "UoP/CO/326/E18/group8/Motor/setlower")){
      low = high;
      high = (char)payload[0] - 48;
      Serial.print("higher level changed to " );
      Serial.println(high);

  }else if (strcmp(topic, "UoP/CO/326/E18/group8/Motor/setupper")){
    low = (char)payload[0] - 48;
      Serial.print("lower level changed to " );
      Serial.println(low);

  }else{
    
  }

}

// reconnect to mqtt broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");

      // Once connected resubscribe
      client.subscribe("UoP/CO/326/E18/group8/Motor/setupper");
      client.subscribe("UoP/CO/326/E18/group8/Motor/setlower");


    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// ---------------------SETUP------------------------------------
void setup() {
  pinMode(red, OUTPUT); // Sets leds as Output
  pinMode(orange, OUTPUT);
  pinMode(blue, OUTPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(relayInput, OUTPUT); // initialize pin as OUTPUT

  Serial.begin(9600); // set speed of communication

  setup_wifi(); // connect to wifi
  client.setServer(mqtt_server, 1883); // connect to mqtt server
  client.setCallback(callback); // set the callback function
}

//-------------------SENSOR FUNCTION-----------------------
int UoP_CO_326_E18_group8_Ultrasonic(){

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  return (duration*0.034/2);

}

//-------------------ACTUATOR FUNCTION-----------------------
void UoP_CO_326_E18_group8_Motor(int sensorData){

  // actuator works acording to water level
  if (distance <= high) {
    digitalWrite(relayInput, HIGH); // turn relay off
    Serial.println("Motor turn OFF");
    resetLow(11);


  } else if (distance >= low) {
    digitalWrite(relayInput, LOW); // turn relay on
    Serial.println("Motor turn ON");

  } else{


  }

}

//-------------------LED FUNCTION-----------------------
void UoP_CO_326_E18_group8_LEDs(int distance){
  if(distance >= 0 && distance < 6){
  //full
  digitalWrite(blue, HIGH); 
  digitalWrite(red, LOW); 
  digitalWrite(orange, LOW); 
}else if(distance >=6 && distance < 11){
  //mid
  digitalWrite(orange, HIGH); 
    digitalWrite(blue, LOW); 
  digitalWrite(red, LOW); 
}else if(distance >=11){
  //low
  digitalWrite(red, HIGH); 
    digitalWrite(blue, LOW); 
  digitalWrite(orange, LOW); 
}
  

}

//-------------------PUBLISH DATA TO MQTT FUNCTION-----------------------
void UoP_CO_326_E18_group8_Publish(int sensorData){

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    snprintf (msg, MSG_BUFFER_SIZE, "Distance is :%ld", sensorData);
    Serial.print("Published distance: ");
    Serial.println(sensorData);
    client.publish("UoP/CO/326/E18/group8/Ultrasonic", msg);
  }

}


// -------------------------LOOP-------------------------------
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int distance = UoP_CO_326_E18_group8_Ultrasonic();

  UoP_CO_326_E18_group8_Publish(distance);

  UoP_CO_326_E18_group8_LEDs(distance);

  UoP_CO_326_E18_group8_Motor(distance);

  // current upper and lower boundary levels
  Serial.print("Upper level = ");
  Serial.println(high);
  Serial.print("Lower level = ");
  Serial.println(low);
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(1000);

}
