int ledPin=9;
  int sensorPin =A0;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  int sensorValue = analogRead(sensorPin);

  Serial.println(sensorValue);

  int brightness = map(sensorValue, 0, 650, 0, 255);  
  analogWrite(ledPin, brightness);   
  delay(10);          
}