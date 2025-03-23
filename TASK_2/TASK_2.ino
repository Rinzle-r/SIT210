#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include "cred.h"  

#define DHTPIN 2       
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;
unsigned long ChannelNumber = 2888958;  

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  Serial.print("waiting for WiFi connection .............");
  while (WiFi.begin(SECRET_SSID, SECRET_PASS) != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWifi Connected ~ ");

  ThingSpeak.begin(client);
}

void loop() 
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) 
  {
    Serial.println("Error!, can't read the data");
    return;
  }

  Serial.print("Temperature :: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity :: ");
  Serial.print(humidity);
  Serial.println("%");

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  int status = ThingSpeak.writeFields(ChannelNumber, SECRET_API_KEY);

  if (status == 200) 
  {
    Serial.println("Data sent to ThingSpeak >>");
  } else 
  {
    Serial.print("Error!, can't send the data ");
    Serial.println(status);
  }

  delay(60000);  
}