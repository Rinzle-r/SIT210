const int SwitchPin = 2;    
const int sound_detection_pin = 3;      
const int LedPin1 = 9;       
const int LedPin2 = 10;      

volatile bool Led1_Status = false;
volatile bool Led2_Status = false;

void setup() 
{
  pinMode(SwitchPin, INPUT_PULLUP);  
  pinMode(sound_detection_pin, INPUT);         
  pinMode(LedPin1, OUTPUT);
  pinMode(LedPin2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(SwitchPin), Button_Input, FALLING);
  attachInterrupt(digitalPinToInterrupt(sound_detection_pin), Sound_Input, RISING);

  Serial.begin(9600);
  Serial.println("Waiting for button or sound input...");
}

void loop() 
{
  digitalWrite(LedPin1, Led1_Status);
  digitalWrite(LedPin2, Led2_Status);

  static bool last_led1_status = false, last_led2_status = false;
  if (last_led1_status != Led1_Status) 
  {
    Serial.println("BUTTON TRIGGERED : ACTION LED 1");
    last_led1_status = Led1_Status;
  }
  if (last_led2_status != Led2_Status) 
  {
    Serial.println("SOUND DETECTED : ACTION LED 2");
    last_led2_status = Led2_Status;
  }

  delay(50); 
}

void Button_Input() 
{
  Led1_Status = !Led1_Status;
}

void Sound_Input() 
{
  Led2_Status = !Led2_Status;
}
