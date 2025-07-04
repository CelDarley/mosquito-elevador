#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <WebServer.h>

// Estrutura para armazenar configurações
struct Config {
  char ssid[32];
  char password[64];
  bool configurado;
};

// Endereço na EEPROM para as configurações
#define CONFIG_ADDRESS 0
Config config;

// Configurações do MQTT
const char* mqtt_server = "mosquitto.catenasystem.com.br";
const int mqtt_port = 8883;
const char* mqtt_topic = "esp32/acionar";
const int rl1 = 4;
const int rl2 = 16;

// Configurações do Access Point
const char* ap_ssid = "ESP32-Config";
const char* ap_password = "12345678";
IPAddress ap_ip(192, 168, 4, 1);
IPAddress ap_gateway(192, 168, 4, 1);
IPAddress ap_subnet(255, 255, 255, 0);

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);
DNSServer dnsServer;

bool modoAP = false;

// Pino do botão de reset
const int BUTTON_PIN = 0; // GPIO0 (botão BOOT/EN)
unsigned long lastButtonPress = 0;
const unsigned long RESET_DELAY = 5000; // 5 segundos para reset

// Variável para controlar o estado dos relés
bool relesAtivos = false;

// Pino do LED
const int LED_PIN = 19;

// Função para salvar configurações na EEPROM
void salvarConfig() {
  EEPROM.put(CONFIG_ADDRESS, config);
  EEPROM.commit();
  Serial.println("Configurações salvas na EEPROM");
}

// Função para carregar configurações da EEPROM
void carregarConfig() {
  EEPROM.get(CONFIG_ADDRESS, config);
  if (!config.configurado) {
    Serial.println("Nenhuma configuração encontrada na EEPROM");
    modoAP = true;
  } else {
    Serial.println("Configurações carregadas da EEPROM:");
    Serial.print("SSID: ");
    Serial.println(config.ssid);
  }
}

// Função para testar o LED
void testarLED() {
  Serial.println("\nTestando LED...");
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}

// Página HTML para configuração
const char* html_page = R"(
<!DOCTYPE html>
<html>
<head>
  <title>Configuração ESP32</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      background: #f0f0f0;
    }
    .container {
      max-width: 400px;
      margin: 0 auto;
      background: white;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    h1 {
      color: #333;
      text-align: center;
      margin-bottom: 20px;
    }
    .form-group {
      margin-bottom: 15px;
    }
    label {
      display: block;
      margin-bottom: 5px;
      color: #666;
    }
    input {
      width: 100%;
      padding: 8px;
      border: 1px solid #ddd;
      border-radius: 4px;
      box-sizing: border-box;
    }
    input[type="submit"] {
      width: 100%;
      padding: 10px;
      background: #4CAF50;
      color: white;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    input[type="submit"]:hover {
      background: #45a049;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Configuração do Dispositivo</h1>
    <form action="/config" method="post">
      <div class="form-group">
        <label for="ssid">Nome da Rede WiFi:</label>
        <input type="text" id="ssid" name="ssid" required>
      </div>
      <div class="form-group">
        <label for="password">Senha do WiFi:</label>
        <input type="password" id="password" name="password" required>
      </div>
      <input type="submit" value="Salvar Configuração">
    </form>
  </div>
</body>
</html>
)";

void handleRoot() {
  Serial.println("Recebida requisição GET para /");
  server.send(200, "text/html", html_page);
}

void handleConfig() {
  Serial.println("Recebida requisição POST para /config");
  
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    Serial.print("SSID recebido: ");
    Serial.println(ssid);
    Serial.print("Senha recebida: ");
    Serial.println(password);
    
    if (ssid.length() > 0 && password.length() > 0) {
      strncpy(config.ssid, ssid.c_str(), sizeof(config.ssid) - 1);
      strncpy(config.password, password.c_str(), sizeof(config.password) - 1);
      config.ssid[sizeof(config.ssid) - 1] = '\0';
      config.password[sizeof(config.password) - 1] = '\0';
      config.configurado = true;
      
      Serial.println("Salvando configurações na EEPROM...");
      salvarConfig();
      
      server.send(200, "text/html; charset=utf-8", 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<style>"
        "body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }"
        "h1 { color: #4CAF50; }"
        "</style>"
        "</head>"
        "<body>"
        "<h1>Configuração Salva!</h1>"
        "<p>O ESP32 irá reiniciar e conectar ao WiFi configurado.</p>"
        "<p>Você pode desconectar do ESP32-Config e conectar à sua rede WiFi.</p>"
        "</body>"
        "</html>"
      );
      
      Serial.println("Reiniciando ESP32...");
      delay(2000);
      ESP.restart();
    } else {
      Serial.println("Erro: SSID ou senha vazios");
      server.send(400, "text/html; charset=utf-8", 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<style>"
        "body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }"
        "h1 { color: #f44336; }"
        "</style>"
        "</head>"
        "<body>"
        "<h1>Erro</h1>"
        "<p>SSID e senha não podem estar vazios.</p>"
        "<p><a href='/'>Voltar</a></p>"
        "</body>"
        "</html>"
      );
    }
  } else {
    Serial.println("Erro: Parâmetros inválidos");
    server.send(400, "text/html; charset=utf-8", 
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<meta charset='UTF-8'>"
      "<meta name='viewport' content='width=device-width, initial-scale=1'>"
      "<style>"
      "body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }"
      "h1 { color: #f44336; }"
      "</style>"
      "</head>"
      "<body>"
      "<h1>Erro</h1>"
      "<p>Parâmetros inválidos.</p>"
      "<p><a href='/'>Voltar</a></p>"
      "</body>"
      "</html>"
    );
  }
}

void iniciarAP() {
  Serial.println("\n=== INICIANDO ACCESS POINT ===");
  
  // Forçar desconexão e limpeza de qualquer estado anterior
  WiFi.disconnect(true);
  delay(1000);
  
  // Forçar modo AP
  WiFi.mode(WIFI_AP_STA);
  delay(1000);
  
  // Configurar IP do AP
  if (!WiFi.softAPConfig(ap_ip, ap_gateway, ap_subnet)) {
    Serial.println("ERRO: Falha ao configurar IP do AP");
    ESP.restart(); // Reinicia o ESP32 em caso de erro
    return;
  }
  
  // Iniciar AP com canal específico e visibilidade máxima
  if (!WiFi.softAP(ap_ssid, ap_password, 1, 0, 4)) {
    Serial.println("ERRO: Falha ao iniciar AP");
    ESP.restart(); // Reinicia o ESP32 em caso de erro
    return;
  }
  
  Serial.println("\nAccess Point iniciado com sucesso!");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("Senha: ");
  Serial.println(ap_password);
  Serial.print("IP do AP: ");
  Serial.println(ap_ip);
  
  // Configurar servidor DNS
  dnsServer.start(53, "*", ap_ip);
  
  // Configurar rotas do servidor web
  server.on("/", handleRoot);
  server.on("/config", HTTP_POST, handleConfig);
  
  server.begin();
  Serial.println("Servidor web iniciado");
  
  // LED piscando lentamente no modo AP
  static unsigned long lastLedChange = 0;
  if (millis() - lastLedChange > 1000) {
    lastLedChange = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}

void setup_wifi() {
  if (modoAP) {
    iniciarAP();
    return;
  }
  
  Serial.println("\n=== CONECTANDO AO WIFI ===");
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.password);
  
  Serial.print("SSID: ");
  Serial.println(config.ssid);
  
  // LED piscando rápido durante tentativa de conexão
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(250);
    Serial.print(".");
    tentativas++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado com sucesso!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Máscara de sub-rede: ");
    Serial.println(WiFi.subnetMask());
    
    // LED ligado continuamente quando conectado
    digitalWrite(LED_PIN, HIGH);
    
    // Configurar e conectar ao MQTT
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    
    Serial.println("\nTentando conectar ao servidor MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado ao servidor MQTT!");
      client.subscribe(mqtt_topic);
      Serial.print("Inscrito no tópico: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("Falha na conexão MQTT, rc=");
      Serial.println(client.state());
    }
  } else {
    Serial.println("Falha ao conectar ao WiFi. Iniciando modo AP...");
    modoAP = true;
    iniciarAP();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.println(message);
  
  // Processar o comando
  processarComando(message);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando reconectar ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void processarComando(const char* comando) {
  if (strcmp(comando, "ON") == 0) {
    digitalWrite(rl1, HIGH);
    digitalWrite(rl2, HIGH);
    relesAtivos = true;
    Serial.println("Relés LIGADOS");
  } else if (strcmp(comando, "OFF") == 0) {
    digitalWrite(rl1, LOW);
    digitalWrite(rl2, LOW);
    relesAtivos = false;
    Serial.println("Relés DESLIGADOS");
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000); // Aguardar inicialização do Serial
  
  Serial.println("\n=== INICIANDO ESP32 ===");
  
  // Configurar pinos
  pinMode(rl1, OUTPUT);
  pinMode(rl2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Inicializar relés e LED
  digitalWrite(rl1, LOW);
  digitalWrite(rl2, LOW);
  digitalWrite(LED_PIN, LOW);
  
  // Testar LED
  testarLED();
  
  EEPROM.begin(512);
  carregarConfig();
  
  setup_wifi();
  
  Serial.println("ESP32 iniciado e aguardando conexões...");
}

void loop() {
  // Verificar botão de reset
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (millis() - lastButtonPress > RESET_DELAY) {
      Serial.println("Reset de fábrica iniciado...");
      config.configurado = false;
      salvarConfig();
      ESP.restart();
    }
  } else {
    lastButtonPress = millis();
  }
  
  if (modoAP) {
    dnsServer.processNextRequest();
    server.handleClient();
    
    // LED piscando lentamente no modo AP
    static unsigned long lastLedChange = 0;
    if (millis() - lastLedChange > 1000) {
      lastLedChange = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  } else {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    
    // Verificar status do WiFi periodicamente
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 30000) { // Verificar a cada 30 segundos
      lastCheck = millis();
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Conexão WiFi perdida. Reconectando...");
        digitalWrite(LED_PIN, LOW); // LED apagado durante reconexão
        setup_wifi();
      }
    }
  }
} 