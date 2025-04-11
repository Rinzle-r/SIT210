#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "cred.h"

const char* ifhost = "maker.ifttt.com";
const int ifpost = 80;

BH1750 light_sensor;
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, ifhost, ifpost);

bool Sunlight_detected = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  light_sensor.begin();
  connectToWiFi();
}

void loop() {
  float lux = light_sensor.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  if (lux > 50 && !Sunlight_detected)
  {
    triggerIFTTT(TRIGGER);      
    Sunlight_detected = true;       
  } else if (lux <= 50) {
    Sunlight_detected = false;     
  }

  delay(10000); 
}

void connectToWiFi() 
{
  Serial.print("Wifi connection");
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected");
}

void triggerIFTTT(const char* Trigger) 
{
  String url = "/trigger/" + String(Trigger) + "/with/key/" + String(IFTTT_KEY);
  client.get(url);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("IFTTT Trigger Status: ");
  Serial.println(statusCode);
}
