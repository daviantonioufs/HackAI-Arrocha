#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>

// ===============================
// CONFIGURAÇÕES REDE / MQTT
// ===============================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_Automacao_Final";

// Tópicos MQTT
const char* topic_temp = "esp32/data/temperatura";
const char* topic_hum = "esp32/data/umidade";
const char* topic_motion = "esp32/alerta/movimento";
const char* topic_status = "esp32/status";
const char* topic_alerta_duplo = "esp32/alerta/risco_temp_movimento";

// ===============================
// SENSOR / PINOS
// ===============================
#define DHTPIN 21
#define DHTTYPE DHT22
#define PIRPIN 18
#define LEDPIN 4

// Limites
const float TEMP_MAXIMA = 30.0;
const float TEMP_HYSTERESIS = 0.8;
const unsigned long DHT_INTERVAL = 10000UL;
const unsigned long PIR_DEBOUNCE_MS = 200UL;
const unsigned long SEM_MOVIMENTO_DESLIGA = 60000UL;  // 1 minuto sem movimento

// ===============================
// OBJETOS / VARIÁVEIS
// ===============================
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastDHTRead = 0;
unsigned long ultimoMovimento = 0;

bool alertaDuplo = false;
bool arLigado = false;

int pirStableState = LOW;
unsigned long pirLastChange = 0;

char msgBuffer[64];

// ===============================
// FUNÇÕES ÚTEIS
// ===============================
void piscarLED() {
  digitalWrite(LEDPIN, HIGH);
  delay(200);
  digitalWrite(LEDPIN, LOW);
  delay(200);
}

void ligarAr() {
  if (!arLigado) {
    piscarLED();
    arLigado = true;
    client.publish(topic_status, "AR_LIGADO");
    Serial.println(">>> AR LIGADO <<<");
  }
}

void desligarAr() {
  if (arLigado) {
    piscarLED();
    arLigado = false;
    client.publish(topic_status, "AR_DESLIGADO");
    Serial.println(">>> AR DESLIGADO <<<");
  }
}

// ===============================
// WIFI
// ===============================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===============================
// MQTT CALLBACK
// ===============================
void callback(char* topic, byte* payload, unsigned int length) {
  size_t len = min(length, sizeof(msgBuffer) - 1);
  memcpy(msgBuffer, payload, len);
  msgBuffer[len] = '\0';

  Serial.print("Comando recebido: ");
  Serial.println(msgBuffer);
}

// ===============================
// MQTT RECONNECT
// ===============================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");

    if (client.connect(mqtt_client_id)) {
      Serial.println("Conectado!");
      client.subscribe("esp32/comando/#");
      client.publish(topic_status, "Online");
    } else {
      Serial.print("Falhou, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ===============================
// SETUP
// ===============================
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(PIRPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  ultimoMovimento = millis();
}

// ===============================
// LOOP PRINCIPAL
// ===============================
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long now = millis();

  // ===============================
  // LEITURA DHT
  // ===============================
  static float lastTemp = NAN;
  static float lastHum = NAN;

  if (now - lastDHTRead >= DHT_INTERVAL) {
    lastDHTRead = now;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t)) {
      lastTemp = t;
      snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", t);
      client.publish(topic_temp, msgBuffer);
    }

    if (!isnan(h)) {
      lastHum = h;
      snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", h);
      client.publish(topic_hum, msgBuffer);
    }

    Serial.print("T=");
    Serial.print(t);
    Serial.print(" | H=");
    Serial.println(h);
  }

  // ===============================
  // PIR COM DEBOUNCE
  // ===============================
  int raw = digitalRead(PIRPIN);

  if (raw != pirStableState) {
    if (millis() - pirLastChange > PIR_DEBOUNCE_MS) {
      pirLastChange = millis();
      pirStableState = raw;

      if (pirStableState == HIGH) {
        Serial.println("Movimento detectado.");
        client.publish(topic_motion, "DETECTADO");
        ultimoMovimento = millis();

        // ALERTA DUPLO (movimento + temperatura alta)
        if (lastTemp > TEMP_MAXIMA) {
          alertaDuplo = true;
          digitalWrite(LEDPIN, HIGH);
          client.publish(topic_alerta_duplo, "ATIVADO");
        }

        // Ligar ar se ambiente crítico
        if (lastTemp >= 30 || lastHum <= 50) {
          ligarAr();
        }
      } else {
        Serial.println("Movimento finalizado.");
        client.publish(topic_motion, "FINALIZADO");
        digitalWrite(LEDPIN, LOW);
        alertaDuplo = false;
      }
    }
  }

  // ===============================
  // FINALIZAÇÃO DO ALERTA DUPLO
  // ===============================
  if (alertaDuplo && lastTemp <= (TEMP_MAXIMA - TEMP_HYSTERESIS)) {
    alertaDuplo = false;
    digitalWrite(LEDPIN, LOW);
    client.publish(topic_status, "ALERTA_TEMP_NORMALIZADA");
  }

  // ===============================
  // 1 MINUTO SEM MOVIMENTO → DESLIGA AR
  // ===============================
  if (millis() - ultimoMovimento >= SEM_MOVIMENTO_DESLIGA) {
    desligarAr();
  }

  delay(10);
}
