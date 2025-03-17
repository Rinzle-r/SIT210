#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

#define LED_PIN 13  // Built-in LED

const char SSID[]     = "VodafonePocketWiFi4-DDC3";    
const char PASS[]     = "hD9D37eLNL6";    

bool BLINK_LED;

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

// To blink a dot
void LED_DOT()
{  
  digitalWrite(LED_PIN, HIGH);  
  delay(200); 

  digitalWrite(LED_PIN, LOW);  
  delay(200);  
}

// To blink a dash
void LED_DASH() {  
  digitalWrite(LED_PIN, HIGH);  
  delay(600);  // Long blink
  digitalWrite(LED_PIN, LOW);  
  delay(200);  
}

// To create a space between letters
void Letter_Space() 
{  
  delay(600);  
}

// To create a space between words
void Word_Space() 
{  
  delay(1400);  
}

// To blink a letter in Morse code
void BLINK_MORSE(char letter) 
{  
  Serial.print("Blinking Morse for letter ::> ");
  Serial.println(letter); 

  switch (letter) 
  {  
    case 'A': LED_DOT(); LED_DASH(); break;  
    case 'R': LED_DOT(); LED_DASH(); LED_DOT(); break;  
    case 'J': LED_DOT(); LED_DASH(); LED_DASH(); LED_DASH(); break;  
    case 'U': LED_DOT(); LED_DOT(); LED_DASH(); break;  
    case 'N': LED_DASH(); LED_DOT(); break;  
  }  
  Letter_Space();  // Space between letters
}

void initProperties() 
{
  ArduinoCloud.addProperty(BLINK_LED, READ, ON_CHANGE, NULL);  // To send LED property to the cloud
}

void setup() 
{  
  Serial.begin(9500);  
  pinMode(LED_PIN, OUTPUT);  
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);  
  initProperties(); 
}

void loop() 
{  
  ArduinoCloud.update();  
  char name[] = "ARJUN";  
  for (int i = 0; i < 5; i++) 
  {  
    BLINK_MORSE(name[i]);  
  }  
  Word_Space();  
}
