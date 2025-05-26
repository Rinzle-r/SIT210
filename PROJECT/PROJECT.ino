#include <WiFiNINA.h>
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>
#include "ThingSpeak.h"
#include <ArduinoHttpClient.h>
#include "credentials.h"

//Defined pins for sensor and components 
#define DHTPIN 2
#define DHTTYPE DHT11
#define RED_LED_PIN 6       // for light alert
#define GREEN_LED_PIN 4     // for system status on or off
#define BUZZER_PIN 5        // alerting user with buzzer
#define SOIL_LED_PIN 7      // for soil alert 
#define SOIL_PIN A0
#define BUTTON_PIN 8       // button for turning on/off the system 

// setting up ifttt
const char* domain = "maker.ifttt.com";
const int port = 80;

// initialising sensor and communication objects
DHT dht(DHTPIN, DHTTYPE);
BH1750 light_sensor;
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, domain, port);
WiFiClient thingSpeakClient;

// Threshold for alerts 
const int light_Threshold = 300;
const int Soil_Threshold = 500; // buzzer ON

// using flags to prevent repeated alerts 
bool Sending_low_light_alert = false;
bool Sending_light_alert = true;
bool temp_alert = false;
bool humi_alert = false;

// for managing blinking logic
unsigned long Last_Blink = 0;
const unsigned long Blink_Delay = 300;
bool Blinking = false;
bool System_Active = false;

// intervals for printing values
unsigned long last_Printed = 0;
const unsigned long Print_Delay = 2000;

void setup() 
{
  // starting serial monitor
  Serial.begin(9600);
  // pins setup
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(SOIL_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // starting sensor connection 
  Wire.begin();
  dht.begin();
  light_sensor.begin();
  // connecting to wifi and thingspeak
  connectWiFi();
  ThingSpeak.begin(client);

  Serial.println("System ready, Slide switch to start >> ");
}

void loop() 
{
  int reading = digitalRead(BUTTON_PIN);

  // based on switch system will turn on or off
  if (reading == LOW && !System_Active) 
  {
    Serial.println("System state changed >> ON");
    digitalWrite(GREEN_LED_PIN, HIGH);
    System_Active = true;
  } 
  else if (reading == HIGH && System_Active)
  {
    Serial.println("System state changed >> OFF");
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(SOIL_LED_PIN, LOW);
    noTone(BUZZER_PIN);
    Blinking = false;
    System_Active = false;
  }

  if (System_Active) 
  {
    // Reading sensors
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    float lux = light_sensor.readLightLevel();
    int soilValue = analogRead(SOIL_PIN);

    // Sensor unplug or error detection
    if (soilValue < 100 || soilValue > 950) {
      Serial.println("Soil sensor error or unplugged!");
      digitalWrite(SOIL_LED_PIN, HIGH);
      tone(BUZZER_PIN, 2000); // high pitch tone
      return;
    }

    // soil dry condition
    if (soilValue > Soil_Threshold) 
    {
      Serial.println("Dry soil alert");
      digitalWrite(SOIL_LED_PIN, HIGH);
      tone(BUZZER_PIN, 1000); // Soil alert tone
    } 
    else 
    {
      digitalWrite(SOIL_LED_PIN, LOW);
      noTone(BUZZER_PIN);
    }

    // if light is low then blink the led for light alert 
    if (lux < light_Threshold) {
      unsigned long Current_val = millis();
      if (Current_val - Last_Blink >= Blink_Delay)
      {
        Last_Blink = Current_val;
        Blinking = !Blinking;
        digitalWrite(RED_LED_PIN, Blinking ? HIGH : LOW);
      }
    } 
    else 
    {
      digitalWrite(RED_LED_PIN, LOW);
    }

    // it will send a ifttt alert if the light is low 
    if (lux < light_Threshold && !Sending_low_light_alert) 
    {
      triggerIFTTT("low_light_alert");
      Sending_low_light_alert = true;
      Sending_light_alert = false;
    }
    else if (lux >= light_Threshold && !Sending_light_alert)
    {
      triggerIFTTT("light_alert");
      Sending_light_alert = true;
      Sending_low_light_alert = false;
    }

    // ifttt alert for temperature
    if (temp > 35 && !temp_alert) 
    {
      triggerIFTTT("high_temperature_alert");
      temp_alert = true;
    } 
    else if (temp < 15 && !temp_alert) 
    {
      triggerIFTTT("low_temperature_alert");
      temp_alert = true;
    } 
    else if (temp >= 15 && temp <= 35 && temp_alert) 
    {
      triggerIFTTT("Temperature_Stabilised_Conditions_Normal");
      temp_alert = false;
    }

    // ifttt alert for humidity
    if (hum < 30 && !humi_alert) 
    {
      triggerIFTTT("low_humidity_alert");
      humi_alert = true;
    } 
    else if (hum > 70 && !humi_alert) 
    {
      triggerIFTTT("high_humidity_alert");
      humi_alert = true;
    } 
    else if (hum >= 30 && hum <= 70 && humi_alert) 
    {
      triggerIFTTT("Humidity_Stabilised_Environment_Healthy_Again");
      humi_alert = false;
    }

    // printing values to serial monitor every 2 seconds
    unsigned long Current_Time = millis();
    if (Current_Time - last_Printed >= Print_Delay) 
    {
      last_Printed = Current_Time;

      Serial.print("Temp >> ");
      Serial.println(temp);
      Serial.print("Humidity >> ");
      Serial.println(hum);
      Serial.print("Light (lux) >> ");
      Serial.println(lux);
      Serial.print("Soil Moisture >> ");
      Serial.println(soilValue);
    }

    // Send the data to ThingSpeak
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, hum);
    ThingSpeak.setField(3, lux);
    ThingSpeak.setField(4, soilValue);

    int status = ThingSpeak.writeFields(THINGSPEAK_CHANNEL, THINGSPEAK_API_KEY);
    if (status == 200) 
    {
      Serial.println("ThingSpeak updated successful");
    } 
    else 
    {
      Serial.print("ThingSpeak error ");
      Serial.println(status);
    }
  }

  // delay to avoid overloading
  delay(500);
}

// it will try to connect wit wifi
void connectWiFi() 
{
  while (WiFi.begin(SECRET_SSID, SECRET_PASS) != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("....Connected to WiFi....");
}

// sending data to ifttt webhook based on event name
void triggerIFTTT(const char* Tri_event) 
{
  String url = "/trigger/" + String(Tri_event) + "/with/key/" + String(IFTTT_KEY);
  client.get(url);

  int statusCode = client.responseStatusCode();
  Serial.print("IFTTT Trigger Status: ");
  Serial.println(statusCode);
}
