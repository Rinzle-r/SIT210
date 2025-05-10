#define USING_TIMER_TC4 true
#include <Arduino.h>
#include <SAMDTimerInterrupt.h>
#include <DHT.h>

#define SWITCH_PIN 2
#define SOUND_PIN 3
#define DHT_PIN 4
#define LED1_PIN 5
#define LED2_PIN 6

#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

SAMDTimer Sensors_Timer(TIMER_TC4);
#define DTH_TIMER_INTERVAL_MS 1000

volatile bool Buttonled_status = false;
volatile bool Sound_Detected = false;
bool led_2_on = false;
unsigned long Led_SoundTimeout = 0;
const unsigned long Led_SoundDuration = 2000;

volatile bool Data_DTH = false;  

void handleSwitchInterrupt() 
{
  Buttonled_status = !Buttonled_status;
  digitalWrite(LED1_PIN,
  Buttonled_status);
  if (Buttonled_status) 
  {
    Serial.println("LED1 ON (Button Pressed) >>");
  } else 
  {
    Serial.println("LED1 OFF (Button Pressed) >>");
  }
}

void handleSoundInterrupt() 
{
  Sound_Detected = true;
  Led_SoundTimeout = millis() + Led_SoundDuration;
}

void Timer_Trigger_Handler() 
{
  Data_DTH = true;
}

void setup() 
{
  delay(1000);
  Serial.begin(9600);

  unsigned long start = millis();
  while (!Serial && (millis() - start < 5000));

  Serial.println("Interrupt Demo Running...");

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(SOUND_PIN, INPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  dht.begin();

  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), handleSwitchInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(SOUND_PIN), handleSoundInterrupt, FALLING);

  if (Sensors_Timer.attachInterruptInterval(DTH_TIMER_INTERVAL_MS * 1000, Timer_Trigger_Handler)) 
  {
    Serial.println("Timer started successfully");
  } else 
  {
    Serial.println("Timer failed to start");
  }
}

void loop() 
{
  if (Sound_Detected) 
  {
    if (!led_2_on) 
    {
      digitalWrite(LED2_PIN, HIGH);
      Serial.println("LED2 ON (Sound Detected) >>");
      led_2_on = true;
    }

    if (millis() > Led_SoundTimeout) 
    {
      digitalWrite(LED2_PIN, LOW);
      Serial.println("LED2 OFF (No Sound) >>");
      Sound_Detected = false;
      led_2_on = false;
    }
  }

  if (Data_DTH) 
  {
    Data_DTH = false;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) 
    {
      Serial.println("DHT Read Failed");
    } else 
    {
      Serial.print("Temp: ");
      Serial.print(temp);
      Serial.print(" °C, Humidity: ");
      Serial.print(hum);
      Serial.println(" %");
    }
  }
}
