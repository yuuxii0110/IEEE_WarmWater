#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <iostream>
#include <ESP32PWM.h>

// Replace with your WiFi credentials
const char* ssid = "ASUS_38_2G";
const char* password = "0123640216";

// MQTT broker details
const char* mqtt_server = "192.168.1.244";
const int mqtt_port = 1883;
const char* mqtt_user = "WarmWater";
const char* mqtt_password = "Warm1234";

String userid="";
String deviceid="6969";
bool flag=0;
bool prevflag=0;
bool moving=0;

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor Pins
const int motorPin = 33;
int buzzerPin = 25; // specify the pin that the buzzer is connected to
int channel = 0; // specify the LEDC channel to use
int frequency = 1000; // specify the frequency of the tone to generate
int LED = 26;
int min_bright=100;
unsigned long prev=0;

unsigned long startT,workoutTime;
unsigned long Energy=0;
unsigned long refresh=0;


void setup() {
  pinMode(motorPin,INPUT);
  pinMode(LED,OUTPUT);
  
    // configure the LEDC module
  ledcSetup(channel, 5000, 8); // set the PWM frequency to 5 kHz and the resolution to 8 bits
  ledcAttachPin(buzzerPin, channel); // attach the buzzer pin to the LEDC channel
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");

  client.setServer(mqtt_server, mqtt_port);
//  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password )) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed to connect to MQTT broker with error code: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  
  // Subscribe to a topic 
  client.subscribe("test_topic");
  ledcWriteTone(channel, frequency); // generate a square wave of the specified frequency on the buzzer pin
  delay(1000); // wait for 1 second
  ledcWrite(channel, 0); // stop generating the square wave
    // Wait for incoming messages from MQTT broker
  client.loop();
}

void loop() {
  // Send a message to the MQTT broker
  
  delay(200); 
  int value = analogRead(motorPin);
  unsigned long now=millis();
  if (value>0){ 
    Energy+=value;
    Serial.println(Energy);
    int myInt = value; // example integer
    char myString[4]; // buffer for converted string
    sprintf(myString, "%d", myInt); // convert int to const char* string
    client.publish("test_topic",myString);
    moving=1;      
    flag=1;
  }
  else {
    if (moving){//every stop
      prev=millis();
      moving=0;

    }
    if ((now-prev)>5000) {//stop for more than 5 sec
      flag=0;
      } 
  }
  if((now-refresh)>5000){// send data every 5sec
      workoutTime=millis()-startT;
      char myTime[8]; // buffer for converted string
      sprintf(myTime, "%d", workoutTime); // convert int to const char* string
      char myEnergy[8]; // buffer for converted string
      sprintf(myEnergy, "%d", Energy); // convert int to const char* string
      String msg=userid + "/" + myEnergy + "/" +myTime;
      client.publish("energy_and_time",msg.c_str());
      refresh=now;
      Serial.println(msg);
    }
  if(flag!=prevflag){//start or end
    if (flag==1){//start
      digitalWrite(LED,HIGH);
      startT=millis();
      String msg=deviceid + "/" + userid;
      client.publish("connection_established",msg.c_str());
    }
    else if (flag==0){//end
      digitalWrite(LED,LOW);
      workoutTime=millis()-startT;
      char myTime[8]; // buffer for converted string
      sprintf(myTime, "%d", workoutTime); // convert int to const char* string
      char myEnergy[8]; // buffer for converted string
      sprintf(myEnergy, "%d", Energy); // convert int to const char* string
      String msg=userid + "/" + deviceid + "/"+ myTime +"/" + myEnergy;
      client.publish("disconnect_device",msg.c_str());
      Serial.println(workoutTime);
    }
    prevflag=flag;
  }
}
// MQTT callback function to handle incoming messages
//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived in topic: ");
//  Serial.println(topic);
//  Serial.print("Message: ");
//
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//}

//void intToBytes(int value, byte* bytes) {
//  bytes[0] = (value >> 8) & 0xFF;
//  bytes[1] = value & 0xFF;
//}


//start session BT connect && value>0)
//publish user id/device id
//buzzer sound when connected
//session online led on

//when value=0 more than 5 sec end session
//publish user id/device id,time taken, energy
//unsigned long sum=0;


//energy/time every 5 sec
