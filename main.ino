#include <DHT.h>

// ---------------------------------------------------------------------
// PINOS
// ---------------------------------------------------------------------
int ledporta = 25;    // LED que envia o sinal para o ar-condicionado
int pirporta = 22;    // Sensor PIR
int dhtporta = 4;     // DHT22

// ---------------------------------------------------------------------
// ESTADOS
// ---------------------------------------------------------------------
bool arLigado = false;       // Estado real do ar
bool pareceLigado = false;   // Deduções pela temperatura/umidade

unsigned long momentoUltimoMovimento = 0;

#define DHTTYPE DHT22
DHT dht(dhtporta, DHTTYPE);

// tempo antes de desligar o ar (1 minuto)
const unsigned long TEMPO_DESLIGAR = 60000;


// =====================================================================
//   IA (APENAS COMENTÁRIOS — NÃO ALTERA O CÓDIGO)
// =====================================================================
/*
    A IA observaria:
        temperatura, umidade, movimento,
        arLigado, pareceLigado, tempoSemMovimento

    E retornaria:
        [0] prob_ar_ligado
        [1] prob_deveria_ligar
        [2] prob_erro_termico

    Exemplo de consulta (não ativo agora):
        float entrada[6] = {...};
        float saida[3];
        ml.predict(entrada, saida);

        if (!arLigado && saida[1] > 0.8) ligarAr();
        if (saida[2] > 0.7) alertaErro();
*/
// =====================================================================



// =====================================================================
//   Deduz se a sala está fria (ou seja, ar "parece" ligado)
// =====================================================================
bool deduzirEstadoFisico(float temp, float umid) {
  float limiteTemp = 24.0;
  float limiteUmid = 60.0;

  return (temp < limiteTemp && umid < limiteUmid);
}



// =====================================================================
//   Sinal enviado ao ar (pisca o LED)
// =====================================================================
void enviarSinal() {
  digitalWrite(ledporta, HIGH);
  delay(250);
  digitalWrite(ledporta, LOW);
}



// =====================================================================
//   Ligar e desligar o ar
// =====================================================================
void ligarAr() {
  if (!arLigado) {
    arLigado = true;
    enviarSinal();
    Serial.println("[AR] LIGADO ✔ (Presença + Sala quente)");
  }
}

void desligarAr() {
  if (arLigado) {
    arLigado = false;
    enviarSinal();
    Serial.println("[AR] DESLIGADO ✔ (1 min sem movimento + Sala fria)");
  }
}



// =====================================================================
//   Setup
// =====================================================================
void setup() {
  Serial.begin(115200);
  pinMode(ledporta, OUTPUT);
  pinMode(pirporta, INPUT);
  dht.begin();
}



// =====================================================================
//   Loop
// =====================================================================
void loop() {

  int movimento = digitalRead(pirporta);
  unsigned long agora = millis();

  // atualizar tempo de movimento
  if (movimento) {
    momentoUltimoMovimento = agora;
  }

  static int contador = 0;
  contador++;

  // lê sensores a cada 20 ciclos (~2 segundos)
  if (contador > 20) {
    float temp = dht.readTemperature();
    float umid = dht.readHumidity();
    contador = 0;

    if (!isnan(temp) && !isnan(umid)) {

      // deduz fisicamente
      pareceLigado = deduzirEstadoFisico(temp, umid);
      bool salaFria = pareceLigado;
      bool salaQuente = !pareceLigado;

      unsigned long tempoSemMovimento = agora - momentoUltimoMovimento;

      // ==========================================================
      //   LÓGICA DE LIGAR
      // ==========================================================
      if (!arLigado) {
        if (movimento && salaQuente) {
          ligarAr();
        }
      }

      // ==========================================================
      //   LÓGICA DE DESLIGAR
      // ==========================================================
      else { // arLigado == true
        if (tempoSemMovimento > TEMPO_DESLIGAR && salaFria) {
          desligarAr();
        }
      }

      // ==========================================================
      // DEBUG SERIAL
      // ==========================================================
      Serial.println("\n--------- SISTEMA ----------");

      Serial.print("Temperatura: ");
      Serial.print(temp);
      Serial.println(" C");

      Serial.print("Umidade: ");
      Serial.print(umid);
      Serial.println(" %");

      Serial.print("Presença: ");
      Serial.println(movimento ? "SIM" : "NÃO");

      Serial.print("Estado físico (sensação): ");
      Serial.println(pareceLigado ? "FRIO (ar parece ligado)" : "QUENTE (ar parece desligado)");

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
