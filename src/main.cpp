//Include

#include <FS.h> 
#include <ESP8266WiFi.h>
#include <SensorManager.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

#include "WiFiSetup.h"

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputState = "off";

// Assign output variables to GPIO pins
char output[2] = "5";

//PINS
#define INPUTPIN 1

//Set Up MQTT

char mqtt_server[40];
WiFiClient client;
PubSubClient MQTTclient(client);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message arrived [");
  Serial.println(topic);
  Serial.println("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void setup() {
    //Serial Start
    Serial.begin(115200);

    // Set Up Wifi Connection of needed (and variable storage)
    setUpWiFi();

    // Set Up MQTT
    Serial.println(mqtt_server);
    MQTTclient.setServer(mqtt_server,1883);
    MQTTclient.setCallback(callback);
    MQTTclient.connect("ESP-01_2-Client");
    MQTTclient.publish("ESP-01_2/state", "Up and running");
    Serial.println("ESP-01_2/state, Up and running");

    // Initialize the Pins
    pinMode(atoi(output), OUTPUT);
    pinMode(INPUTPIN, INPUT);

    // Set outputs to LOW
    digitalWrite(atoi(output), LOW);;
    
    server.begin();
}

void loop(){

  MQTTclient.publish("ESP-01_2/state", "Sent");
  Serial.println("ESP-01_2/state, Sent");
  delay(10000);
  Measure(MQTTclient);
  
}