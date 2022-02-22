#include <Arduino.h>

//=============================================================================================================
// - Funções para PWM
// ledcAttachPin(pin,ch);  //associa o pino ao canal PWM
// ledcSetup(ch,freq,res); // canal ultilizado, frequancia e resolução
// ledcWrite(ch, dutty);   // canal ultilizado, duty cicle (deve ser configurado por meio da resolução do ch)

//=============================================================================================================
// --- Mapeamento de Hardware
#define pwm_tst 13
#define brilho_led 15
#define leitura_pwm_tst 12
#define pot 2
#define buzzer 27
#include "servo.hpp"

servo servo1(14, 9, 16);

void setup() {

  //Teste Teorico
  pinMode(pwm_tst, OUTPUT);
  pinMode(leitura_pwm_tst, INPUT);
  pinMode(pot, INPUT);
  ledcAttachPin(pwm_tst,2);  
  ledcSetup(2,1000,11);// a frequancia levando em conta o limite de fmax = 80Mhz/(2^resolução) 
  ledcWrite(2, 1024);

  //Teste brilho Led
  pinMode(brilho_led, OUTPUT);
  ledcAttachPin(brilho_led,3); //Canal diferente para não ter interferência
  ledcSetup(3,1000,11);
  ledcWrite(3, 1024);

  //Teste Buzzer
  pinMode(buzzer, OUTPUT);
  ledcAttachPin(buzzer,2);  //Teste utilizando o mesmo canal do teste teorico 

 //Teste Servo
 //ledcAttachPin(servo, 9);
 //ledcSetup(9, 50, 16); //50Hz de frequancia foi escolhido lvando em conta a definição de 20ms de periodo
 //ledcWrite(9, 7864); //Para o angulo de de 180 
 
  Serial.begin(9600);

}

void loop() {

//Teste  teorico
  int duty= map(analogRead(pot), 0, 4095, 0, 2048);
  ledcWrite(2, duty);
  Serial.println(digitalRead(leitura_pwm_tst));

// Teste variando um brilho de led
  for(int i=0; i<=2048; i+=10){
    ledcWrite(3, i);
    delay(100);
  }
  for(int i=2048; i>=0; i-=10){
    ledcWrite(3, i);
    delay(100);
  }

//Teste de Servo
 servo1.Angle(0);
  delay(1000);
  servo1.Angle(90);
  delay(1000);
  servo1.Angle(180);
  delay(1000);
}