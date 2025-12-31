#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRac.h> // <--- A CLASSE UNIVERSAL
#include <IRutils.h>
#include <DHT.h>

//PINOS
//uint8_t está sendo usado para otimização, pois consome menos bits que o tipo int, ele consome 8 bits, enquanto o int são 32 bits 
const uint8_t pinoLED_IR = 4;
const uint8_t pinoDHT = 14; 
const uint8_t pinoPIR = 12; //NOTA: configurar o pir para tempo de 5 minutos de verificação e o alcance(sensibilidade) tem que ser analisado em relação ao contexto para evitar imprecisão
//sensibilidade alta pode fazer o pir dispsarar com ruido eletrico do próprio microcontrolador, com feixes de sol ou fluxos de vento
//o posicionamento tem que ser bem pensado, no canto da parede pode ser uma boa opção

#define DHTTYPE DHT11

IRac ac(pinoLED_IR); 
DHT dht(pinoDHT, DHTTYPE);

// --- CONFIGURAÇÃO UNIVERSAL ---
//Para trocar o protocolo, mude apenas isto:
//exemplos: decode_type_t::SAMSUNG, decode_type_t::LG, decode_type_t::GREE
decode_type_t protocolo = decode_type_t::MIRAGE;


// Variáveis Universais para guardar preferências
std_ac_fan_t velocidadePadrao = std_ac_fan_t::kAuto; // Universal
std_ac_opmode_t modoPadrao    = std_ac_opmode_t::kCool; // Universal
//Lista de temperaturaas para alterar
uint8_t temperatura_1 = 24;
uint8_t temperatura_2 = 20;
uint8_t temperatura_3 = 18;
//outtras temperaturas poderão ser adicionadas posteriormente


bool pareceLigado = false; //estado do ar condicionado baseado no sensor de temperatura/umidade
bool arCondicionadoLigado = false; // Estado atual do Ar (falso = desligado)
unsigned long ultimoTempoMovimento = 0; // Guarda o tempo do último movimento

//TEMPO DE TOLERANCIA PARA DELISGAR O AR CASO A PRESENÇA ACABE
// Para teste : 10000 (10 segundos)
// Para vida real: 600000 (10 minutos)

//NOTA: PARA ARCONDICIONADOS INVERTER
//o modelo inverter é tão econômico que só vale a pena desligar ele caso a sala se mantenha vazia por 90 minutos.
const unsigned long TEMPO_ESPERA = 10000;


//Captados pelo DHT
float TemperaturaMedida;
float UmidadeMedida;


void ligarAr() {
  //atualizamos o estado do objeto para ligado
  ac.next.power = true; 
  
  //é interessante enviar o sinal mais de uma vez, para garantir que vai ser captado
  ac.sendAc(); //primeiro envio
  delay(50); //pausa minúscula (padrão em muitos protocolos)
  ac.sendAc(); //envia a segunda vez
  delay(50); //pausa minúscula (padrão em muitos protocolos)
  ac.sendAc(); //envia a terceira vez para garantir
}

void desligarAr() {
  //atualizamo o estado do objeto para desligado
  ac.next.power = false;

  //é interessante enviar o sinal mais de uma vez, para garantir que vai ser captado
  ac.sendAc(); //primeiro envio
  delay(50); //pausa minúscula (padrão em muitos protocolos)
  ac.sendAc(); //envia a segunda vez
  delay(50); //pausa minúscula (padrão em muitos protocolos)
  ac.sendAc(); //envia a terceira vez para garantir
}


void mudarTemperatura(uint8_t novaTemp) {

  if (ac.next.degrees != novaTemp) {
    ac.next.degrees = novaTemp; //atualiza a memória do objeto
    ac.next.power = true;       //garante que o ar esteja ligado ao mudar temperatura
    // Envia o comando atualizado
     ac.sendAc(); //primeiro envio
    delay(50); //pausa minúscula (padrão em muitos protocolos)
    ac.sendAc(); //envia a segunda vez
    delay(50); //pausa minúscula (padrão em muitos protocolos)
    ac.sendAc(); //envia a terceira vez para garantir
  }
}



//gerenciamento de controle para ligar/desligar basedo no estado e presença
void gerenciarArCondicionado() {
  bool movimentoAtual = digitalRead(pinoPIR);
  //registro de tempo baseada numa função interna que conta o tempo em milissegundos, fica sempre ativo (mesmo sem ser convocado) e reseta acada 49 dias
  unsigned long tempoAtual = millis(); //unsigned long funciona como um ponteiro de um relógio, se uma operação estourar para um negativo ele faz a conta ao contrário em relação ao máximo

  // CENÁRIO 1: TEM GENTE (Movimento Detectado)
  if (movimentoAtual == HIGH) {
    // 1. Reseta o cronômetro (renova os 10 minutos)
    ultimoTempoMovimento = tempoAtual; //anota quanto o cronômetro marcava da última vez que alguém se mecheu

    // 2. Se o ar estava desligado, liga AGORA.
    //NOTA: TALVEZ VALA A PENA REMOVER A CONDIÇÃO pareceLigado == false PARA FORÇAR QUE LIGUE DE TODO MODO, CASO JÁ ESTEJA LIGADO, NÃO OCORRERIA NADA
    if (arCondicionadoLigado == false && pareceLigado == false ) {
      arCondicionadoLigado = true; // Atualiza estado
      ac.next.degrees = temperatura_1;
      ligarAr();              //LIGA O AR
    }
  }
  
  // --- CENÁRIO 2: NÃO TEM GENTE (Timeout) ---
  else {
    if (arCondicionadoLigado == true) {
      //SUBTRAINDO o tempo que o cronometro registra do tempo que o cronometro marcou da ultima vez que alguém se mecheu
      //É possível conseguir o intervalo de tempo a partir do último estado de presença para verificar se passou o tempo de tolerância
      //caso o cronometro do millis reincie e a conta retorne um valor negativo, a aritmédica modular aplicada aos números unsigned resolverá isso, impedindo valores gradnes demais
      if (tempoAtual - ultimoTempoMovimento > TEMPO_ESPERA) {
        
        // DESLIGAR: Só se a sala estiver FRIA (Confirmando que está ligado)
        if (pareceLigado == true) { 

           arCondicionadoLigado = false; 
           desligarAr();             
        } 
        else {
           // [AJUSTE IMPORTANTE AQUI]
           // Se deveria desligar, mas a sala JÁ ESTÁ QUENTE, significa que 
           // o ar desligou de outro modo ou quebrou. 
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
  float limiteTemp = 26.0; 
  float limiteUmid = 60.0; 

  if (temp < limiteTemp && umid < limiteUmid) { 
    return true; // "Parece ligado"
  } else {
    return false; // "Parece desligado"
  }
}


//FUNÇÃO BÁSICA DE EXEMPLO PARA AJUSTAR A TEMPERATURA com histerese (zona morta)
void AjustarTemperatura(float temp) {
  //Se ficar quente, esfria
  if ( arCondicionadoLigado ) {
    if (temp > 26.0) {
      mudarTemperatura(temperatura_3);
      //Se ficar frio, esquenta
    } else if (temp < 16.0 ) {
      mudarTemperatura(temperatura_2);
      }
  }
}

void setup() {
  Serial.begin(115200);

  // Dizer ao ESP32 o que cada pino faz
  dht.begin();
  pinMode(pinoPIR, INPUT);  // O PIR envia sinal
  // O pino do DHT a própria biblioteca configura sozinha

  ac.begin();

  //1- definimos qual protocolo o controle vai usar
  ac.next.protocol = protocolo; 

  // 2- Aplicamos as configurações iniciais usando os tipos universais
  ac.next.fanspeed = velocidadePadrao;
  ac.next.mode     = modoPadrao;
  ac.next.celsius  = true;
  ac.next.degrees  = temperatura_1;
  ac.next.power    = false; //começa desligado na memória do ESP, não necessariamente condiz com a realidade do aparelho
  
  Serial.print("Sistema iniciado para o protocolo: ");
  Serial.println(typeToString(ac.next.protocol));

  dht.begin(); // Inicia o sensor
}

void loop() {
  //1.chama a função luz
  gerenciarArCondicionado();
  
 delay(100);

  // O DHT a gente lê só de vez em quando usando um contador simples
  static uint8_t contadorTempo = 0;
  contadorTempo++;
  
  if (contadorTempo > 20) { 
    TemperaturaMedida = dht.readTemperature();
    UmidadeMedida = dht.readHumidity();
    
    // Leitura atual do PIR
    bool leituraPIR = digitalRead(pinoPIR);
    if (leituraPIR) {
      Serial.println("Tem gente");
    } else {
      Serial.println("Não tem gente");
    }


    //se a temperatura e umidade foram medidaas, ai atualiza o estado físico, condicional importante para evitar erros
    if (!isnan(TemperaturaMedida) && !isnan(UmidadeMedida)) {
      // Atualiza a dedução física
      pareceLigado = DeduzirEstadoFisico(TemperaturaMedida, UmidadeMedida);
      AjustarTemperatura(TemperaturaMedida);

      Serial.println("----Dados de exibição-----");
      Serial.print("Temp: ");
      Serial.print(TemperaturaMedida);
      Serial.print("C ");
      Serial.print("Umidade: ");
      Serial.println(UmidadeMedida);
      Serial.print("Estado registrado: ");
      Serial.print(arCondicionadoLigado);
      Serial.print(" Estado aparente: ");
      Serial.println(pareceLigado);

      
 
      
      Serial.println("----------------------------------------");
    }
    contadorTempo = 0; //reseta o contador para rodar o dht posteriormente
  }
}
