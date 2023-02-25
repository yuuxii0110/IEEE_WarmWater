#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>

// Replace with your WiFi credentials
const char* ssid = "ASUS_38_2G";
const char* password = "0123640216";

// MQTT broker details
const char* mqtt_server = "192.168.1.244";
const int mqtt_port = 1883;
const char* mqtt_user = "WarmWater";
const char* mqtt_password = "Warm1234";

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor Pins
const int motorPin = 34;

void setup() {
  pinMode(motorPin,INPUT);
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
  client.subscribe("test_topic");
}

void loop() {
  
  // Send a message to the MQTT broker
  client.publish("test_topic", "Hello from ESP32!");

  // Wait for incoming messages from MQTT broker
  client.loop();
}

// MQTT callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//float readMotor(){}
