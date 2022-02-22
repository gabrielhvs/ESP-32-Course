#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

// Pino que está conectado ao DIN da matriz de LEDs.
#define PIN 4
// Quantidade de LEDs na matriz.
#define NUMPIXELS 16

// Instância do NeoPixel utilizada para controle da matriz de LEDs.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUID do serviço de matriz de LEDs.
#define SERVICE_UUID "0f5f7ca0-0fab-4743-aab2-105efc3ff3ea"
// Característica que recebe comandos para alterar a matriz de LEDs (ver protocolo.md).
#define PIXELS_CHARACTERISTIC_UUID "274b22a7-c4b8-403e-8780-bc6cb835ddf9"

// Comandos (ver protocolo.md)

// Altera a cor de um LED específico.
#define COMMAND_SET_LED 0
// Altera o brilho de todos os LEDs.
#define COMMAND_SET_BRIGHTNESS 1
// Apaga todos os LEDs.
#define COMMAND_CLEAR 2
// Alterna entre LEDs piscando ou não.
#define COMMAND_TOGGLE_BLINK 3
// Altera a cor de todos os LEDs de uma só vez.
#define COMMAND_SET_ALL_LEDS 4

// Se há alguma central conectada atualmente ao nosso periférico.
bool deviceConnected = false;
// Se o efeito de piscar está atualmente ativo.
bool blinkingEffect = false;
// Brilho atual da matriz de LEDs.
uint8_t brightness = 0xFF;

// Funções que respondem aos comandos escritos na característica do periférico, implementadas mais abaixo.

void handleSetLEDCommand(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
void handleSetBrightnessCommand(uint8_t newBrightness);
void handleSetAllLEDsCommand(uint8_t r, uint8_t g, uint8_t b);

// Callbacks da característica que recebe os comandos da central.
class LEDMatrixCharacteristicCallbacks: public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *characteristic) {
    // Obtém um ponteiro para os dados escritos na característica.
    uint8_t *data = characteristic->getData();
    
    // O primeiro byte represente o comando a ser executado.
    uint8_t command = data[0];

    Serial.print("Received command ");
    Serial.println(command, HEX);

    // Executa o comando especificado no primeiro byte.
    switch (command) {
    case COMMAND_SET_LED:
    {
    // O segundo byte no comando "Set LED" é o número do LED a ser alterado.
    uint8_t led = data[1];
    // Os bytes subsequentes representam a cor, em RGB.
    uint8_t red = data[2];
    uint8_t green = data[3];
    uint8_t blue = data[4];
    // Chama a função que aplica o comando, já com os dados prontos.
    handleSetLEDCommand(led, red, green, blue);
    }
    break;
    
    case COMMAND_SET_BRIGHTNESS:
    // Apenas chama a função com o segundo byte do comando, que representa o brilho a ser usado.
    handleSetBrightnessCommand(data[1]);
    break;

    case COMMAND_CLEAR:
    // Limpa matriz de LEDs e reseta o brilho para o máximo.
    pixels.clear();
    handleSetBrightnessCommand(255);
    break;

    case COMMAND_TOGGLE_BLINK:
    // Inverte a flag blinkingEffect, que é lida no loop() para piscar ou não os LEDs.
    blinkingEffect = !blinkingEffect;
    break;

    case COMMAND_SET_ALL_LEDS:
    {
    // O comando para alterar todos os LEDs envia a cor RGB a partir do byte 1.
    uint8_t red = data[1];
    uint8_t green = data[2];
    uint8_t blue = data[3];
    handleSetAllLEDsCommand(red, green, blue);
    }
    break;
    
    default:
      break;
    }
  }

};

// Callbacks usados para controlar o status de conexão/desconexão.
class ServerCallbacks: public BLEServerCallbacks {

  int connectedCount = 0;
  
  void onConnect(BLEServer *server) {
    Serial.println("Connected! Waiting for commands...");

    if (connectedCount <= 0) {
      Serial.println("First device connected, clearing matrix");
      pixels.clear();
      deviceConnected = true;
    }
    
    connectedCount++;

    // Reiniciamos o advertising após uma conexão para que mais de uma central
    // possa encontrar nosso periférico e se conectar a ele simultaneamente.
    BLEDevice::startAdvertising();
  }

  void onDisconnect(BLEServer *server) {
    Serial.println("Disconnected");

    connectedCount--;

    if (connectedCount <= 0) {
      Serial.println("All devices disconnected");

      // Se todas as centrais desconectaram, limpa a matriz de LEDs e reseta o estado.
      pixels.clear();
      deviceConnected = false;  
    }
  }

};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  // Configuração inicial da matriz de LEDs, completamente apagada.
  pixels.begin();
  pixels.clear();

  // Inicialização do periférico BLE e configuração dos callbacks.
  BLEDevice::init("LEDMatrix");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService *service = server->createService(SERVICE_UUID);
  
  // Esta é a característica que centrais modificarão para enviar comandos que modificam
  // a matriz de LEDs. Ela só pode ser escrita, não lida.
  BLECharacteristic *characteristic = service->createCharacteristic(
    PIXELS_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE_NR
  );

  // Registra nossa classe de callbacks na característica para receber dados escritos por centrais.
  characteristic->setCallbacks(new LEDMatrixCharacteristicCallbacks());

  // Torna o serviço ativo.
  service->start();

  // Começa a fazer o advertising do periférico, incluindo o UUID do serviço no advertising.
  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);

  advertising->setMinPreferred(0x06);
  
  BLEDevice::startAdvertising();

  Serial.println("Advertising!");
}

// Cor padrão, usada quando não há central conectada.
#define NOT_CONNECTED_COLOR 20
// Cor atual da matriz de LEDs, quando não há central conectada e ela está piscando.
uint8_t blinkColor = 0;
// Brilho atual da matriz de LEDs.
uint8_t currentBrightness = 0;
// Último brilho registrado.
uint8_t lastBrightness = 0;

void handleSetLEDCommand(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
  // Modifica o LED especificado com a cor recebida.
  pixels.setPixelColor(led, pixels.Color(r, g, b));
  pixels.show();
}

void handleSetBrightnessCommand(uint8_t newBrightness)
{
  // Atualiza o brilho para o brilho recebido, ele será lido no loop().
  brightness = newBrightness;
}

void handleSetAllLEDsCommand(uint8_t r, uint8_t g, uint8_t b)
{
  // Preenche a matriz de LEDs inteira com a cor especificada.
  pixels.fill(pixels.Color(r, g, b));
  // Após uso de fill é necessário resetar o brilho para que os LEDs acendam.
  pixels.setBrightness(brightness);
  pixels.show();
}

void loop() {
  if (!deviceConnected) {
    // Pisca os 4 LEDs centrais quando não conectado.
    blinkColor = (blinkColor == NOT_CONNECTED_COLOR) ? 0 : NOT_CONNECTED_COLOR;

    pixels.setPixelColor(5, pixels.Color(blinkColor, blinkColor, blinkColor));
    pixels.setPixelColor(6, pixels.Color(blinkColor, blinkColor, blinkColor));
    pixels.setPixelColor(9, pixels.Color(blinkColor, blinkColor, blinkColor));
    pixels.setPixelColor(10, pixels.Color(blinkColor, blinkColor, blinkColor));

    pixels.show();

    delay(500);
  } else {
    if (blinkingEffect) {
      // Controla o efeito de pisca que pode ser ativado pela central com um comando.
      currentBrightness = (currentBrightness == brightness) ? 0x01 : brightness;
      
      Serial.print("currentBrigthness = ");
      Serial.println(currentBrightness, HEX);

      pixels.setBrightness(currentBrightness);
      pixels.show();
      delay(500);
    } else {
      // Atualiza o brilho da matriz de LEDs caso o brilho requisitado seja diferente do atual.
      if (brightness != lastBrightness) {
        Serial.print("Setting brightness to ");
        Serial.println(brightness, HEX);

        pixels.setBrightness(brightness);
        pixels.show();

        lastBrightness = brightness;
        currentBrightness = brightness;
      }
    }
  }
}