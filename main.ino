#include <WiFi.h>
#include <PubSubClient.h>

// ================= CONFIGURAÇÃO WiFi =================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ================= CONFIGURAÇÃO MQTT =================
const char* mqtt_server = "test.mosquitto.org";  // Broker público

// ================= PINOS =================
const int PIR_PIN = 27;    // Sensor de movimento
const int LED_PIN = 25;    // LED do ar condicionado

// ================= TÓPICOS =================
const char* topic_sensor = "mangaba/sala/sensor";    // ENVIA dados
const char* topic_control = "mangaba/sala/controle"; // RECEBE comandos

// ================= VARIÁVEIS GLOBAIS =================
WiFiClient espClient;
PubSubClient client(espClient);
bool lastPirState = false;
bool ledState = false;

// ================= SETUP WiFi =================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("📡 Conectando à ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("✅ WiFi conectado!");
    Serial.print("📱 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("❌ Falha na conexão WiFi");
  }
}

// ================= CALLBACK MQTT =================
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("📨 Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

  // Controle do LED/Ar Condicionado
  if (String(topic) == topic_control) {
    if (messageTemp == "ON") {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
      Serial.println("💡 AR CONDICIONADO LIGADO");
    } else if (messageTemp == "OFF") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
      Serial.println("💡 AR CONDICIONADO DESLIGADO");
    }
  }
}

// ================= RECONEXÃO MQTT =================
void reconnect() {
  while (!client.connected()) {
    Serial.print("🔄 Tentando conexão MQTT...");
    
    // Gera ID único para o cliente
    String clientId = "MangabaESP32-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("✅ Conectado ao broker!");
      client.subscribe(topic_control);
      Serial.print("📡 Inscrito no tópico: ");
      Serial.println(topic_control);
    } else {
      Serial.print("❌ Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s...");
      delay(5000);
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  
  // Configura pinos
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Inicia com LED desligado
  
  Serial.println("🚀 Iniciando Sistema Mangaba...");
  Serial.println("💡 Simulador de Gestão Energética");
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  Serial.println("✅ Sistema inicializado! Aguardando movimento...");
}

// ================= LOOP PRINCIPAL =================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leitura do sensor PIR
  bool pirState = digitalRead(PIR_PIN);
  
  // Detecta transição de movimento ( LOW -> HIGH )
  if (pirState == HIGH && lastPirState == LOW) {
    Serial.println("🚶 MOVIMENTO DETECTADO! Enviando para o Hub...");
    client.publish(topic_sensor, "MOVIMENTO");
    Serial.println("📤 Dados enviados para processamento");
  }
  
  lastPirState = pirState;
  delay(100);  // Pequeno delay para estabilidade
}
