

//=============================================================================================================
//Bibliotecas Auxiliares
#include <Arduino.h>
#ifndef KeyBorad
#define KeyBorad

//=============================================================================================================
//Constantes do Projeto

#define QuantB 3

//=============================================================================================================
//Construção do objeto teclado
class Bts{

    public: 
        Bts(const int bt1, const int bt2, const int bt3);
        unsigned readBT(const int Nbt, const int bounce);
    private:
        int pins [QuantB];


};
 #endif
