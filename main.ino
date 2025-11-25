#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHTesp.h"

// ===========================
// CONFIGURAÇÕES DO HARDWARE
// ===========================
#define DHT_PIN 32
#define PIR_PIN 27
#define LED_PIN 25

DHTesp dht;

// ===========================
// CONFIG WI-FI
// ===========================
const char* SSID_NAME = "Wokwi-GUEST";
const char* SSID_PASS = "";

// ===========================
// URL DO BACKEND
// ===========================
String backendUrl = "http://SEU_BACKEND_BASE44/process";

// ===========================
// FUNÇÃO: Conecta WiFi
// ===========================
void connectWiFi() {
  WiFi.begin(SSID_NAME, SSID_PASS);
  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===========================
// FUNÇÃO: Envia os dados ao backend
// ===========================
void enviarParaBackend(float temp, float umidade, bool presenca) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reconnecting...");
    connectWiFi();
  }

  HTTPClient http;
  http.begin(backendUrl);
  http.addHeader("Content-Type", "application/json");

  // Monta JSON
  DynamicJsonDocument doc(256);
  doc["modelo_id"] = "AC_01";
  doc["temp_amb"] = temp;
  doc["umidade"] = umidade;
  doc["tempo_uso_horas"] = 1.2;
  doc["temp_alvo"] = 23.0;
  doc["presenca_detectada"] = presenca;

  String json;
  serializeJson(doc, json);

  Serial.println("\n📤 Enviando JSON:");
  Serial.println(json);

  // Envia requisição
  int httpCode = http.POST(json);

  if (httpCode > 0) {
    Serial.printf("📥 Resposta HTTP: %d\n", httpCode);

    String resposta = http.getString();
    Serial.println("Resposta do backend:");
    Serial.println(resposta);

    // ===========================
    // LED: CONFIRMAÇÃO DE COMANDO
    // ===========================
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  }
  else {
    Serial.printf("❌ Erro ao enviar: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

// ===========================
// CONFIGURAÇÕES INICIAIS
// ===========================
void setup() {
  Serial.begin(115200);

  dht.setup(DHT_PIN, DHTesp::DHT22);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  connectWiFi();
}

// ===========================
// LOOP PRINCIPAL
// ===========================
void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();
  bool presenca = digitalRead(PIR_PIN);

  Serial.println("==============================");
  Serial.println("📡 LEITURAS DO SENSOR");
  Serial.printf("Temperatura: %.2f °C\n", data.temperature);
  Serial.printf("Umidade: %.2f %%\n", data.humidity);
  Serial.printf("Movimento: %s\n", presenca ? "SIM" : "NÃO");

  enviarParaBackend(data.temperature, data.humidity, presenca);

  delay(4000);
}
