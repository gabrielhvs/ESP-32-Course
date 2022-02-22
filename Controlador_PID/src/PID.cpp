
//=============================================================================================================
//Bibliotecas Auxiliares
#include <Arduino.h>
#include "PID.hpp"
#include <math.h>

//=============================================================================================================
//Declaração das funções do Objeto

PIDcontrol :: PIDcontrol(const float Vkp, const float Vki, const float Vkd, const int Vdt){
   kp = Vkp;
   ki = Vki;
   kd = Vkd;
   dt = Vdt;
}

double PIDcontrol::AplicPID(float SetPoint, float Messure)
{
   static double proporcional = 0, 
          integral =0, 
          derivativo = 0, 
          error = 0,
          lastMes = 0;
    static long int  Tpid_upd = 0;
    
    if(millis() - Tpid_upd >= dt){        
        error = SetPoint - Messure;
        proporcional = kp*error;
        integral += ki*error*(dt/1000);
        derivativo = (kd*(lastMes-Messure))/(dt/1000.0);
        Tpid_upd=millis();
        lastMes=Messure;
        
    }
   
    return (proporcional+integral+derivativo);
}