void setup() 
{
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.begin(115200);

}

void loop()
{
  Serial.println("Hello ESP32 on Arduino IDE");
  digitalWrite(2, HIGH);
  delay(741);
  digitalWrite(2,  LOW);
  delay(555);
  
}
