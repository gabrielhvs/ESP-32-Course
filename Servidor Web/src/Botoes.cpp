#include <Arduino.h>

// =======================================================================
// --- Bibliotecas Auxiliares ---
#include <WiFi.h>                         //inclui biblioteca WiFi


// =======================================================================
// --- Mapeamento de Hardware ---
#define   relay1   33                    //pino para controle do relé 1
#define   relay2   32                    //pino para controle do relé 2
#define   buzzer   25                    //pino para controle do buzzer


// =======================================================================
// --- Constantes Auxialires ---
const char *ssid     = "brisa-1460064";  //atribuir nome da rede WiFi
const char *password = "1mcswgje"; //atribuir senha da rede


// =======================================================================
// --- Objetos ---
WiFiServer server(80);                    //define a porta que o servidor irá utilizar
                                          //(normalmente utiliza-se a porta padrão 80)

// =======================================================================
// --- Protótipo das Funções ---
void relay_wifi();                       //função para gerar web server e controlar os relés
                                


// =======================================================================
// --- Variáveis Globais ---
String header;


// Auxiliar variables to store the current output state
String RL1_status = "OFF";
String RL2_status = "OFF";
String BZ1_status = "OFF";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


// =======================================================================
// --- Configurações Iniciais ---
void setup() 
{
  Serial.begin(115200);                  //inicializa Serial em 115200 baud rate
  pinMode(relay1, OUTPUT);               //configura saída para relé 1
  pinMode(relay2, OUTPUT);               //configura saída para relé 2
  pinMode(buzzer, OUTPUT);               //configura saída para o buzzer

  digitalWrite(relay1, LOW);             //relé 1 desligado
  digitalWrite(relay2, LOW);             //relé 2 desligado
  digitalWrite(buzzer, LOW);             //buzzer desligado

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

  server.begin();                        //inicializa o servidor web

}


// =======================================================================
// --- Loop Infinito ---
void loop() 
{


  while(WiFi.status() != WL_CONNECTED)   //aguarda conexão (WL_CONNECTED é uma constante que indica sucesso na conexão)
  {
    WiFi.begin(ssid, password);          //inicializa WiFi, passando o nome da rede e a senha
    Serial.print(".");                   //vai imprimindo pontos até realizar a conexão...

    delay(741);                          //741ms
  }
  
  relay_wifi();                                //chama função para controle dos relés por wifi


  
} //end loop


// =======================================================================
// --- relay_wifi ---
void relay_wifi()
{

  WiFiClient client = server.available();      //verifica se existe um cliente conectado com dados a serem transmitidos
 
  if(client)                                   //existe um cliente?
  {                                            //sim                           
    currentTime = millis();                    //atualiza o tempo atual
    previousTime = currentTime;                //armazena tempo atual
    Serial.println("Novo cliente definido");   //informa por serial
    String currentLine = "";                   //string para aguardar entrada de dados do cliente
    
    while(client.connected() && currentTime - previousTime <= timeoutTime)  //executa enquanto cliente conectado
    {   
      currentTime = millis();                  //atualiza tempo atual
      
      if(client.available())                   //existem dados do cliente a serem lidos?
      {                                        //sim
        char c = client.read();                //salva em c
        Serial.write(c);                       //imprime via serial
        header += c;                           //acumula dados do cliente em header
        
        if (c == '\n')                         //é um caractere de nova linha?
        {                                      //sim           
         
          if (currentLine.length() == 0)       //se final da mensagem...
          {
           
            client.println("HTTP/1.1 200 OK");        //HTTP sempre inicia com este código de resposta
            client.println("Content-type:text/html"); //e com este conteúdo sabermos o que está por vir
            client.println();                         //imprime nova linha

            // Controle das Saídas do ESP32:
            if(header.indexOf("GET /rl1/on") >= 0)         //liga Relé 1
            {
              RL1_status = "ON";                           //atualiza status
              digitalWrite(relay1, HIGH);                  //ativa saída
              
            } //end if RL1 ON
            
            else if(header.indexOf("GET /rl1/off") >= 0)   //desliga Relé 1
            {          
              RL1_status = "OFF";                          //atualiza status
              digitalWrite(relay1, LOW);                   //desativa saída
              
            } //end else if RL1 OFF
             
            else if(header.indexOf("GET /rl2/on") >= 0)    //liga Relé 2
            {            
              RL2_status = "ON";                           //atualiza status
              digitalWrite(relay2, HIGH);                  //ativa saída
              
            } //end else if RL2 ON
            
            else if(header.indexOf("GET /rl2/off") >= 0)   //desliga Relé 2
            {
              RL2_status = "OFF";                          //atualiza status
              digitalWrite(relay2, LOW);                   //desativa saída
            
            } //end if RL2 OFF
            
            else if(header.indexOf("GET /bz1/on") >= 0)   //liga buzzer 1
            { 
              BZ1_status = "ON";                          //atualiza status
              digitalWrite(buzzer, HIGH);                 //ativa saída
             
            } //end else if BZ1 ON
            
            else if(header.indexOf("GET /bz1/off") >= 0)  //desliga buzzer 1
            {
              BZ1_status = "OFF";                         //atualiza status
              digitalWrite(buzzer, LOW);                  //desativa saída
             
            } //end else if BZ1 OFF
             
            
            //Gera a página HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
           
            client.println("<style>html { font-family: Tahoma; margin: 0px auto; text-align: center; background-color: #FEFFFF;}");
            client.println(".bts { border: none; color: #FFFFFF; padding: 14px 36px;");
            client.println("text-decoration: none; font-size: 23px; margin: 1.5px; cursor: pointer; font-family: Verdana;}");
            client.println(".btOn  { background-color: #3F459E; }");
            client.println(".btOff { background-color: #606060; }");
            client.println("</style>");
            client.println("<title>WR Kits Web</title></head>");
            
            client.println("<body><h1>WR Kits ESP32 Web Server</h1>");
            
            //Imprime status atual do Relé 1
            client.println("<p>RELAY 1 Status: " + RL1_status + "</p>");

            //Gera o botão conforme o status do Relé 1
            if(RL1_status=="OFF") 
              client.println("<p><a href=\"/rl1/on\"><button class=\"bts btOn\">TURN ON RL1</button></a></p>");
            else 
              client.println("<p><a href=\"/rl1/off\"><button class=\"bts btOff\">TURN OFF RL1</button></a></p>");
         
            //Imprime o status atual do Relé 2
            client.println("<p>RELAY 2 Status: " + RL2_status + "</p>");

            //Gera o botão conforme o status do Relé 2
            if(RL2_status=="OFF")
              client.println("<p><a href=\"/rl2/on\"><button class=\"bts btOn\">TURN ON RL2</button></a></p>");
            else 
              client.println("<p><a href=\"/rl2/off\"><button class=\"bts btOff\">TURN OFF RL2</button></a></p>");
            
            //Imprime o status atual do Buzzer 1
            client.println("<p>BUZZER Status: " + BZ1_status + "</p>");

            //Gera o botão conforme o status do Buzzer 1
            if(BZ1_status=="OFF") 
              client.println("<p><a href=\"/bz1/on\"><button class=\"bts btOn\">PULSE BUZZ1</button></a></p>");
            else
              client.println("<p><a href=\"/bz1/off\"><button class=\"bts btOff\">ALERT BUZZ1</button></a></p>");


            client.println("</body></html>");
            client.println();
            break;
          } 
          
          else currentLine = ""; //senão, impede string de fizer com espaços em branco
            
        } //end if c
        
        else if (c != '\r')  
          currentLine += c;      //adiciona caractere como parte da mensage
 

      } //end if client.available
      
    } //end while client.connected
    
    header = "";                              //limpa header
   
    client.stop();                            //finaliza conexão
    Serial.println("Cliente desconectado");   //
    Serial.println("");                       //
    
  } //end if client
  
  
} //end relay_wifi











