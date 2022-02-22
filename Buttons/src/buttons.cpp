
//=============================================================================================================
//Bibliotecas Auxiliares
#include <Arduino.h>
#include "buttons.hpp"


//=============================================================================================================
//Declaração das funções do Objeto

Bts::Bts(const int bt1, const int bt2, const int bt3):pins{bt1,bt2,bt3}
{
    for (int i = 0; i<=QuantB; i++)
        pinMode(pins[i], INPUT_PULLDOWN);

}

unsigned Bts :: readBT(const int Nbt, const int bounce)
{
    static int flag = 0;
    if(!digitalRead(pins[Nbt])) flag = 1;
    if(digitalRead(pins[Nbt]) && flag){

        flag = 0;
        delay(bounce);
        return 1;
        
    }
    return 0;

}