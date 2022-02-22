/* =======================================================================

   CURSO ESP32: Aprenda de Verdade!
   WR Kits: https://wrkits.com.br/ | https://space.hotmart.com/wrkits 

   Gerador PWM Profissional com ESP32
   
   ESP-WROOM-32
   Board: DevKitV1
   Compilador: Arduino IDE 1.8.4

   Autor: Eng. Wagner Rambo
   Data:  Março de 2021

 
======================================================================= */


// =======================================================================
// --- Bibliotecas Auxiliares ---
#include <Arduino.h>                                       //biblioteca Arduino
#include <Wire.h>                                          //biblioteca para o protocolo I2C
#include <LiquidCrystal_I2C.h>                             //biblioteca para o display I2C


// =======================================================================
// --- Mapeamento de Hardware ---
#define   pwm_out   18                                     //saída PWM
#define   amp1       2                                     //controle para amplitude do PWM (LSB)
#define   amp2       4                                     //controle para amplitude do PWM (MSB)
#define   bt_esc    26                                     //botão de esc
#define   bt_dec    25                                     //botão de decremento '-'
#define   bt_inc    33                                     //botão de incremento '+'
#define   bt_ent    32                                     //botão enter


// =======================================================================
// --- Constantes ---
#define   pwm_ch     0                                     //canal PWM selecionado
#define   M_MAX      5                                     //número máximo de menus no projeto
#define   AMP_MAX    2                                     //número máximo de amplitudes disponíveis
#define   fast_adj   1000                                  //indicador para ajuste rápido de frequência


// =======================================================================
// --- Objetos ---
LiquidCrystal_I2C lcd(0x3F, 16, 2);                        //cria objeto lcd, endereço 3Fh, 16x2


// =======================================================================
// --- Protótipo das Funções ---
void read_keyb();                                          //leitura dos botões
void menu_sel();                                           //controle de seleção de menus
void sub_menu();                                           //ação nos sub-menus
void bLight_turnOff(uint16_t time_ms);                     //temporização para desligar backlight
void pwm_boot(uint16_t boot_ms);                           //função de boot do sistema


// =======================================================================
// --- Variáveis Globais ---
uint32_t         pwm_freq = 1000;                          //frequência do PWM
int16_t          pwm_duty = 50;                            //duty cycle do PWM (inicia em 50%)
uint8_t          pwm_res  = 10;                            //resolução do PWM
int              menu_num = 1,                             //número para lista de menus
                 ampli    = 1;                             //seleção de amplitude
unsigned long    bl_time_save = 0,                         //tempo para manter backlight ligado
                 bt_time      = 0;                         //armazena o tempo de clique dos botões
bool             adj      = 0,                             //indicador de ajuste de parâmetros
                 inc_fl   = 0,                             //indicador de incremento
                 dec_fl   = 0;                             //indicador de decremento


// =======================================================================
// --- Configurações Iniciais ---
void setup()
{

  pinMode(pwm_out,       OUTPUT);                          //configura pwm_out como saída
  pinMode(amp1,          OUTPUT);                          //saída para controle de amplitude LSB
  pinMode(amp2,          OUTPUT);                          //saída para controle de amplitude MSB
  pinMode(bt_esc,  INPUT_PULLUP);                          //entrada para botão esc com pull-up interno
  pinMode(bt_dec,  INPUT_PULLUP);                          //entrada para botão dec com pull-up interno
  pinMode(bt_inc,  INPUT_PULLUP);                          //entrada para botão int com pull-up interno
  pinMode(bt_ent,  INPUT_PULLUP);                          //entrada para botão ent com pull-up interno

  digitalWrite(amp1,        LOW);                          //amp1 inicia em LOW
  digitalWrite(amp2,        LOW);                          //amp2 inicia em LOW
  
  lcd.begin();                                             //inicializa LCD
  pwm_boot(4000);                                          //tempo de boot do sistema, 4 segundos

  ledcAttachPin(pwm_out,pwm_ch);                           //associa pino ao canal PWM
  ledcSetup(pwm_ch,pwm_freq,pwm_res);                      //configura frequência e resolução
  ledcWrite(pwm_ch,map(pwm_duty,0,100,0,1023));            //seta duty cycle

 
} //end setup


// =======================================================================
// --- Loop Infinito ---
void loop()
{

  if(!adj) menu_sel();                                     //se flag de ajuste limpa, chama menu principal
  else     sub_menu();                                     //senão, entra no sub-menu correspondente

  read_keyb();                                             //lê teclado
  
  bLight_turnOff(2000);                                    //apaga backlight depois de 2 segundos, após clique em qualquer botão

 
} //end loop


// =======================================================================
// --- Desenvolvimento das Funções ---


// =======================================================================
// --- menu_sel ---
// → Controle de seleção dos menus
void menu_sel()
{

  switch(menu_num)                                         //verifica menu_num
  {
    case 1:                                                //menu 1
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print(">a) Set Duty:   ");                        //menu atual: duty cycle
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print(" b) Set Freq:   ");                        //próximo menu da lista
     break;                                                //encerra o laço
    case 2:                                                //menu 2
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print(">b) Set Freq:   ");                        //menu atual: frequência
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print(" c) Set Ampl:   ");                        //próximo menu da lista
     break;                                                //encerra o laço
    case 3:                                                //menu 3
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print(">c) Set Ampl:   ");                        //menu atual: amplitude
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print(" d) P.Dty Sweep:");                        //próximo menu da lista
     break;                                                //encerra o laço
    case 4:                                                //menu 4
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print(">d) P.Dty Sweep:");                        //menu atual: varredura positiva de duty cycle
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print(" e) N.Dty Sweep:");                        //próximo menu da lista
     break;                                                //encerra o laço
    case 5:                                                //menu 5 
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print(">e) N.Dty Sweep:");                        //menu atual: varredura negativa de duty cycle
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 1
     lcd.print(" a) Set Duty:   ");                        //próximo menu da lista
     break;                                                //encerra o laço
      
  } //end switch 
  
 
} //end menu_sel


// =======================================================================
// --- sub_menu ---
// → Mostra sub-menu atual, efetua ajuste dos parâmetros
void sub_menu()
{
  bool dtZero = 0, noZero = 0;                             //variáveis locais para limpeza de zeros à esquerda
  
  switch(menu_num)                                         //verifica menu_num
  {        
    case 1:                                                //caso 1  
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print("    Set Duty    ");                        //título do menu atual
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print("val: ");                                   //imprime texto val
     lcd.setCursor(5,1);                                   //posiciona cursor coluna 6, linha 2

     if(!(pwm_duty/100) && !dtZero)                        //primeiro caractere é zero e dtZero limpa?
       lcd.write(' ');                                     //sim, imprime espaço em branco
     else                                                  //senão...
     {
       lcd.write((char)((pwm_duty/100)+0x30));             //...imprime caractere (centenas)
       dtZero=1;                                           //seta dtZero para imprimir todos os demais caracteres
     }
     
     if(!(pwm_duty%100/10) && !dtZero)                     //segundo caractere é zero e dtZero limpa?
       lcd.write(' ');                                     //sim, imprime espaço em branco
     else                                                  //senão...
     {  
       lcd.write((char)((pwm_duty%100/10)+0x30));          //...imprime caractere (dezenas)
       dtZero=1;                                           //seta dtZero para imprimir todos os demais caracteres
     }
     
     lcd.write((char)((pwm_duty%10)+0x30));                //imprime caractere (unidades)
     lcd.setCursor(9,1);                                   //posiciona cursor coluna 10, linha 2
     lcd.write('%');                                       //imprime '%'

     if(inc_fl)                                            //flag inc setada?
     {                                                     //sim
       pwm_duty++;                                         //incrementa duty
       if(pwm_duty>100) pwm_duty=0;                        //se maior que 100, volta para o 0
       ledcWrite(pwm_ch,map(pwm_duty,0,100,0,1023));       //atualiza duty cycle
       inc_fl=0;                                           //limpa flag inc
     } //end if inc_fl

     if(dec_fl)                                            //flag dec setada?
     {                                                     //sim
       pwm_duty--;                                         //decrementa duty
       if(pwm_duty < 0) pwm_duty=100;                      //se menor que zero, volta para o 100
       ledcWrite(pwm_ch,map(pwm_duty,0,100,0,1023));       //atualiza duty cycle
       dec_fl=0;                                           //limpa flag dec
     } //end if dec_fl

     break;

    case 2:                                                //caso 2
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print("    Set Freq    ");                        //título do menu atual
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print("val: ");                                   //imprime texto val
     lcd.setCursor(5,1);                                   //posiciona cursor coluna 6, linha 2
     
     if(!(pwm_freq/10000) && !noZero)                      //primeiro caractere é zero e noZero limpa?
       lcd.write(' ');                                     //sim, imprime espaço em branco
     else                                                  //senão...
     {
       lcd.write((char)((pwm_freq/10000)+0x30));           //...imprime caractere (dezenas de milhares)
       noZero=1;                                           //seta noZero para imprimir todos os demais caracteres
     }  

     if(!(pwm_freq%10000/1000) && !noZero)                 //segundo caractere é zero e noZero limpa?
       lcd.write(' ');                                     //sim, imprime espaço em branco
     else                                                  //senão...
     {
       lcd.write((char)((pwm_freq%10000/1000)+0x30));      //...imprime caractere (milhares)
       noZero=1;                                           //seta noZero para imprimir todos os demais caracteres
     }

     if(!(pwm_freq%1000/100) && !noZero)                   //terceiro caractere é zero e noZero limpa?
       lcd.write(' ');                                     //sim, imprime espaço em branco
     else                                                  //senão...
     {
       lcd.write((char)((pwm_freq%1000/100)+0x30));        //...imprime caractere (centenas)
       noZero=1;                                           //seta noZero para imprimir todos os demais caracteres
     }
       
     if(!(pwm_freq%100/10) && !noZero)                     //quarto caractere é zero e noZero limpa?
       lcd.write(' ');                                     //sim, imprime espaço em branco
     else                                                  //senão...
     {
       lcd.write((char)((pwm_freq%100/10)+0x30));          //...imprime caractere (dezenas)
       noZero=1;                                           //seta noZero para imprimir todos os demais caracteres
     }

       
     lcd.write((char)((pwm_freq%10)+0x30));                //imprime quinto caractere (unidades)
     lcd.setCursor(11,1);                                  //posiciona cursor coluna 12, linha 2
     lcd.print("Hz");                                      //imprime texto Hz

     if(inc_fl)                                            //flag de incremento setada?
     {                                                     //sim
       pwm_freq++;                                         //incrementa frequência
       ledcSetup(pwm_ch,pwm_freq,pwm_res);                 //configura frequência e resolução
       ledcWrite(pwm_ch,map(pwm_duty,0,100,0,1023));       //atualiza duty cycle
       inc_fl=0;                                           //limpa flag de incremento
     } //end if inc_fl

     if(dec_fl)                                            //flag de decremento setada?
     {                                                     //sim
       pwm_freq--;                                         //decremente frequência
       ledcSetup(pwm_ch,pwm_freq,pwm_res);                 //configura frequência e resolução
       ledcWrite(pwm_ch,map(pwm_duty,0,100,0,1023));       //atualiza duty cycle
       dec_fl=0;                                           //limpa flag de decremento
     } //end if dec_fl

     break;                                                //encerra o laço

    case 3:                                                //caso 3
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print("    Set Ampl    ");                        //título do menu atual
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print("val: ");                                   //imprime texto val
     lcd.setCursor(5,1);                                   //posiciona cursor coluna 6, linha 2
   

     if(inc_fl)                                            //flag inc setada?
     {                                                     //sim
       ampli++;                                            //incrementa amplitude 
       if(ampli>AMP_MAX)ampli=1;                           //verifica valor máximo volta para 1 se for atingido
       inc_fl=0;                                           //limpa flag inc
     } //end if inc_fl

     if(dec_fl)                                            //flag dec setada?
     {                                                     //sim
       ampli--;                                            //decrementa amplitur  
       if(ampli<1)ampli=AMP_MAX;                           //volta para valor máximo, se for menor que 1
       dec_fl=0;                                           //limpa flag dec
     } //end if dec_fl

     switch(ampli)                                         //verifica ampli
     {
       case 1:                                             //caso 1
        lcd.print("3.3 V");                                //imprime 3.3 V
        digitalWrite(amp1,  LOW);                          //amp1 em LOW 
        digitalWrite(amp2,  LOW);                          //amp2 em LOW
        break;                                             //encerra o laço
      
       case 2:                                             //caso 2
        lcd.print("5.0 V");                                //imprime 5.0 V
        digitalWrite(amp1, HIGH);                          //amp1 em HIGH 
        digitalWrite(amp2,  LOW);                          //amp2 em  LOW
        break;                                             //encerra o laço
      
     } //end switch ampli

     break;                                                //encerra o laço

    case 4:                                                //caso 4
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print("  P.Dty Sweep   ");                        //título do menu atual
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print("running...");                              //imprime texto running...

     pwm_duty++;                                           //incrementa duty
     if(pwm_duty>100) pwm_duty=0;                          //se maior que 100, volta para o 0
     ledcWrite(pwm_ch,map(pwm_duty,0,100,0,1023));         //atualiza duty cycle
     delay(10);                                            //tempo entre incrementos                              
     break;                                                //encerra o laço

    case 5:                                                //caso 5
     lcd.setCursor(0,0);                                   //posiciona cursor coluna 1, linha 1
     lcd.print("  N.Dty Sweep   ");                        //título do menu atual
     lcd.setCursor(0,1);                                   //posiciona cursor coluna 1, linha 2
     lcd.print("running...");                              //imprime texto running...

     pwm_duty--;                                           //decrementa duty
     if(pwm_duty < 0) pwm_duty=100;                        //se menor que zero, volta para o 100
     ledcWrite(pwm_ch,map(pwm_duty,0,100,0,1023));         //atualiza duty cycle
     delay(10);                                            //tempo entre decrementos
     break;                                                //encerra o laço
    
  } //end switch menu_num
  
} //end sub_menu
 

// =======================================================================
// --- read_keyb ---
// → Leitura, tratamento e ação dos botões (teclado)
void read_keyb()
{
  static bool st_esc=0,                                    //status do botão esc
              st_dec=0,                                    //status do botão dec
              st_inc=0,                                    //status do botão inc
              st_ent=0,                                    //status do botão ent
              fast_inc=0,                                  //indicador para ajuste incremental rápido
              fast_dec=0;                                  //indicador para ajuste decremental rápido

    
  if(!digitalRead(bt_esc)) st_esc=1;                       //se esc pressionado, status de esc setado
  if(!digitalRead(bt_dec)) st_dec=1;                       //se dec pressionado, status de dec setado
  if(!digitalRead(bt_inc)) st_inc=1;                       //se inc pressionado, status de inc setado
  if(!digitalRead(bt_ent)) st_ent=1;                       //se ent pressionado, status de ent setado

  if(digitalRead(bt_esc) && st_esc)                        //esc solto e status setado?
  {                                                        //sim
    st_esc = 0;                                            //limpa status
    adj    = 0;                                            //limpa indicador de ajuste de parâmetro
    inc_fl = 0;                                            //limpa flag de incremento
    dec_fl = 0;                                            //limpa flag de decremento
    lcd.clear();                                           //limpa lcd
    lcd.backlight();                                       //liga backlight do LCD
    bl_time_save = millis();                               //armazena tempo atual
    delay(150);                                            //anti-bouncing
    
  } //end if bt_esc

  if(digitalRead(bt_dec) && st_dec)                        //dec solto e status setado?
  {                                                        //sim
    st_dec = 0;                                            //limpa status
    dec_fl = 1;                                            //seta flag de decremento
    fast_dec = 0;                                          //limpa flag de ajuste rápido
    bt_time+=fast_adj;                                     //faz um "shift" no bt_time para evitar incrementos rápidos erráticos
    if(!adj) menu_num--;                                   //decrementa menu_num, se adj estiver limpa
    if(menu_num<1) menu_num=M_MAX;                         //se menu_num menor que 1, volta para o máximo
    lcd.backlight();                                       //liga backlight do LCD
    bl_time_save = millis();                               //armazena tempo atual
    delay(150);                                            //anti-bouncing
    
  } //end if bt_dec

  if(digitalRead(bt_inc) && st_inc)                        //inc solto e status setado?
  {                                                        //sim
    st_inc = 0;                                            //limpa status
    inc_fl = 1;                                            //seta flag de incremento
    fast_inc = 0;                                          //limpa flag de ajuste rápido
    bt_time+=fast_adj;                                     //faz um "shift" no bt_time para evitar incrementos rápidos erráticos
    if(!adj) menu_num++;                                   //incrementa menu_num, se adj estiver limpa
    if(menu_num>M_MAX) menu_num=1;                         //se menu_num maior que máximo, volta para 1
    lcd.backlight();                                       //liga backlight do LCD
    bl_time_save = millis();                               //armazena tempo atual
    delay(150);                                            //anti-bouncing
    
  } //end if bt_inc

  if(digitalRead(bt_ent) && st_ent)                        //ent solto e status setado?
  {                                                        //sim
    st_ent = 0;                                            //limpa status
    adj    = 1;                                            //seta indicador de ajuste de parâmetros
    inc_fl = 0;                                            //limpa flag de incremento
    dec_fl = 0;                                            //limpa flag de decremento
    lcd.clear();                                           //limpa lcd
    lcd.backlight();                                       //liga backlight do LCD
    bl_time_save = millis();                               //armazena tempo atual
    delay(150);                                            //anti-bouncing
    
  } //end if bt_ent


  if(millis()-bt_time >= fast_adj)                         //base de tempo de fast_adj ms 
  {
    
    if(st_inc) fast_inc=1;                                 //se st_inc setada, seta fast_inc
    if(st_dec) fast_dec=1;                                 //se st_dec setada, seta fast_dec
    
    bt_time = millis();                                    //atualiza bt_time cmo tempo atual
    
  } //end if millis()

  if(fast_inc)                                             //fast_inc setada?
  {                                                        //sim
    pwm_freq+=100;                                         //incrementa PWM de 100 em 100
    delayMicroseconds(1);                                  //a cada 1us
    
  } //end if fast_inc

  if(fast_dec)                                             //fast_dec setada?
  {                                                        //sim
    pwm_freq-=100;                                         //decrementa PWM de 100 em 100
    delayMicroseconds(1);                                  //a cada 1us
  } //end if fast_dec

  if(pwm_freq>78125) pwm_freq=1;                           //se frequência maior que 78.125kHz, volta para 1
  if(pwm_freq<1) pwm_freq=78125;                           //se frequência menor que 1, volta para 78.125kHz
  
 
} //end read_keyb


// =======================================================================
// --- bLight_turnOff ---
// → Controle de tempo para desligar backlight do LCD
void bLight_turnOff(uint16_t time_ms)
{
  if(millis()-bl_time_save >= time_ms)                     //monitora o tempo com millis()
  {
    lcd.noBacklight();                                     //desliga backlight ao atingir valor
  
    
  } //end if millis()
  
} //end bLight_turnOff


// =======================================================================
// --- pwm_boot ---
// → Tempo para aguardar sistema estabilizar (boot)
void pwm_boot(uint16_t boot_ms)
{
// ======================================================= //  
  lcd.backlight();                                         //liga backlight
  lcd.setCursor(0,0);                                      //posiciona cursor coluna 1, linha 1
  lcd.print("WR PWM Gen.v.1.0");                           //imprime texto WR PWM Gen.v.1.0
  lcd.setCursor(0,1);                                      //posiciona cursor coluna 1, linha 2
  
  for(int i=0;i<16;i++)                                    //executa 16 vezes...
  {
    lcd.write('.');                                        //preenche linha 2 com pontos '.'
    delay(boot_ms>>4);                                     //a cada boot_ms/16
  } //end for
  
  lcd.clear();                                             //limpa display
  lcd.noBacklight();                                       //desliga backlight

} //end pwm_boot













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




