// =======================================================================
// --- Bibliotecas ---
#include <Arduino.h>
#include "PID.hpp"


// =======================================================================
// --- Mapeamento de Hardware ---
#define  adc_in      4
#define  light_ctr   18 

// =======================================================================
// --- Variáveis Globais ---
int time_upt=0;

// =======================================================================
// --- Declaração de Objetos ---

PIDcontrol myPID(1.0,0.02,0.00,100);

// =======================================================================
// --- Configurações Iniciais ---
void setup() 
{
  Serial.begin(115200);
  pinMode(adc_in,     INPUT);
  pinMode(light_ctr, OUTPUT);

  ledcAttachPin(light_ctr,0);
  ledcSetup(0,2000,12);
  ledcWrite(0,2048);
 

} //end setup


// =======================================================================
// --- Loop Infinito ---
void loop() 
{
  float pwm_val;
  pwm_val =myPID.AplicPID(100.0, analogRead(adc_in));
  ledcWrite(0,pwm_val+2048);

  if(millis()-time_upt >= 741)
  {
    Serial.print("LDR=  ");
    Serial.print(analogRead(adc_in));
    Serial.print("  pwm=  ");
    Serial.print(pwm_val);
    Serial.println("%");
    time_upt = millis();
 
  } //end millis 741 

} //end loop
