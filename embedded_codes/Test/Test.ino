#include <WiFi.h>
#include <PubSubClient.h>
#include <iostream>

// Replace with your WiFi credentials
const char* ssid = "ASUS_38_2G";
const char* password = "0123640216";

// MQTT broker details
const char* mqtt_server = "192.168.1.244";
const int mqtt_port = 1883;
const char* mqtt_user = "WarmWater";
const char* mqtt_password = "Warm1234";

std::string userid;
std::string deviceid="6969";
bool working_out=0;
bool prev_working_out=0;
bool moving=0;
bool konek=0;
bool buzzer;
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

unsigned long startT,workoutTime,workoutTimeTotal,time_stamp;
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
  client.setCallback(callback);

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
  std::string pair="/pair_request/"+deviceid;
  client.subscribe(pair.c_str());
    // Wait for incoming messages from MQTT broker
  
}

void loop() {
  client.loop();
  // Send a message to the MQTT broker
  if ((buzzer==1)&&((millis()-time_stamp)>1000)){
    ledcWrite(channel, 0); // stop generating the square wave
    buzzer=0;
    }
  if (konek){
    int value = analogRead(motorPin);
//    Serial.println(value);
    unsigned long now=millis();
    if (value>0){ 
      Energy+=value;
//      Serial.println(Energy);
      moving=1;      
    }
    
    else {
      if (moving){//every stop
        prev=millis();
        moving=0;
      }
      else{
      if ((now-prev)>5000) {//stop for more than 5 sec
        Serial.println("trigerred");
        working_out=0;
        prev=millis();
        } 
      }
    }
    if(moving && !working_out){
      working_out=1;
    }

    

      
    if(working_out!=prev_working_out){//start or end
      if (working_out){//start
        digitalWrite(LED,HIGH);
        startT=millis();
        std::string msg=userid + "/" + deviceid;
        client.publish("workout_started",msg.c_str());
      }
      else if (working_out==0){//end
        digitalWrite(LED,LOW);
        workoutTime=millis()-startT;
        workoutTimeTotal+=workoutTime;
        char myTime[8]; // buffer for converted std::string
        sprintf(myTime, "%d", workoutTimeTotal); // convert int to const char* std::string
        char myEnergy[8]; // buffer for converted std::string
        sprintf(myEnergy, "%d", Energy); // convert int to const char* std::string
        std::string msg=userid + "/" + myEnergy + "/" +myTime;
        client.publish("workout_pause",msg.c_str());
        //Serial.println(msg);
      }
      prev_working_out=working_out;
    }
    if((now-refresh)>2000&&(working_out)){// send data every 5sec
        workoutTime=millis()-startT;
        workoutTimeTotal+=workoutTime;
        char myTime[8]; // buffer for converted std::string
        sprintf(myTime, "%d", workoutTimeTotal); // convert int to const char* std::string
        char myEnergy[8]; // buffer for converted std::string
        sprintf(myEnergy, "%d", Energy); // convert int to const char* std::string
        std::string msg=userid + "/" + myEnergy + "/" +myTime;
        client.publish("energy_and_time",msg.c_str());
        refresh=now;
        workoutTimeTotal-=workoutTime;
      }
  }
}
// MQTT callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  std::string str(topic);
  std::string str2="/pair_request/"+deviceid;
  if (str==str2){
    int var = *payload;
    if(var != konek){
      time_stamp=millis();
      ledcWriteTone(channel, frequency);// generate a square wave of the specified frequency on the buzzer pin
      buzzer=1;
      if (*payload=='0'){
        konek=0;
        working_out=0;
        prev_working_out=0;
        moving=0;
        Energy=0;
        workoutTime=0;
        digitalWrite(LED,LOW);
        Serial.println("Disconnected");
        }
      if (*payload=='1'){
        konek=1;
        working_out=0;
        prev_working_out=0;
        moving=0;
        workoutTimeTotal=0;
        Serial.println("Connected");
      }
    std::string useridtemp="";
//    Serial.println(len);
    for (int i = 2; i < len; i++) {
      useridtemp+=(char)payload[i];
      }
    userid=useridtemp;
    }
    
    }
}
