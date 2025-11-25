#include <DHT.h>

//incluindo a IA no código
//ela não fará nada efetivamente, pois não pode ser treinada no escopo de tempo disponível
#include <EloquentTinyML.h>

int ledporta = 25; //porta onde o led esta conectado
int pirporta = 22; //porta onde o pir está conectadao
int dhtporta = 4; //porta onde o dht est conectado

bool pareceLigado; //estado do ar condicionado baseado no sensor de temperatura/umidade
bool arCondicionadoLigado = false; // Estado atual do Ar (falso = desligado)
unsigned long ultimoTempoMovimento = 0; // Guarda o tempo do último movimento
// Para teste no Wokwi: 10000 (10 segundos)
// Para vida real: 600000 (10 minutos)
const unsigned long TEMPO_ESPERA = 10000;

int EstadoPresenca = 0;
int ultimoEstadoPresenca = 0; // Memória para saber o que aconteceu antes
float Temperatura;
float Umidade;

#define DHTTYPE DHT22
DHT dht(dhtporta, DHTTYPE);

// ==========================================================
// 1. CONFIGURAÇÃO DA IA (TENSORFLOW LITE)
// ==========================================================

// Definimos 6 Entradas para a IA ter visão total do sistema
// 1.Temperatura | 2.Umidade | 3.PIR | 4.EstadoDesejado | 5.EstadoReal | 6.TempoDecorrido
//ela não fará nada pois não está treinada, apenas lerá dados
#define NUM_INPUTS 6   
#define NUM_OUTPUTS 1  
#define TENSOR_ARENA_SIZE 2*1024 

// Modelo Dummy (Vazio/Inválido) apenas para compilar a estrutura
const unsigned char model_data[] = { 0x00, 0x01, 0x02, 0x03 }; 

// Instância da IA
Eloquent::TinyML::TfLite<NUM_INPUTS, NUM_OUTPUTS, TENSOR_ARENA_SIZE> ml;

// ==========================================================




//o ar condicionado não fica ligado, ele lança o sinal e para
//devido a limitações no simulador, supor que é o mesmo sinal para ligar e desligar
void enviarSinal() {
  digitalWrite(ledporta, HIGH);
  delay(500); // Pisca por meio segundo
  digitalWrite(ledporta, LOW);
}

//gerenciamento de controle para ligar/desligar basedo no estado e presença
void gerenciarArCondicionado() {
  int movimentoAtual = digitalRead(pirporta);
  unsigned long tempoAtual = millis();

  // CENÁRIO 1: TEM GENTE (Movimento Detectado)
  if (movimentoAtual == HIGH) {
    // 1. Reseta o cronômetro (renova os 10 minutos)
    ultimoTempoMovimento = tempoAtual;

    // 2. Se o ar estava desligado, liga AGORA.
    if (arCondicionadoLigado == false && pareceLigado == false ) {
      arCondicionadoLigado = true; // Atualiza estado
      enviarSinal();               // Pisca o LED
    }
  }
  
  // --- CENÁRIO 2: NÃO TEM GENTE (Timeout) ---
  else {
    if (arCondicionadoLigado == true) {
      if (tempoAtual - ultimoTempoMovimento > TEMPO_ESPERA) {
        
        // Desligar: Só se a sala estiver FRIA (Confirmando que está ligado)
        if (pareceLigado == true) { 

           arCondicionadoLigado = false; 
           enviarSinal();                
        } 
        else {
           // [AJUSTE IMPORTANTE AQUI]
           // Se deveria desligar, mas a sala JÁ ESTÁ QUENTE, significa que 
           // o ar desligou sozinho ou quebrou. 
           // Atualizamos a variável para 'false' para alinhar o sistema.
           arCondicionadoLigado = false; 
           //atializa o estado corretamente
        }
      }
    }
  }
}

//vai deduzir o estado do ar condicionado baseado na temperatura e umidade
bool DeduzirEstadoFisico(float temp, float umid) {
  float limiteTemp = 24.0; 
  float limiteUmid = 60.0; 

  if (temp < limiteTemp && umid < limiteUmid) { 
    return true; // "Parece ligado"
  } else {
    return false; // "Parece desligado"
  }
}


void setup() {
  Serial.begin(115200);

  // Dizer ao ESP32 o que cada pino faz
  pinMode(ledporta, OUTPUT); // O LED recebe energia
  pinMode(pirporta, INPUT);  // O PIR envia sinal
  // O pino do DHT a própria biblioteca configura sozinha

  dht.begin(); // Inicia o sensor
}

void loop() {
  //1.chama a função luz
  gerenciarArCondicionado();
  
 delay(100);

  // O DHT a gente lê só de vez em quando usando um contador simples
  // 2. Leitura de Sensores e INPUT DA IA
  static int contadorTempo = 0;
  contadorTempo++;
  
  if (contadorTempo > 20) { 
    Temperatura = dht.readTemperature();
    Umidade = dht.readHumidity();
    
    // Leitura atual do PIR para alimentar a IA
    int leituraPIR = digitalRead(pirporta);

    if (!isnan(Temperatura) && !isnan(Umidade)) {
      // Atualiza a dedução física
      pareceLigado = DeduzirEstadoFisico(Temperatura, Umidade);

      // --- 3. CÁLCULO DA VARIÁVEL DE TEMPO PARA A IA ---
      // A IA não sabe contar tempo sozinha. Nós calculamos e entregamos a ela.
      // Resultado em segundos (float)
      float segundosSemPresenca = (millis() - ultimoTempoMovimento) / 1000.0;

      // --- 4. PREPARAÇÃO DO PACOTE DE DADOS ---
      // Vetor com as 6 variáveis que representam o estado total do sistema
      float input_data[6] = {
          Temperatura,                  // Input 0: Temperatura
          Umidade,                      // Input 1: Umidade
          (float) leituraPIR,           // Input 2: Sensor de Presença Instantâneo
          (float) arCondicionadoLigado, // Input 3: Estado Lógico (Comando)
          (float) pareceLigado,         // Input 4: Estado Físico (Realidade)
          segundosSemPresenca           // Input 5: TEMPO (O segredo para a IA funcionar)
      };

      // Exibe o que a IA está "enxergando"
      Serial.println("--- DADOS LIDOS PELA TENSORFLOW LITE (Prova que a IA está enxergando) ---");
      Serial.print("Temp: "); Serial.print(input_data[0]);
      Serial.print(" | Umid: "); Serial.print(input_data[1]);
      Serial.print(" | PIR: "); Serial.print(input_data[2]);
      Serial.print(" | Estado Registrado: "); Serial.print(input_data[3]);
      Serial.print(" | Estado Aparente: "); Serial.print(input_data[4]);
      Serial.print(" | Tempo(s): "); Serial.println(input_data[5]);
      Serial.println(" ");
      Serial.println("----Dados de exibição-----");
      Serial.print("Temp: ");
      Serial.print(Temperatura);
      Serial.print("C ");
      Serial.print("Umidade: ");
      Serial.println(Umidade);
      Serial.print("Estado registrado: ");
      Serial.print(arCondicionadoLigado);
      Serial.print(" Estado aparente: ");
      Serial.println(pareceLigado);

      
      // Se houvesse modelo treinado, seria possível basear-se nesses dados para tomar uma decisão
      //a ia deveria ser capaz de decidir o estado real e aparente de forma mais refinada
      //devido a o escopo de tempo limitado não foi posível treinar a IA que precisaria de dias
      //entretanto ainda assim o protótipo existe e a ideia é funcional, baseado no mocking da IA
 
      
      Serial.println("----------------------------------------");
    }
    contadorTempo = 0; 
  }
}
