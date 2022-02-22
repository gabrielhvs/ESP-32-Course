/* =======================================================================

   CURSO ESP32: Aprenda de Verdade!
   WR Kits: https://wrkits.com.br/ | https://space.hotmart.com/wrkits 

   Central BLE para sensor de temperatura e umidade (Bluetooth e ESP32 - Aula 2)

   ESP-WROOM-32
   Board: WEMOS LOLIN32
   Compilador: Arduino IDE 1.8.13

   Autor: Guilherme Rambo
   Data:  Março de 2021

 
======================================================================= */

#include <Arduino.h>
#include <BLEDevice.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Largura e altura da tela OLED, alterar de acordo com o modelo utilizado.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Tamanho da fonte usada para mensagens de status como "Not Connected".
#define SMALL_FONT_SIZE 1
// Tamanho a fonte usada para informacoes de temperature a umidade.
#define LARGE_FONT_SIZE 3

// Instancia do display OLED para exibir informacoes de temperatura e umidade.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pino do LED na placa. O LED fica desligado quando nao estiver conectado e ligado quando conectado.
#define LED_PIN 2

// UUIDs do servico e caracteristicas do periferico. Precisam ser iguais aos definidos no TempSensorPeripheral.
#define SERVICE_UUID "c467ea89-16b0-4314-a6e7-9ef52dfcb489"
#define TEMPERATURE_UUID "c467ea89-16b0-4314-a6e7-9ef52dfcb499"
#define HUMIDITY_UUID "c467ea89-16b0-4314-a6e7-9ef52dfcb589"

static BLEUUID serviceUUID(SERVICE_UUID);
static BLEUUID tempCharUUID(TEMPERATURE_UUID);
static BLEUUID humidityCharUUID(HUMIDITY_UUID);

// O periferico ao qual estamos nos conectando ou conectados.
static BLEAdvertisedDevice *connectedDevice;
// Caracteristicas obtidas do periferico, usadas para leitura e notificacoes.
static BLERemoteCharacteristic *tempChar = nullptr;
static BLERemoteCharacteristic *humidityChar = nullptr;

// Se estamos ou nao com uma conexao estabelecida ao periferico.
bool isConnected = false;

// Quantidade maxima de caracteres que exibiremos no display OLED.
#define DISPLAY_LIMIT 50

// Se temos ou nao um display disponivel. Este codigo pode ser rodado em uma placa
// sem display conectado, sem problemas.
bool hasDisplay = false;
// A mensagem que estamos mostrando atualmente no display.
std::string currentMessage = "Not Connected";
// Tamanho atual do texto no display.
uint8_t currentTextSize = SMALL_FONT_SIZE;
// Flag que indica que o display esta "sujo" e precisa ser atualizado no proximo loop.
bool drawDisplay = true;

// Atualiza o display com uma mensagem em texto e o tamanho de fonte especificado.
static void updateDisplay(const char *newMessage, uint8_t textSize) {
  currentMessage.assign(newMessage);
  currentTextSize = textSize;
  drawDisplay = true;
}

// Atualiza o display, formatando os valores de temperatura e umidade.
static void updateDisplay(float temperature, float humidity) {
  char msg[DISPLAY_LIMIT];
  sprintf(msg, " %.1fC\n %.0f%%", temperature, humidity);
  updateDisplay(msg, LARGE_FONT_SIZE);
}

// Ultimo valor de temperatura indicado.
float lastTemperature = 0;
// Ultimo valor de umidade indicado.
float lastHumidity = 0;

// Este callback e chamado toda vez que o periferico notifica sobre uma mudanca
// nas caracteristicas de temperatura ou umidade.
static void notificationCallback(
  BLERemoteCharacteristic *characteristic, 
  uint8_t *data, 
  size_t length, 
  bool flag
) {
  Serial.println("Received notification");

  if (characteristic == tempChar) {
    // Le informacao de temperatura recebida na notificacao e solicita
    // atualizacao do display com o novo valor.
    float *temp = (float*)data;
    Serial.printf("Temperature = %f C \n", *temp);
    
    updateDisplay(*temp, lastHumidity);
    lastTemperature = *temp;
  } else if (characteristic == humidityChar) {
    // Le informacao de umidade recebida na notificacao e solicita
    // atualizacao do display com o novo valor.
    float *humidity = (float*)data;
    Serial.printf("Humidity = %f \n", *humidity);
    
    updateDisplay(lastTemperature, *humidity);
    lastHumidity = *humidity;
  }
}

class ClientCallbacks: public BLEClientCallbacks {
  void onConnect(BLEClient *client) {
    updateDisplay("Connected", SMALL_FONT_SIZE);
  }

  void onDisconnect(BLEClient *client) {
    // Limpa o estado atual quando desconectado.
    isConnected = false;
    connectedDevice = nullptr;
    Serial.println("Connection lost");
    updateDisplay("Not Connected", SMALL_FONT_SIZE);
  }
};

// Realiza a conexao e descoberta de servicos e caracteristicas do periferico especificado.
bool connect(BLEAdvertisedDevice *device) {
  // Configura um cliente e define nossos callbacks para que sejam invocados
  // quando o cliente se conectar ou desconectar de um periferico.
  BLEClient *client = BLEDevice::createClient();
  client->setClientCallbacks(new ClientCallbacks());

  // Tenta estabelecer uma conexao.
  if (!client->connect(device)) {
    return false;
  }

  Serial.println("Connection established, finding service...");

  // Tenta obter o servico no qual estamos interessados.
  BLERemoteService *remoteService = client->getService(serviceUUID);
  if (remoteService == nullptr) {
    // Caso o periferico nao tenha o servico especificado, desconecta.
    Serial.println("Couldn't find sensor service");
    client->disconnect();
    return false;
  }

  Serial.println("Finding characteristics...");

  // Obtem caracteristicas remotas de temperatura e umidade.
  tempChar = remoteService->getCharacteristic(tempCharUUID);
  humidityChar = remoteService->getCharacteristic(humidityCharUUID);

  if (tempChar == nullptr || humidityChar == nullptr) {
    // Caso o servico nao tenha as caracteristicas que precisamos, desconecta.
    Serial.println("Couldn't find temperature or humidity characteristic");
    client->disconnect();
    return false;
  }

  // Le os valores iniciais de temperatura e umidade do periferico.
  lastTemperature = tempChar->readFloat();
  lastHumidity = humidityChar->readFloat();

  // Solicita atualizacao do display com informacoes iniciais.
  updateDisplay(lastTemperature, lastHumidity);

  Serial.println("Registering for notifications...");

  // Registra nosso callback para que sejamos notificados quando o periferico atualizar
  // as caracteristicas de temperatura e umidade.
  tempChar->registerForNotify(notificationCallback);
  humidityChar->registerForNotify(notificationCallback);

  return true;
}

class AdvertisedCallbacks: public BLEAdvertisedDeviceCallbacks {
  // Callback chamado quando nosso scan encontra um periferico fazendo advertising.
  void onResult(BLEAdvertisedDevice device) {
    // So estamos interessados em perifericos que tenham o nosso servico no advertising.
    if (device.haveServiceUUID() && device.isAdvertisingService(serviceUUID)) {
      Serial.printf("Found device: %s \n", device.toString().c_str());

      // Para o scan quando encontramos um periferico compativel.
      BLEDevice::getScan()->stop();

      // Armazena uma copia do periferico encontrado para conexao no loop.
      connectedDevice = new BLEAdvertisedDevice(device);
    }
  }
};

void setup() {
  Serial.begin(9600);

  // Configuracao do display OLED (neste caso, display built-in de placa WEMOS LOLIN32).
  Wire.begin(5, 4);

  // Inicializa o display e salva a informacao de se ha ou nao um display disponivel,
  // para que nossa central possa funcionar com ou sem um display.
  hasDisplay = display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false);
  if(!hasDisplay) {
    Serial.println("Failed to initialize SSD1306 display!");
  } else {
    Serial.println("Display initialized");

    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(SMALL_FONT_SIZE);
    delay(100);
  }

  // Inicializa o dispositivo BLE.
  BLEDevice::init("");

  // Configurao o LED como saida para que possamos acende-lo quando conectados.
  pinMode(LED_PIN, OUTPUT);

  // Cria um scan e configura para que use nosso callback.
  BLEScan *scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new AdvertisedCallbacks());
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(true);

  Serial.println("Scanning...");
}

void loop() {
  // Se tivermos um display e ele estiver precisando ser atualizado, atualizamos
  // com as informacoes mais recentes.
  if (drawDisplay && hasDisplay) {
    drawDisplay = false;
    
    display.clearDisplay();

    display.setTextSize(currentTextSize);
    display.setCursor(0, 0);
    
    display.println(F(currentMessage.c_str()));

    display.display();
  }

  if (connectedDevice != nullptr && !isConnected) {
    // Se houver um periferico no qual estamos interessados, mas nao estivermos conectados ainda.
    Serial.println("Connecting...");

    // Tenta uma conexao com o periferico.
    if (connect(connectedDevice)) {
      // Se a conexao der certo, liga o LED na placa e reflete na flag isConnected.
      Serial.println("Connected!");
      digitalWrite(LED_PIN, HIGH);
      isConnected = true;
    } else {
      // Se a conexao der errado, descarta o periferico e reflete na flag isConnected.
      Serial.println("Connection failed!");
      connectedDevice = nullptr;
      isConnected = false;
    }
  }

  if (connectedDevice == nullptr && !isConnected) {
      // Se nao houver um periferico no qual estamos interessados e nao estivermos conectados,
      // desliga o LED na placa e reinicia o scan para buscar perifericos.
      digitalWrite(LED_PIN, LOW);
      BLEDevice::getScan()->start(0);
  }

  // Nosso loop nao precisa rodar mais rapido que a cada segundo,
  // mas se formos realizar outras tarefas neste firmware,
  // talvez seja necessario remover este delay e utilizar millis().
  delay(1000);
}