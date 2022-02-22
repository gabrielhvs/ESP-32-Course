#include <Arduino.h>

//=============================================================================================================
//Mapeamento de Hardware

#define out1 15
#define out2 2
#define out3 4

//=============================================================================================================
//Declaração de variáveis 

int long time_out1 = 400;
int long time_save_out1 = 0;
int long time_out2 = 200;
int long time_save_out2 = 0;
int long time_out3 = 400;
int long time_save_out3 = 0;

//=============================================================================================================
//Inicialição

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
  
  if(millis()-time_save_out1>time_out1){
    digitalWrite(out1, !digitalRead(out1));
    time_save_out1 = millis();

  }

  if(millis()-time_save_out2>time_out2){
    digitalWrite(out2, !digitalRead(out2));
    time_save_out2 = millis();

  }

  if(millis()-time_save_out3>time_out3){
    digitalWrite(out3, !digitalRead(out3));
    time_save_out3 = millis();

  }
  
}