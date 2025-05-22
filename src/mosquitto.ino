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

// Declarações antecipadas das funções
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void salvarConfig();
void carregarConfig();
void iniciarAP();
void handleRoot();
void handleConfig();

// Pino do botão de reset
const int BUTTON_PIN = 0; // GPIO0 (botão BOOT/EN)
unsigned long lastButtonPress = 0;
const unsigned long RESET_DELAY = 5000; // 5 segundos para reset

// Variável para controlar o estado dos relés
bool relesAtivos = false;

// Função para salvar configurações na EEPROM
void salvarConfig() {
  EEPROM.put(CONFIG_ADDRESS, config);
  EEPROM.commit();
}

// Função para carregar configurações da EEPROM
void carregarConfig() {
  EEPROM.get(CONFIG_ADDRESS, config);
  if (!config.configurado) {
    modoAP = true;
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
  Serial.print("Canal WiFi: ");
  Serial.println(WiFi.channel());
  Serial.print("Número de estações conectadas: ");
  Serial.println(WiFi.softAPgetStationNum());
  
  // Configurar DNS
  dnsServer.start(53, "*", ap_ip);
  Serial.println("Servidor DNS iniciado");
  
  // Configurar servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/config", HTTP_POST, handleConfig);
  
  server.begin();
  Serial.println("Servidor web iniciado");
  Serial.println("=== AP CONFIGURADO E PRONTO ===\n");
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
  
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
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
  Serial.println("\n=== MENSAGEM MQTT RECEBIDA ===");
  Serial.print("Tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem: ");

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  if(message == "ON") {
    if (!relesAtivos) {
      Serial.println("Comando: LIGAR relés (DESCER)");
      digitalWrite(rl1, HIGH);
      digitalWrite(rl2, HIGH);
      relesAtivos = true;
      Serial.println("Relés LIGADOS - Elevador DESCENDO");
    } else {
      Serial.println("Relés já estão LIGADOS - Elevador já está DESCENDO");
    }
  } else if(message == "OFF") {
    if (relesAtivos) {
      Serial.println("Comando: DESLIGAR relés (SUBIR)");
      digitalWrite(rl1, LOW);
      digitalWrite(rl2, LOW);
      relesAtivos = false;
      Serial.println("Relés DESLIGADOS - Elevador SUBINDO");
    } else {
      Serial.println("Relés já estão DESLIGADOS - Elevador já está SUBINDO");
    }
  }
}

void reconnect() {
  Serial.println("\n=== TENTANDO RECONECTAR AO MQTT ===");
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao MQTT!");
      client.subscribe(mqtt_topic);
      Serial.print("Inscrito no tópico: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("Falha na conexão MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000); // Aguardar inicialização do Serial
  
  Serial.println("\n=== INICIANDO ESP32 ===");
  
  EEPROM.begin(512);
  carregarConfig();
  
  pinMode(rl1, OUTPUT);
  pinMode(rl2, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Iniciar com relés desligados (elevador subindo)
  digitalWrite(rl1, LOW);
  digitalWrite(rl2, LOW);
  relesAtivos = false;
  
  setup_wifi();
  
  Serial.println("ESP32 iniciado e aguardando conexões...");
  Serial.println("Mantenha o botão BOOT pressionado por 5 segundos para reativar o modo AP");
  Serial.println("Estado inicial: Relés DESLIGADOS - Elevador SUBINDO");
}

void loop() {
  // Verificar botão de reset
  if (digitalRead(BUTTON_PIN) == LOW) { // Botão pressionado
    if (lastButtonPress == 0) {
      lastButtonPress = millis();
      Serial.println("Botão pressionado. Mantenha pressionado por 5 segundos para reset...");
    } else if (millis() - lastButtonPress >= RESET_DELAY) {
      Serial.println("\n=== RESET SOLICITADO ===");
      Serial.println("Reativando modo AP...");
      
      // Limpar configurações
      config.configurado = false;
      salvarConfig();
      
      // Desconectar WiFi
      WiFi.disconnect(true);
      delay(1000);
      
      // Reiniciar ESP32
      ESP.restart();
    }
  } else {
    lastButtonPress = 0;
  }

  if (modoAP) {
    dnsServer.processNextRequest();
    server.handleClient();
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
        setup_wifi();
      }
    }
  }
}