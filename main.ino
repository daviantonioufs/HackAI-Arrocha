#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include <DHT.h>

// --- CONFIGURAÇÕES HARDWARE ---
#define DHTPIN 4      // Pino do Sensor de Temperatura (Mudamos para o 4 que é mais seguro)
#define DHTTYPE DHT22 // Tipo do sensor
#define PIR_PIN 27    // Pino do Slide Switch (Simula mmWave/Presença)
#define AC_LED_PIN 25 // LED do Ar Condicionado

// --- INSTÂNCIAS ---
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// --- CONFIGURAÇÕES MQTT ---
const char* mqtt_server = "test.mosquitto.org"; // Broker Público
const char* topic_sensor = "mangaba/sala/sensor";
const char* topic_control = "mangaba/sala/controle";

// --- VARIÁVEIS ---
unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("📡 Conectando WiFi");
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Conectado!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  
  Serial.print("📩 Comando recebido: ");
  Serial.println(msg);

  if (msg == "ON") {
    digitalWrite(AC_LED_PIN, HIGH);
    Serial.println("💡 AR LIGADO");
  } else if (msg == "OFF") {
    digitalWrite(AC_LED_PIN, LOW);
    Serial.println("💡 AR DESLIGADO");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("🔄 Conectando MQTT...");
    // Cria um ID aleatório para não desconectar
    String clientId = "MangabaSensor-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("✅ Conectado!");
      client.subscribe(topic_control);
    } else {
      Serial.print("Falha rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configura Pinos
  pinMode(PIR_PIN, INPUT);
  pinMode(AC_LED_PIN, OUTPUT);
  digitalWrite(AC_LED_PIN, LOW);

  // Inicia Sensor de Temperatura
  dht.begin();
  
  Serial.println("⏳ Aguardando sensor esquentar (2s)...");
  delay(2000); // Pausa essencial para o DHT não dar erro

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long now = millis();
  
  // Envia dados a cada 2 segundos
  if (now - lastMsg > 2000) {
    lastMsg = now;

    // 1. Ler Presença (Slide Switch)
    bool movement = digitalRead(PIR_PIN);

    // 2. Ler Temperatura e Umidade
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Se o sensor falhar, avisa mas não trava
    if (isnan(t) || isnan(h)) {
      Serial.println("⚠️ Falha leitura DHT! Tentando novamente...");
      return;
    }

    // 3. Montar JSON
    StaticJsonDocument<200> doc;
    doc["movement"] = movement;
    doc["temperature"] = t;
    doc["humidity"] = h;
    doc["sensor_type"] = "DHT22_PIR_COMBO"; // Identificador para a IA
    
    char buffer[256];
    serializeJson(doc, buffer);

    // 4. Enviar
    client.publish(topic_sensor, buffer);
    Serial.print("📤 Enviado: ");
    Serial.println(buffer);
  }
}
