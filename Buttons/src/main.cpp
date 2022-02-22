#include <Arduino.h>
#include "buttons.hpp"
//=============================================================================================================
//Mapeamento de Hardware



#define out1 15
#define out2 2
#define out3 4
#define bt1 21
#define bt2 22
#define bt3 23

Bts teclado(bt1, bt2, bt3);

void setup() {
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);

  digitalWrite(out1, LOW);
  digitalWrite(out2, LOW);
  digitalWrite(out3, LOW);


}

//=============================================================================================================
//Loop

void loop() {

 if(teclado.readBT(0,130)) digitalWrite(out1, !digitalRead(out1));
 if(teclado.readBT(1,130)) digitalWrite(out2, !digitalRead(out2));
}