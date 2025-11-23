#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include <DHT.h> // <--- Biblioteca do Sensor de Temperatura

// --- Configurações de Hardware ---
#define DHTPIN 15     // Pino do DHT22
#define DHTTYPE DHT22 
#define PIR_PIN 27    // Pino do Slide Switch (Simula mmWave)
#define AC_LED_PIN 25 // LED do AC

// --- Instâncias ---
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// --- Configurações MQTT ---
const char* mqtt_server = "test.mosquitto.org";
const char* topic_sensor = "mangaba/sala/sensor";
const char* topic_control = "mangaba/sala/controle";

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
  } else if (msg == "OFF") {
    digitalWrite(AC_LED_PIN, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("🔄 Conectando ao Broker MQTT...");
    String clientId = "MangabaSensor-";
    clientId += String(random(0xffff), HEX);
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
  
  pinMode(PIR_PIN, INPUT);
  pinMode(AC_LED_PIN, OUTPUT);
  dht.begin(); // Inicia o sensor DHT
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  static unsigned long lastMsg = 0;
  unsigned long now = millis();

  // Envia dados a cada 2 segundos
  if (now - lastMsg > 2000) {
    lastMsg = now;

    // 1. Ler Presença (Slide Switch simula mmWave)
    bool movement = digitalRead(PIR_PIN);

    // 2. Ler Ambiente (Temperatura e Umidade)
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("❌ Falha ao ler DHT!");
      return;
    }

    // 3. Montar JSON
    StaticJsonDocument<200> doc;
    doc["movement"] = movement;
    doc["temperature"] = t;
    doc["humidity"] = h;
    
    char buffer[256];
    serializeJson(doc, buffer);

    // 4. Enviar para a IA
    client.publish(topic_sensor, buffer);
    Serial.print("📤 Enviado: ");
    Serial.println(buffer);
  }
}
