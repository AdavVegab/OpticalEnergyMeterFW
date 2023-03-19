
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "main.h"

// define times
#define SECOND 1e3  // 1e3 ms is 1 second
#define MINUTE SECOND*60  // 60e3 ms is 60 seconds = 1 minute
#define HOUR MINUTE*60 // number of millis in an hour
#define DAY HOUR*24 // number of millis in a day
#define MEASUREMENT MINUTE

#define PULSE_MAX_LENGTH 45   // actual LED pulse of power meter is 38 ms
#define PULSE_MIN_LENGTH 25   // actual LED pulse of power meter is 38 ms
#define TOTALDELAY 1000        // publish value of counter every ## counts

//Variables
long time_elapsed;
long time1;
long time2; 
long pulsebegin;
long measurementbegin;
long lastpulse;
long lastReconnectAttempt = 0;
long pulselength;
long pulsecount;
int totalcounter = 0;
bool pulsestate = false;    // false: not detecting a pulse
bool measurement = false;   // not measuring yet
bool debug = false;         // switched by mechanical switch, if true debug messages are published via MQTT
String tmp_str;             // String for publishing the int's as a string to MQTT
char buf[5];
#define INPUTPIN 5

void Measure(PubSubClient MQTTclient){
    if  (digitalRead(INPUTPIN) == HIGH)
    {
        MQTTclient.publish("ESP-01_2/state", "HIGH DETECTED");
        Serial.println("ESP-01_2/state, HIGH DETECTED");
        delay(2000);
    }
    // if (pulsestate == false)
    // {
    //     if (digitalRead(INPUTPIN) == LOW)   // a pulse started
    //     {
    //         pulsestate = true;
    //         pulsebegin = millis();
    //     }
    // }
    // if (pulsestate == true && digitalRead(INPUTPIN) == HIGH)    // pulse ended
    // {
    //     pulselength = millis() - pulsebegin;
    //     if (pulselength <  PULSE_MIN_LENGTH || pulselength > PULSE_MAX_LENGTH) // pulse too short  or too long
    //     {
    //      pulsestate = false;
    //     }
    //     else    // this was a good pulse
    //     {
    //         if (measurement == false)
    //         {
    //             measurementbegin = millis();           // start the measurement period
    //             measurement = true;                    // we are in a measurement
    //         }
    //         pulsestate = false;         // end pulse state
    //         pulsecount++;               // increase counters
    //         totalcounter++;
    //         lastpulse = millis();       // measure te moment of the last pulse in the measurement
    //         if (debug)      // publish debug messages?
    //         {
    //             tmp_str = String(pulselength); //converting count to a string
    //             tmp_str.toCharArray(buf, tmp_str.length() + 1);
    //             MQTTclient.publish("ESP-01_2/pulselength", buf);
    //             MQTTclient.publish("ESP-01_2/state", "total counts:");
    //             tmp_str = String(totalcounter); //converting int to a string
    //             tmp_str.toCharArray(buf, tmp_str.length() + 1);
    //             MQTTclient.publish("ESP-01_2/state", buf);
    //         }
    //         if (totalcounter % TOTALDELAY == 0)      // publish value every ## times as status update
    //         {
    //             MQTTclient.publish("ESP-01_2/state", "total counts:");
    //             tmp_str = String(totalcounter); //converting count to a string
    //             tmp_str.toCharArray(buf, tmp_str.length() + 1);
    //             MQTTclient.publish("ESP-01_2/state", buf);
    //         }
    //         //digitalWrite(LEDPIN, HIGH);     // blink led
    //         //delay(100);
    //         //digitalWrite(LEDPIN, LOW);
    //     }
    // }
    // if (measurement == true && millis() > (measurementbegin + MEASUREMENT))
    // {
    //     if (debug)      // publish debug messages
    //     {
    //         MQTTclient.publish("ESP-01_2/state", "measurement ended");
    //     }
    //     tmp_str = String(pulsecount); //converting count to a string
    //     tmp_str.toCharArray(buf, tmp_str.length() + 1);
    //     MQTTclient.publish("ESP-01_2/count", buf);

    //     // new calculation
    //     time_elapsed = lastpulse - measurementbegin;  // the last detected pulse defines the actual measurement period
    //     int hourcount = (pulsecount - 1) * ((HOUR) / (time_elapsed - pulselength));      //the last pulse must be substracted, because we should measure until the beginning of the pulse
    //     tmp_str = String(hourcount); //converting number to a string
    //     tmp_str.toCharArray(buf, tmp_str.length() + 1);
    //     //MQTTclient.publish("ESP-01_2/state", buf);
    //     MQTTclient.publish("sensor/powerW", buf);

    //     // clean up values
    //     pulsecount = 0;
    //     measurement = false;
  
}