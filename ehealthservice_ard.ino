#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#define USE_SERIAL Serial
ESP8266WiFiMulti WiFiMulti;
void notify();

#define pos D2
#define neg D1
#define pluse A0

void setup() {

  pinMode(pos, OUTPUT);
  pinMode(neg, OUTPUT);
  pinMode(pluse, INPUT);

  digitalWrite(neg, HIGH);
  USE_SERIAL.begin(115200);
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();
  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("rk", "lkjhgfdsa");
  notify();
}

void notify()
{
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    digitalWrite(pos, HIGH);
    digitalWrite(neg, LOW);
  }
  else {
    setup();
    digitalWrite(neg, HIGH);
    digitalWrite(pos, LOW);
  }
}

void loop() {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    int data = analogRead(pluse);
    USE_SERIAL.println(data / 10);

    HTTPClient http;
    http.begin("http://192.168.43.161:5000/value?storage=" + String(data / 10));
    int httpCode = http.GET();
    if (httpCode > 0) {
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        USE_SERIAL.println(payload);
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    if ((data / 10) > 100)
    {
      HTTPClient http;
      http.begin("http://192.168.43.161:5000/high");
      int httpCode = http.GET();
      if (httpCode > 0) {
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          USE_SERIAL.println(payload);
        }
      } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
    else if ((data / 10) < 50)
    {
      HTTPClient http;
      http.begin("http://192.168.43.161:5000/low");
      int httpCode = http.GET();
      if (httpCode > 0) {
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          USE_SERIAL.println(payload);
        }
      } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
    delay(5000);
  }
}
