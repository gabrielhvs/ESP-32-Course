// =======================================================================
// --- Bibliotecas ---
#include <Arduino.h>


// =======================================================================
// --- Mapeamento de Hardware ---
#define  adc_in      4
#define  light_ctr  23 


// =======================================================================
// --- Constantes ---
#define  dt        100  //atualização do PID


// =======================================================================
// --- Protótipo das Funções ---
float pid_control(float meas, float set_point); //função para o controle PID


// =======================================================================
// --- Variáveis Globais ---
int time_upt=0, pid_upt=0;

float   kp = 1.5,
        ki = 0.6,
        kd = 0.01;


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

  if(millis()-pid_upt >= dt)
  {
    pwm_val = pid_control(analogRead(adc_in),2800.0);
    ledcWrite(0,pwm_val+2048);
    pid_upt = millis();

  } //end if millis pid
  
  if(millis()-time_upt >= 741)
  {
    Serial.print("light=  ");
    Serial.println(analogRead(adc_in));
    time_upt = millis();
 
  } //end millis 741 

} //end loop


// =======================================================================
// --- Desenvolvimento das Funções ---

float pid_control(float meas, float set_point)
{
  static float last_meas;

  float        error_meas,
               proportional,
               integral,
               derivative;

  error_meas = set_point - meas;

  proportional = kp*error_meas;

  integral += (error_meas*ki) * (dt/1000.0);

  derivative = ((last_meas - meas)*kd) / (dt/1000.0);
  last_meas = meas;

  return (proportional+integral+derivative);


} //end pid_control













        
/* ======================================================================= 
                                                              
                                       _                      
                                      / \                     
                                     |oo >                    
                                     _\=/_                    
                    ___         #   /  _  \   #               
                   /<> \         \\//|/.\|\\//                
                 _|_____|_        \/  \_/  \/                 
                | | === | |          |\ /|                    
                |_|  0  |_|          \_ _/                    
                 ||  0  ||           | | |                    
                 ||__*__||           | | |                    
                |* \___/ *|          []|[]                    
                /=\ /=\ /=\          | | |                    
________________[_]_[_]_[_]_________/_]_[_\______________     
                                                              
                                                              
======================================================================= */
/* --- Final do Programa --- */






