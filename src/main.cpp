//Include

#include <FS.h> 
#include <ESP8266WiFi.h>
//#include <SensorManager.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
//#include "WiFiSetup.h"


#define DEBUG true

// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String outputState = "off";
// Assign output variables to GPIO pins
char outputpin[2] = "5";
char inputpin [2] = "5";

// MQTT IDs
char espid [10] = "ESP-01_01";
char ClientId[8] = "-Client";
char TopicSystem [8] = "/System";
char TopicSensor [8] = "/Sensor";
char TopicPulses [8] = "/Pulses";
char TopicPower [8] =  "/Powerh";

// Will be automatically built
char mqttClientId[17];
char mqttTopicSystem[17];
char mqttTopicSensor[17];
char mqttTopicPulses[17];
char mqttTopicPower[17];

// Pulse Variables
int maxPulseLenght = 200; // ms (50 standard)
int minPulseLenght = 20; // ms
long pulseLengt;
long pulseCount;
long totalCount;
bool pulseState;

// Time Keeping Vars
long timeElapsed, time1, time2, pulseBegin, measurementBegin, lastPulse;
bool measurement;
long minute = 60*1e3;
long hour = minute *60;
//long measurementPeriod = minute; // minute in ms
long measurementPeriod = minute/6;


/******************************
******************************
MQTT CONFIG
*******************************
*******************************/

//Set Up MQTT
char mqtt_server[40] = "test.mosquitto.org";
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

/// @brief Publish a Payload to a apotic and Debugs in Serial
/// @param Topic MQTT Topic
/// @param Message MQTT Payload
void mqtt_Publish_print (String Topic, String Message){
  //Check if connected
  if (!MQTTclient.connected()) {
  Serial.println("Attempting MQTT connection...");
  if (MQTTclient.connect(mqttClientId)) {
    Serial.println("Connected to MQTT server.");
  } else {
    Serial.println("Failed to connect to MQTT server.");
  }
}
  MQTTclient.publish(Topic.c_str() , Message.c_str());
  if (DEBUG) {
    Serial.println("----MQTT PUBLISH----");
    Serial.println("TOPIC:" + Topic);
    Serial.println("PAYLOAD:" + Message);
    Serial.println("--------------------");
  }
}


/******************************
******************************
WIFI CONFIG
*******************************
*******************************/

//flag for saving data
bool shouldSaveConfig = false;

/// @brief callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

/// @brief Sets Up the Wifi Connection and a server if necessary
void setUpWiFi(void)
{
    //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(outputpin, json["output"]);
          strcpy(mqtt_server, json["mqtt_server"]);
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  
  WiFiManagerParameter custom_output("output", "output", outputpin, 2);
  WiFiManagerParameter custom_input("input", "input", inputpin, 2);
  WiFiManagerParameter custom_espid("espid", "espid", espid, 10);
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //add all your parameters here
  wifiManager.addParameter(&custom_output);
  wifiManager.addParameter(&custom_mqtt_server);

  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("CONGIF WLAN - ESP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");

  //Update the Parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());  
  strcpy(outputpin, custom_output.getValue());
  //espid and topics
  strcpy(espid, custom_espid.getValue());
  //Client ID
  strcpy(mqttClientId,espid);
  strcat(mqttClientId,ClientId);
  //Sensor Topic
  strcpy(mqttTopicSensor,espid);
  strcat(mqttTopicSensor,TopicSensor);
  //System TopiC
  strcpy(mqttTopicSystem,espid);
  strcat(mqttTopicSystem,TopicSystem);
  //Pulses TopiC
  strcpy(mqttTopicPulses,espid);
  strcat(mqttTopicPulses,TopicPulses);
  //Powerh TopiC
  strcpy(mqttTopicPower,espid);
  strcat(mqttTopicPower,TopicPower);


  if (DEBUG){
    Serial.println(mqttClientId);
    Serial.println(mqttTopicSensor);
    Serial.println(mqttTopicSystem);
  }

  




  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["output"] = outputpin;
    json["mqtt_server"] = mqtt_server;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    
    //end save
  }
}

bool CheckInputPin(){
  if (digitalRead(atoi(inputpin)) == HIGH){
    return true;
  }else{
    return false;
  }
}

/******************************
******************************
ESP STANDARD FUNCTIONS
*******************************
*******************************/

/// @brief Device Setup after boot
void setup() {
    //Serial Start
    Serial.begin(115200);

    // Set Up Wifi Connection of needed (and variable storage)
    setUpWiFi();

    // Set Up MQTT
    Serial.println(mqtt_server);
    MQTTclient.setServer(mqtt_server,1883);
    MQTTclient.setCallback(callback);
    MQTTclient.connect(mqttClientId);
    mqtt_Publish_print(mqttTopicSystem, "Staurt-Up Done"); 
    
    // Initialize the Pins
    pinMode(atoi(outputpin), OUTPUT);
    pinMode(atoi(inputpin), INPUT);

    // Set outputs to LOW
    digitalWrite(atoi(outputpin), LOW);;
    
    server.begin();

}

/// @brief Looping Code
void loop(){
 // mqtt_Publish_print("ESP-01_2/System", "Looping");
  // if (digitalRead(atoi(inputpin)) == HIGH)
  // {
  //   mqtt_Publish_print(mqttTopicSensor, "Is High"); 
  // } else {
  //   mqtt_Publish_print(mqttTopicSensor, "Is Low"); 
  // }

  if (pulseState == false){
    if (CheckInputPin() == false){  // Pulse Started
        pulseState = true;
        pulseBegin = millis();  // Recird Starting time
        Serial.println("Pulse Started");
    }
  }

  if (pulseState == true && CheckInputPin() == true){ //Pulse Ended
    pulseLengt = millis() - pulseBegin; // Calculate the lenght
    if (pulseLengt < minPulseLenght || pulseLengt > maxPulseLenght){  //Invalid Lenght
      pulseState = false;
      Serial.println("-----------Invalid Pulse:");
      Serial.println(pulseLengt);
      Serial.println("-----------");
      
    } else {  // Valid Pulse! 
      Serial.println("-----------Valid Pulse:");
      Serial.println(pulseLengt);
      Serial.println("-----------");
      if (measurement == false) // Start the measurenment
      {
        measurementBegin = millis(); // Record the starting time
        measurement = true;
        Serial.println("Starting Measurement at:");
        Serial.println(measurementBegin);        
      }
      pulseState = false;         // end pulse state
      pulseCount++;               // increase counters
      totalCount++;
      lastPulse = millis();       // measure te moment of the last pulse in the measurement
    }
  }

  if (measurement == true && millis() > (measurementBegin + measurementPeriod)){
    Serial.println(MQTTclient.connected());
    mqtt_Publish_print(mqttTopicPulses,String(pulseCount));
    //calculate Power
    timeElapsed = lastPulse - pulseLengt; // The last Pulse defines the actual period
    int hourCount = (pulseCount -1) * ((hour)/(timeElapsed - pulseLengt)); //the last pulse must be substracted, because we should measure until the beginning of the pulse
    mqtt_Publish_print(mqttTopicPower,String(hourCount));

    // clean up values
    pulseCount = 0; // neu Count
    measurement = false;  //new Measurement
  }
}

