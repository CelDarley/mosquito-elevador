#include <WiFi.h>
#include <PubSubClient.h>

// Configurações do WiFi
const char* ssid = "catena";
const char* password = "Lolina2501";

// Configurações do MQTT
const char* mqtt_server = "10.100.0.21"; // IP do computador rodando o broker
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/acionar";
const int ledPin = 4;

WiFiClient espClient;
PubSubClient client(espClient);

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
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println(message);
  
  // Verifica se a mensagem é "ON" ou "0" para ligar o LED
  if(message == "ON" || message == "0") {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED LIGADO");
  }
  // Verifica se a mensagem é "OFF" ou "1" para desligar o LED
  else if(message == "OFF" || message == "1") {
    digitalWrite(ledPin, LOW);
    Serial.println("LED DESLIGADO");
  }
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

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Inicia com o LED desligado
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
} 