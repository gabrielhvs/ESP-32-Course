

//=============================================================================================================
//Bibliotecas Auxiliares
#include <Arduino.h>
#ifndef SERVO
#define SERVO

//=============================================================================================================
//Constantes do Projeto

#define FreqEsp 80000000

//=============================================================================================================
//Construção do objeto teclado
class servo{
   private:
        int  Schanel;
        int  Sresolution;
   public: 
        servo(const int pin, const int chanel, const int resolution);
        void Angle(const int angle);
 


};
 #endif
