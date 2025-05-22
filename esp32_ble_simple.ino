#include <WiFi.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Configurações do WiFi
const char* ssid = "catena";
const char* password = "Lolina2501";

// Configurações do MQTT
const char* mqtt_server = "10.100.0.21";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/acionar";

// Definição dos pinos dos relés
const int rl1 = 4;
const int rl2 = 16;

// UUIDs para o serviço e característica BLE
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

WiFiClient espClient;
PubSubClient client(espClient);

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem MQTT recebida [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  
  Serial.println(message);
  processarComando(message);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Dispositivo BLE conectado");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Dispositivo BLE desconectado");
      pServer->getAdvertising()->start();
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // Obter o valor como um array de bytes
      uint8_t* data = pCharacteristic->getData();
      size_t length = pCharacteristic->getLength();
      
      if (length > 0) {
        // Criar um buffer temporário para a string
        char temp[length + 1];
        
        // Copiar os dados para o buffer
        for (int i = 0; i < length; i++) {
          temp[i] = (char)data[i];
        }
        temp[length] = '\0';  // Adicionar terminador nulo
        
        Serial.print("Comando BLE recebido: ");
        Serial.println(temp);
        
        // Processar o comando
        processarComando(temp);
      }
    }
};

void processarComando(const char* comando) {
  if (strcmp(comando, "ON") == 0) {
    digitalWrite(rl1, HIGH);
    digitalWrite(rl2, HIGH);
    Serial.println("Relés LIGADOS");
  } else if (strcmp(comando, "OFF") == 0) {
    digitalWrite(rl1, LOW);
    digitalWrite(rl2, LOW);
    Serial.println("Relés DESLIGADOS");
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(rl1, OUTPUT);
  pinMode(rl2, OUTPUT);
  digitalWrite(rl1, LOW);
  digitalWrite(rl2, LOW);

  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  BLEDevice::init("ESP32-Elevador");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("ESP32 iniciado e aguardando conexões...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
} 