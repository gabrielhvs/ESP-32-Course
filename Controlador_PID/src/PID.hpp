//=============================================================================================================
//Bibliotecas Auxiliares
#include <Arduino.h>
#ifndef PID
#define PID

//=============================================================================================================
//Constantes do Projeto

#define FreqEsp 80000000

//=============================================================================================================
//Construção do objeto PID
class PIDcontrol{
   private:
        float kp,
              ki,
              kd;

        int  dt;
   public: 
        PIDcontrol(const float Vkp, const float Vki, const float Vkd, const int Vdt);
        double AplicPID(float SetPoint, float Messure);
};
 #endif
