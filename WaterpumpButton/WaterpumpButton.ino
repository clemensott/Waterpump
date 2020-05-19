/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#ifndef STASSID
#define STASSID "Ottspot"
#define STAPSK  "PASSWORD"
#define IP "192.168.1.110"
#define ID "57"
#endif

const int btnPin = 13;
const int ledPin = 14;
const unsigned long tapMillis = 50;
const unsigned long offMillis = 2000;

ESP8266WiFiMulti WiFiMulti;

void setup() {
  pinMode(btnPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);
}

void loop() {
  delay(1);
  if (!digitalRead(btnPin)) return;

  Serial.println("Down");

  unsigned long startMillis = millis();

  while (digitalRead(btnPin)) delay(1);

  Serial.print("Up: ");
  Serial.println(abs(millis() - startMillis), DEC);

  if (abs(millis() - startMillis) >= offMillis) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Off");

    String endpoint = "on?id=";
    endpoint += ID;
    endpoint += "&millis=0";

    while (!sendRequestSuccessful(endpoint)) delay(1000);

    Serial.println("Offed");
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
  }
  else if (abs(millis() - startMillis) >= tapMillis) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Tap");

    String endpoint = "on?id=";
    endpoint += ID;
    endpoint += "&millis=-1";
    while (!sendRequestSuccessful(endpoint)) delay(1000);

    Serial.println("Tapped");
    digitalWrite(ledPin, LOW);
  }
}

bool sendRequestSuccessful(String endpoint) {
  if ((WiFiMulti.run() != WL_CONNECTED)) return false;

  WiFiClient client;
  HTTPClient http;
  String url = "http://";
  url += IP;
  url += "/wasserpumpe/";
  url += endpoint;
  Serial.println(url);

  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, url)) {  // HTTP


    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);

        return true;
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }

  return false;
}

