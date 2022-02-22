
//=============================================================================================================
//Bibliotecas Auxiliares
#include <Arduino.h>
#include "servo.hpp"
#include <math.h>

//=============================================================================================================
//Declaração das funções do Objeto

servo::servo(const int pin, const int chanel, const int resolution){
   Sresolution = resolution;
   Schanel = chanel;
   ledcAttachPin(pin, Schanel);
   ledcSetup(Schanel, 50, Sresolution); 
   Serial.begin(9600);
}
void servo :: Angle(const int angle)
{
   int static duttyMax; 
   int static pwm;
   duttyMax = int(pow(2,Sresolution));
   float Tangle=map(angle, 0, 180, 6, 24);
   pwm = int(((Tangle*0.1)/20)*duttyMax);
   ledcWrite(Schanel, pwm);
   Serial.println(Tangle);
  

}