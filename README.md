<div align="center">

# 🌡️ Mangaba AI
### Sistema Inteligente de Gestão Energética

![Python](https://img.shields.io/badge/Python-3.x-blue?style=for-the-badge&logo=python&logoColor=white)
![ESP32](https://img.shields.io/badge/Hardware-ESP32-red?style=for-the-badge&logo=espressif&logoColor=white)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange?style=for-the-badge&logo=mqtt&logoColor=white)
![Status](https://img.shields.io/badge/Status-MVP%20Demo-success?style=for-the-badge)

</div>

---

## ⚙️ Arquitetura da Simulação (MVP Demo)

> **🎯 Objetivo da Demonstração**
> Simular o ciclo completo de detecção de movimento/presença via sensor IR no Wokwi (ESP32), envio de dados para o **"Mangaba AI Hub"** (notebook com Python), processamento inteligente por IA, e controle remoto de um "ar condicionado" (representado por LED).

---

## 📁 Estrutura do Projeto

mangaba-ai-project/
├── 📄 README.md # Este arquivo
├── 🔧 mangaba_ai_hub.py # Processamento inteligente (Python)
├── ⚡ main.ino # Código do ESP32 (Wokwi)
├── 🔌 diagram.json # Diagrama do circuito (Wokwi)
└── 🛠️ secrets.h # Configurações WiFi (Wokwi)


---

## 🛠️ Componentes e Ferramentas

| Componente | Função |
| :--- | :--- |
| 🌐 **Wokwi** | Plataforma online de simulação de eletrônica |
| 📟 **ESP32 (Wokwi)** | Microcontrolador simulado |
| 📡 **Sensor PIR/IR** | Detecção de movimento/presença |
| 💡 **LED (Wokwi)** | Simula status do ar condicionado |
| 🦟 **MQTT Broker** | Broker público de comunicação |
| 🐍 **Python 3 + paho** | Processamento inteligente no "Mangaba AI Hub" |

---

## 📡 Tópicos MQTT Configurados

| Tópico | Direção | Função |
|--------|---------|--------|
| `mangaba/sala/sensor` | ESP32 → Python | Dados de movimento do sensor |
| `mangaba/sala/controle` | Python → ESP32 | Comandos do ar condicionado |

> **💡 Importante:** Estes tópicos já estão pré-configurados nos arquivos do projeto.

---

## 🚀 Tutorial de Configuração

### 🧠 Passo 1: Configurar o Mangaba AI Hub

Este script simula a inteligência artificial que processa os dados.

1.  **Instale a dependência:**

    ```bash
    pip install paho-mqtt
    ```

2.  **Execute o Hub:**

    ```bash
    python mangaba_ai_hub.py
    ```

> ⚠️ **Pronto para uso:** O arquivo `mangaba_ai_hub.py` já está configurado com o broker público `test.mosquitto.org`.

-----

### 🔌 Passo 2: Configurar ESP32 no Wokwi

1.  Crie um projeto **ESP32** no [Wokwi](https://wokwi.com/).
2.  Monte o circuito usando o `diagram.json` (PIR no GPIO 27, LED no GPIO 25).
3.  Copie o código do `main.ino` para o editor.
4.  Crie uma aba `secrets.h` no Wokwi com:

    ```cpp
    #define SECRET_SSID "Wokwi-GUEST"
    #define SECRET_PASS ""
    ```

> ⚡ **Pronto para uso:** O `main.ino` já está configurado com o broker público `test.mosquitto.org`.

-----

## 🔄 Fluxo da Demonstração (Demo Day)

1.  **Start:** Execute o script Python:
    ```bash
    python mangaba_ai_hub.py
    ```

2.  **Wokwi:** Inicie a simulação no Wokwi com os arquivos do repositório.

3.  **Ação:** Clique no **sensor PIR** no Wokwi (simula movimento).

4.  **Reação em Cadeia:**
    - ✅ ESP32 detecta movimento
    - ✅ Dados enviados para `mangaba/sala/sensor`
    - ✅ Python processa (se temperatura > 28°C)
    - ✅ Comando `ON` enviado para `mangaba/sala/controle`
    - ✅ **LED acende** (Ar Condicionado LIGADO)

5.  **Economia Automática:**
    - ⏰ Aguarde 15 segundos sem interagir
    - ✅ Hub detecta inatividade
    - ✅ Comando `OFF` enviado
    - ✅ **LED apaga** (Economia de energia)

-----

## 🎊 Teste Bem-Sucedido!

Quando funcionar corretamente, você verá:

**No Terminal Python:**

🚀 Iniciando Mangaba AI Hub...
✅ Conectado ao MQTT Broker!
🚶 Movimento detectado! | 🌡️ Temperatura: 31°C
🔥 Temperatura ALTA! Ligando ar condicionado...
💡 Comando ON enviado para o ESP32


**No Serial Monitor Wokwi:**

🚀 Iniciando Sistema Mangaba...
✅ WiFi conectado!
✅ Conectado ao broker!
🚶 MOVIMENTO DETECTADO! Enviando para o Hub...
💡 AR CONDICIONADO LIGADO

**E o LED vermelho no Wokwi acenderá!** 🎉

-----

## 🚨 Solução de Problemas

### ❌ "Conexão MQTT falhou"
- Verifique se está usando `test.mosquitto.org` em ambos os códigos
- Execute o Python primeiro, depois inicie a simulação Wokwi

### ❌ "LED não acende"
- Clique no sensor PIR no Wokwi (mude o estado)
- Verifique o Serial Monitor no Wokwi para ver as mensagens

### ❌ "Nenhuma mensagem no Python"
- Confirme que os tópicos são exatamente `mangaba/sala/sensor` e `mangaba/sala/controle`

### ✅ Sequência Correta:
1. Execute `python mangaba_ai_hub.py`
2. Inicie simulação no Wokwi
3. Clique no sensor PIR
4. Observe o LED acender após 2-3 segundos

-----

## 🎯 Características da Demonstração

| Funcionalidade | Status | Observações |
| :--- | :---: | :--- |
| **Detecção de movimento** | ✅ Funcional | Sensor PIR simulado |
| **Processamento IA** | ✅ Básico | Lógica de temperatura + movimento |
| **Controle remoto** | ✅ Funcional | LED como simulador de AC |
| **Comunicação MQTT** | ✅ Estável | Broker público Mosquitto |
| **Economia de energia** | ✅ Automática | Desliga após 15s inatividade |

-----

## 📈 Próximas Evoluções

<div align="left">

**🛠️ Expansões técnicas**

  - [ ] Integração com sensores reais (DHT22, PIR físico)
  - [ ] Dashboard web em tempo real
  - [ ] Algoritmos de ML para otimização preditiva
  - [ ] Múltiplas salas/zones

**💼 Oportunidades de negócio**

  - [ ] Escala para outras instituições (hospitais, indústrias)
  - [ ] Modelo SaaS com assinatura
  - [ ] Serviços de analytics preditivo
  - [ ] Integração com sistemas BMS existentes

</div>

-----

## 🏆 Reflexões do Hackathon

### 💪 Pontos Fortes

  * Arquitetura modular e escalável.
  * Protótipo funcional em ambiente simulado.
  * Potencial claro de economia energética.
  * Tecnologias acessíveis e documentadas.

### 🚧 Áreas de Evolução

  * Robustez em ambientes de produção.
  * Segurança (autenticação MQTT, criptografia).
  * Políticas mais complexas de controle.
  * Análise de dados históricos.

-----

## 🤝 Como Contribuir

Quer ajudar a melhorar o Mangaba AI? Siga os passos:

1.  🍴 Faça um **fork** do projeto
2.  🌿 Crie uma **branch** para sua feature (`git checkout -b feature/AmazingFeature`)
3.  💻 Faça o **commit** das suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4.  🚀 Faça o **push** para a branch (`git push origin feature/AmazingFeature`)
5.  📬 Abra um **Pull Request**

-----

## 📄 Licença

Distribuído sob licença **MIT**. Veja `LICENSE` para mais informações.

-----

<div align="center">

**🔗 Links úteis**

[📚 Documentação Wokwi](https://docs.wokwi.com/) • [🦟 Mosquitto MQTT](https://mosquitto.org/) • [🐍 Paho-MQTT](https://pypi.org/project/paho-mqtt/)

</div>
