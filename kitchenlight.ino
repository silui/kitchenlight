/**********************************************************************************
  * Code for controlling multiple devices connected to one NodeMCU using Amazon Echo
  * 
  * Written by Sid for Sid's E Classroom
  * 
  * https://www.youtube.com/c/SidsEClassroom
  *********************************************************************************/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include <Servo.h>
#define WIFI_SSID "THICC 2.4"//change your Wifi name
#define WIFI_PASS "ilovehongkong"//Change your Wifi Password
#define SERIAL_BAUDRATE                 115200
#define Kitchen D1
WiFiServer server(80);
Servo servo;
bool lightoff=false;

void servo_lightoff()
{
  servo.attach(Kitchen);
  servo.write(10);
  delay(650);
  servo.write(135);
  delay(650);
  servo.detach();
}
void checklightoff()
{
  if(lightoff)
  {
    servo_lightoff();
  }
  lightoff=false;
}

fauxmoESP fauxmo;
//declare switching pins
//Change pins according to your NodeMCU pinouts
#define Kitchen D1
// -----------------------------------------------------------------------------
// Wifi Setup
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}
// -----------------------------------------------------------------------------
// Device Callback
// -----------------------------------------------------------------------------
void callback(uint8_t device_id, const char * device_name, bool state) {
  Serial.print("Device "); Serial.print(device_name); 
  Serial.print(" state: ");
  if (state) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }
if ( (strcmp(device_name, "Kitchen Lights") == 0) ) 
  {
    lightoff=true;
  }
}

void setup() {
    servo.attach(Kitchen);
    delay(250);
    servo.detach();
    

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("After connection, ask Alexa/Echo to 'turn <devicename> on' or 'off'");

    // Wifi
    wifiSetup();
   server.begin();

    // Device Names for Simulated Wemo switches
    fauxmo.addDevice("Kitchen Lights");
    fauxmo.onMessage(callback);
}


void loop(void) {
  // Check if module is still connected to WiFi.
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected.");

    while (client.connected()) {
          fauxmo.handle();
      if (client.available()) 
      {
        char command = client.read();
        if (command == 'L') {
          lightoff=true;
          Serial.println("kitchenlight off");
        }
      }
      checklightoff();
    }
    Serial.println("Client disconnected.");
    client.stop();
  }
    fauxmo.handle();
    checklightoff();
}
