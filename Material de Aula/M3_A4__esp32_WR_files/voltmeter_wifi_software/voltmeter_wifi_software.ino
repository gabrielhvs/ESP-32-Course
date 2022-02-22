/* =======================================================================

   CURSO ESP32: Aprenda de Verdade!
   WR Kits: https://wrkits.com.br/ | https://space.hotmart.com/wrkits 

   Voltímetro WiFi: envia valor de tensão do ESP para Web.
   A tensão será a leitura do nível de uma bateria de 15V hipotética.
   Atualiza a página automaticamente com JavaScript.

   Utiliza servidor Web assíncrono (pode-se gerenciar mais de uma
   conexão ao mesmo tempo, entre outras vantagens).


   DICA: Selecionar ícones para o seu projeto em:
   https://fontawesome.com/icons 
   
   ESP-WROOM-32
   Board: DevKitV1
   Compilador: Arduino IDE 1.8.4

   Autor: Eng. Wagner Rambo
   Data:  Março de 2021

 
======================================================================= */


// =======================================================================
// --- Bibliotecas Auxiliares ---
#include <WiFi.h>                         //inclui biblioteca WiFi
#include <ESPAsyncWebServer.h>            //inclui biblioteca para gerar Web Server Assíncrono


// =======================================================================
// --- Mapeamento de Hardware ---
#define   an_pin   36                    //entrada analógica para leitura de tensão


// =======================================================================
// --- Constantes Auxialires ---
const char* ssid     = "NOME_DA_SUA_REDE";    //nome da sua rede wifi
const char* password = "SENHA_DA_SUA_REDE";   //senha da sua rede wifi


// =======================================================================
// --- Objetos ---
AsyncWebServer server(80);              //cria objeto para o servidor assíncrono na porta padrão 80


// =======================================================================
// --- Protótipo das Funções ---
float meter();                                          //mede tensão, calcula e retorna o valor
long moving_average(unsigned content, const int n);     //realiza média móvel de content, com n pontos 
String readAnalog();                                    //função para tratar a leitura e retornar string
String update_v(const String &num);                     //função de atualização


// =======================================================================
// --- Página HTML ---
const char html_page[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML><html>
<head>
  <title>WR Kits Web</title>
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css">
  <style> 
    html 
    {
      font-family: Tahoma;
      display: inline-block;
      margin: 0px auto;
      text-align: center;
    }
  
    h2 
    { 
      font-size: 45px; 
  
    }
    
    p 
    { 
      font-size: 40px; 
    
    }
    
    .volt_un 
    { 
      font-size: 30px; 
    
    }
    .volt_disp
    {
      font-size: 35px;
     
    }
  </style>
</head>

<body>
  <h2>WR Kits Voltmeter ESP32</h2>
  
  <p>
   
  <i class="fas fa-battery-half" style="color:#12237b"></i>
    <span class="volt_disp">Battery Voltage: </span> 
    <span id="voltage">%VOLTS%</span>
    <span class="volt_un">V</span>
  
  </p>

</body>

<script> 

setInterval(
function() 
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() 
  {
    if (this.readyState == 4 && this.status == 200)
      document.getElementById("voltage").innerHTML = this.responseText;
  };
  xhttp.open("GET", "/voltage", true);
  xhttp.send();
}, 1500 );

</script>
</html>

)rawliteral";




// =======================================================================
// --- Configurações Iniciais ---
void setup()
{
 
  Serial.begin(115200);                  //inicializa Serial em 115200 baud rate
 
  Serial.println();                      //
  Serial.print("Conectando-se a ");      //
  Serial.println(ssid);                  //
  WiFi.begin(ssid, password);            //inicializa WiFi, passando o nome da rede e a senha

  while(WiFi.status() != WL_CONNECTED)   //aguarda conexão (WL_CONNECTED é uma constante que indica sucesso na conexão)
  {
    delay(741);                          //
    Serial.print(".");                   //vai imprimindo pontos até realizar a conexão...
  }

  Serial.println("");                    //mostra WiFi conectada
  Serial.println("WiFi conectada");      //
  Serial.println("Endereço de IP: ");    //
  Serial.println(WiFi.localIP());        //mostra o endereço IP

  //Atualiza Web Server Assíncrono
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
           {  request->send_P(200, "text/html", html_page, update_v);  } );
             
  server.on("/voltage", HTTP_GET, [](AsyncWebServerRequest *request)
           {  request->send_P(200, "text/plain", readAnalog().c_str());} );
 

  server.begin();                        //inicializa o servidor web
  
} //end setup


// =======================================================================
// --- Loop Infinito ---
void loop()
{
  
} //end loop


// =======================================================================
// --- Desenvolvimento das Funções ---


// =================================================================================
// --- meter ---
// → calcula tensão em V
// → retorna valor
float meter()
{      
  static float volts_f = 0.0;                           //variável estática para armazenar tensão em ponto flutuante
              
  volts_f  = moving_average(analogRead(an_pin), 3) * 0.0008058;   //calcula tensão a partir da média móvel
  volts_f *= 4.5454;                                              //normaliza para 15V (3,3 x 4,5454)

  return volts_f;                                       //retorna valor de tensão

} //end meter


// =================================================================================
// --- moving_average ---
// → recebe um valor para filtrar e uma constante de pontos de média móvel "n"
// → calcula a média móvel até 100 pontos
long moving_average(unsigned content, const int n)
{

   long acc = 0;                                        //acumulador para somar os pontos da média móvel
   static int numbers[100];                             //vetor com os valores para média móvel
 
   for(int i= n-1; i>0; i--) numbers[i] = numbers[i-1]; //desloca os elementos do vetor de média móvel

   numbers[0] = content;                                //posição inicial do vetor recebe a leitura original

   for(int i=0; i<n; i++) acc += numbers[i];            //faz a somatória do número de pontos

   return acc/n;                                        //retorna a média móvel

} //end moving_average


// =================================================================================
// --- readAnalog ---
// → lê valor de tensão e converte para string
String readAnalog()
{
   float adc = meter();                                 //captura tensão 

   return String(adc);                                  //retorna em formato string
  
} //end readAnalog


// =================================================================================
// --- update_v ---
// → compara string a ser tratada, retorna valor
String update_v(const String &num)
{
  
  if(num == "VOLTS")                                   //é a string "VOLTS"?
  return readAnalog();                                 //retorna leitura
  
} //end update_v

















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












