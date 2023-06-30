#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --------------------------------------------------
const int trigPin = 14; 
const int echoPin = 12; 
const int relayInput = 2; // the input to the relay pin
const int red = 10 ;// 1
const int orange = 15 ;// 2
const int blue = 13 ;// 3
int high  = 5;
int low = 7;


long duration;
int distance;
// ----------------------------------------------------

const char* ssid = "Jaya3_miA2lite";
const char* password = "abcd1234";
const char* mqtt_server = "91.121.93.94";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value;

void changeHigh(int higherLevel){
  high = higherLevel;

}

void changeLow(int lowerLevel){
  low = lowerLevel;

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println((char)payload[0]);
    
  if (strcmp(topic, "tank/level/setlower")){
      low = high;
      high = (char)payload[0] - 48;

      Serial.print("high level changed to " );
      Serial.println(high);

  }else if (strcmp(topic, "tank/level/setupper")){
    low = (char)payload[0] - 48;
    // changeLow((int)payload[0]);

      Serial.print("low level changed to " );
      Serial.println(low);

  }else{
    
  }


}

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
      // Once connected, publish an announcement...
      // client.publish("tank/#", "Mqtt connected");
      // ... and resubscribe
      client.subscribe("tank/motor");
      client.subscribe("tank/level/setupper");
      client.subscribe("tank/level/setlower");


    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(red, OUTPUT);
  pinMode(orange, OUTPUT);
  pinMode(blue, OUTPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(relayInput, OUTPUT); // initialize pin as OUTPUT
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  // -----------------------------------------------------------
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
  // ---------------------------------------------------------------

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    // ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "Distance is :%ld", distance);
    Serial.print("Published distance: ");
    Serial.println(distance);
    client.publish("tank/level/get", msg);
  }

  if(distance >= 3 && distance < 5){
  //full
  digitalWrite(blue, HIGH); 
  digitalWrite(red, LOW); 
  digitalWrite(orange, LOW); 
}else if(distance >=5 && distance < 6){
  //mid
  digitalWrite(orange, HIGH); 
    digitalWrite(blue, LOW); 
  digitalWrite(red, LOW); 
}else if(distance >=6){
  //low
  digitalWrite(red, HIGH); 
    digitalWrite(blue, LOW); 
  digitalWrite(orange, LOW); 
}

  Serial.print("Upper level = ");
  Serial.println(high);
  Serial.print("Lower level = ");
  Serial.println(low);

  // -----------------------------------------------------------------
  if (distance <= high) {
    digitalWrite(relayInput, HIGH); // turn relay off
    Serial.print("Motor turn OFF");
    //changeHigh(5);
    changeLow(7);


  } else if (distance >= low) {
    digitalWrite(relayInput, LOW); // turn relay on
    Serial.print("Motor turn ON      ");

  } else{


  }
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(1000);
  //---------------------------------------------------------------
}
