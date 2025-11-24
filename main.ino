#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h> 
#include <DHT_U.h> 

// ******************************************************
// --- 1. CONFIGURAÇÕES DE REDE E MQTT ---
// ******************************************************
const char* ssid = "Wokwi-GUEST";          // <<<<< SUBSTITUA
const char* password = "";     // <<<<< SUBSTITUA

const char* mqtt_server = "test.mosquitto.org"; 
const int mqtt_port = 1883;                            
const char* mqtt_client_id = "ESP32_Automação_Final";

// --- TÓPICOS ---
const char* topic_temp = "esp32/data/temperatura"; 
const char* topic_hum = "esp32/data/umidade"; 
const char* topic_motion = "esp32/alerta/movimento"; 
const char* topic_status = "esp32/status";
const char* topic_alerta_duplo = "esp32/alerta/risco_temp_movimento"; 

// Tópicos de Assinatura (Comandos)
const char* topic_subscribe_comando = "esp32/comando/#"; 
const char* topic_comando_alerta = "esp32/comando/acao_alerta"; 

// ******************************************************
// --- 2. CONFIGURAÇÕES DOS SENSORES E PINOS ---
// ******************************************************
#define DHTPIN 4       // Pino GPIO para o DHT22
#define DHTTYPE DHT22  
#define PIRPIN 5       // Pino GPIO para o PIR
#define LEDPIN 2       // Pino GPIO para LED de teste (onboard)

// --- CONFIGURAÇÕES DE LIMITE ---
const float TEMP_MAXIMA = 30.0; // Limite de temperatura em Celsius

// ******************************************************
// --- 3. VARIÁVEIS E OBJETOS ---
// ******************************************************
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE); 

// Variáveis de Controle
long lastDHTRead = 0;
const long DHT_INTERVAL = 10000; // Publica DHT a cada 10 segundos
int pirState = LOW; 
bool alertaAtivo = false; 
char msgBuffer[50]; 

// ******************************************************
// --- 4. FUNÇÕES DE REDE E MQTT ---
// ******************************************************

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

  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect(mqtt_client_id)) { 
      Serial.println("conectado!");
      
      client.subscribe(topic_subscribe_comando); 
      client.subscribe(topic_comando_alerta);    
      
      Serial.println("Inscrito nos tópicos de Comando e Ação de Alerta.");
      client.publish(topic_status, "Online"); 
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// --- Função de Callback (Receptor de Mensagens) ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(">>> Mensagem recebida no tópico: ");
  Serial.println(topic);

  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  
  // 1. Lógica do Alerta Duplo (Resposta do Broker: LIGAR/DESLIGAR LED)
  if (String(topic) == topic_comando_alerta) {
    if (messageTemp == "LIGAR_LED_ALERTA") {
      digitalWrite(LEDPIN, HIGH);
      Serial.println(">>> ORDEM RECEBIDA: Ligar LED de Alerta!");
      client.publish(topic_status, "LED_ALERTA_LIGADO_PELO_BROKER");
    } else if (messageTemp == "DESLIGAR_LED_ALERTA") {
      digitalWrite(LEDPIN, LOW);
      Serial.println(">>> ORDEM RECEBIDA: Desligar LED de Alerta.");
      client.publish(topic_status, "LED_ALERTA_DESLIGADO");
    }
  }
  // 2. Lógica de Comandos Genéricos (LED e Reboot)
  else if (String(topic) == "esp32/comando/led") {
    if (messageTemp == "ligar") {
      digitalWrite(LEDPIN, HIGH);
      client.publish(topic_status, "LED_Ligado");
    } else if (messageTemp == "desligar") {
      digitalWrite(LEDPIN, LOW);
      client.publish(topic_status, "LED_Desligado");
    }
  } 
  else if (String(topic) == "esp32/comando/reboot") {
    Serial.println(">>> Comando de Reboot recebido. Reiniciando...");
    client.publish(topic_status, "Reiniciando");
    delay(100); 
    ESP.restart();
  }
}

// ******************************************************
// --- 5. SETUP E LOOP (Funções Principais) ---
// ******************************************************

void setup() {
  Serial.begin(115200);

  // Inicialização de Pinos e Sensores
  dht.begin();
  pinMode(PIRPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW); 

  // Inicialização de Rede e MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // 1. Manter Conexão MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // *Ouvinte ativo para receber comandos*

  // --- Lógica de Publicação dos Sensores ---
  long now = millis();
  
  // Leitura das variáveis DHT (assumindo que já foram lidas ou serão lidas agora)
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // 2. Publicação Temporizada do DHT
  if (now - lastDHTRead >= DHT_INTERVAL) { 
    lastDHTRead = now;
    
    if (isnan(h) || isnan(t)) {
      client.publish(topic_status, "Erro_DHT");
    } else {
      snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", t);
      client.publish(topic_temp, msgBuffer);
      snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", h);
      client.publish(topic_hum, msgBuffer);
    }
  }

  // 3. Lógica de Alerta Duplo (PIR + Temperatura)
  int val = digitalRead(PIRPIN);
  
  if (val == HIGH) { // Movimento detectado
    if (pirState == LOW) {
      pirState = HIGH; 
      
      // >>> VERIFICAÇÃO DE ALERTA DUPLO <<<
      if (t > TEMP_MAXIMA && alertaAtivo == false) {
          Serial.println(">>> ALERTA DUPLO DISPARADO! Enviando Risco ao Broker.");
          
          snprintf(msgBuffer, sizeof(msgBuffer), "RISCO_TEMP_MOVIMENTO_T:%.1f", t);
          client.publish(topic_alerta_duplo, msgBuffer); // Envia o alerta
          
          alertaAtivo = true; // Impede envio repetido
      } else {
          client.publish(topic_motion, "DETECTADO");
      }
    }
  } else { // Sem Movimento
    if (pirState == HIGH) {
      pirState = LOW; 
      alertaAtivo = false; // Reseta o flag
      
      client.publish(topic_motion, "FINALIZADO");
    }
  }

  delay(10); 
}
