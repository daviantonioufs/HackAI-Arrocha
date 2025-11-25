#include <DHT.h>

// ---------------------------------------------------------------------
// PINOS
// ---------------------------------------------------------------------
int ledporta = 25;
int pirporta = 22;
int dhtporta = 4;

// ---------------------------------------------------------------------
// ESTADOS
// ---------------------------------------------------------------------
bool arLigado = false;       // Estado real do ar
bool pareceLigado = false;   // Deduções pela temperatura/umidade

unsigned long momentoUltimoMovimento = 0;  
unsigned long tempoSemMovimento = 0;       

#define DHTTYPE DHT22
DHT dht(dhtporta, DHTTYPE);

// tempo antes de desligar o ar (1 minuto)
const unsigned long TEMPO_DESLIGAR = 60000;


// =====================================================================
// IA (somente documentada)
// =====================================================================
/*
    A IA teria entradas:
      [temperatura, umidade, movimento, arLigado,
       pareceLigado, tempoSemMovimento]

    E retornaria:
      prob_ar_ligado
      prob_deveria_ligar
      prob_erro_termico

    EXEMPLO (não usado agora):
        ml.predict(input, output);
*/
// =====================================================================


// =====================================================================
// Deduz estado térmico
// =====================================================================
bool deduzirEstadoFisico(float temp, float umid) {
  float limiteTemp = 24.0;
  float limiteUmid = 60.0;

  return (temp < limiteTemp && umid < limiteUmid);
}



// =====================================================================
// LED -> sinal IR universal
// =====================================================================
void enviarSinal() {
  digitalWrite(ledporta, HIGH);
  delay(250);
  digitalWrite(ledporta, LOW);
}



// =====================================================================
// Liga e desliga
// =====================================================================
void ligarAr() {
  if (!arLigado) {
    arLigado = true;
    momentoUltimoMovimento = millis(); // zera contagem ao ligar
    tempoSemMovimento = 0;

    enviarSinal();
    Serial.println("[AR] LIGADO ✔ (Presença + Sala quente)");
  }
}

void desligarAr() {
  if (arLigado) {
    arLigado = false;
    tempoSemMovimento = 0; // zera ao desligar

    enviarSinal();
    Serial.println("[AR] DESLIGADO ✔ (1 min sem movimento + Sala fria)");
  }
}



// =====================================================================
// Setup
// =====================================================================
void setup() {
  Serial.begin(115200);
  pinMode(ledporta, OUTPUT);
  pinMode(pirporta, INPUT);
  dht.begin();
}



// =====================================================================
// Loop principal
// =====================================================================
void loop() {

  int movimento = digitalRead(pirporta);
  unsigned long agora = millis();

  static int contador = 0;
  contador++;

  // leitura do DHT a cada ~2 segundos
  if (contador > 20) {

    float temp = dht.readTemperature();
    float umid = dht.readHumidity();
    contador = 0;

    if (!isnan(temp) && !isnan(umid)) {

      pareceLigado = deduzirEstadoFisico(temp, umid);
      bool salaFria = pareceLigado;
      bool salaQuente = !pareceLigado;

      // =======================================================
      //  CONTAGEM DE TEMPO SEM MOVIMENTO
      //  (somente quando o AR está ligado)
      // =======================================================
      if (arLigado) {

        if (movimento) {
          momentoUltimoMovimento = agora;  // reseta quando detecta movimento
        }

        tempoSemMovimento = agora - momentoUltimoMovimento;
      } 
      else {
        tempoSemMovimento = 0;             // ZERA quando o ar está desligado
      }


      // =======================================================
      //  LIGAMENTO
      // =======================================================
      if (!arLigado) {
        if (movimento && salaQuente) {
          ligarAr();
        }
      }

      // =======================================================
      //  DESLIGAMENTO
      // =======================================================
      if (arLigado) {
        if (tempoSemMovimento > TEMPO_DESLIGAR && salaFria) {
          desligarAr();
        }
      }


      // =======================================================
      // Serial debug
      // =======================================================
      Serial.println("\n--------- SISTEMA ----------");

      Serial.print("Temperatura: ");
      Serial.print(temp);
      Serial.println(" C");

      Serial.print("Umidade: ");
      Serial.print(umid);
      Serial.println(" %");

      Serial.print("Presença: ");
      Serial.println(movimento ? "SIM" : "NÃO");

      Serial.print("Estado físico: ");
      Serial.println(pareceLigado ? "FRIO (parece ligado)" : "QUENTE (parece desligado)");

      Serial.print("Estado lógico: ");
      Serial.println(arLigado ? "AR LIGADO" : "AR DESLIGADO");

      Serial.print("Tempo sem movimento: ");
      Serial.print(tempoSemMovimento / 1000);
      Serial.println("s");

      Serial.println("-----------------------------");
    }
  }

  delay(100);
}
