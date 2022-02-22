#include <Arduino.h>

void setup() {
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  Serial.begin(115200);
}

void loop() {
 static int i=0;
 Serial.println("ESP32 VISUAL STUDIO CODE!"); 
 Serial.println(i); 
 digitalWrite(2,HIGH);
 delay(50);
 digitalWrite(2,LOW);
 delay(50);
 i++;
}