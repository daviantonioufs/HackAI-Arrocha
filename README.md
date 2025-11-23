
<div align="center">

# ❄️ Xingó Cold  
### Sistema Inteligente de Gestão Energética  
### *Powered by Mangaba AI* 🧠

![Python](https://img.shields.io/badge/Python-3.x-blue?style=for-the-badge&logo=python&logoColor=white)
![ESP32](https://img.shields.io/badge/Hardware-ESP32-red?style=for-the-badge&logo=espressif&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/Edge-Raspberry%20Pi%204%2F5-C51A4A?style=for-the-badge&logo=raspberrypi&logoColor=white)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange?style=for-the-badge&logo=mqtt&logoColor=white)
![Status](https://img.shields.io/badge/Status-MVP%20Demo-success?style=for-the-badge)

</div>

---

## 🧠 Sobre a Mangaba AI

> **"A inteligência que entende o ambiente para economizar energia de forma inteligente"**

**Mangaba AI** é o cérebro do sistema Xingó Cold, responsável por:
- 🎯 **Análise preditiva** de padrões de uso
- 🌡️ **Monitoramento inteligente** de temperatura e ocupação  
- 💡 **Otimização em tempo real** do consumo energético
- 📊 **Aprendizado contínuo** com dados dos sensores

---

## ⚙️ Arquitetura do Sistema (Visão Final)

> **🎯 Objetivo do Produto Final**
> Sistema completo de gestão energética usando **Raspberry Pi 4/5** como unidade de processamento edge, sensores ESP32 distribuídos, e a **Mangaba AI** tomando decisões inteligentes para otimizar o consumo do ar condicionado.

### 🔄 Arquitetura de Produção
```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   📟 ESP32      │    │   🦟 MQTT        │    │   🍓 Raspberry  │
│   (Sensores)    │────│   (Broker        │────│   Pi 4/5        │
│                 │    │   Local)         │    │                 │
│  • PIR/IR       │    │                  │    │  • Mangaba AI   │
│  • mmWave       │    │                  │    │  • Controle     │
│  • Temperatura  │    │                  │    │  • Dashboard    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                      ┌─────────────────┐
                      │   ☁️ Nuvem      │
                      │   (Opcional)    │
                      │                 │
                      │  • Analytics    │
                      │  • Backup       │
                      └─────────────────┘
```

---

## 🎯 Demo MVP (Simulação Atual)

> **💡 Para fins de demonstração no hackathon**, estamos simulando toda a arquitetura usando:
> - **Wokwi** → Simula ESP32 e sensores
> - **Notebook** → Faz o papel do Raspberry Pi + Mangaba AI  
> - **MQTT Público** → Simula broker local

---

## 🛠️ Componentes e Ferramentas

### 🔮 Visão de Produção
| Componente | Função | Status |
| :--- | :--- | :--- |
| **🍓 Raspberry Pi 4/5** | Unidade central de processamento | 🚧 *Planejado* |
| **📟 ESP32** | Coleta de dados dos sensores | 🚧 *Planejado* |
| **🎯 Mangaba AI** | Inteligência artificial local | 🚧 *Planejado* |

### 🎭 Demo MVP (Simulação)
| Componente | Função |
| :--- | :--- |
| 🌐 **Wokwi** | Plataforma online de simulação de eletrônica |
| 📟 **ESP32 (Wokwi)** | Microcontrolador simulado |
| 📡 **Sensores Virtuais** | Detecção PIR, IR, mmWave simulados |
| 💡 **LED (Wokwi)** | Simula status do ar condicionado |
| 🦟 **MQTT Broker** | Broker público de comunicação |
| 🐍 **Python + Mangaba AI** | Processamento inteligente no notebook |

---

## 📁 Estrutura do Projeto

```
xingo-cold-project/
├── 📄 README.md                 # Este arquivo
├── 🔧 mangaba_ai_hub.py         # Mangaba AI (Python)
├── ⚡ main.ino                  # Código do ESP32 (Wokwi)
├── 🔌 diagram.json             # Diagrama do circuito (Wokwi)
└── 🛠️ secrets.h               # Configurações WiFi (Wokwi)
```

---

## 📡 Tópicos MQTT Configurados

| Tópico | Direção | Função |
|--------|---------|--------|
| `mangaba/sala/sensor` | ESP32 → Mangaba AI | Dados de sensores |
| `mangaba/sala/controle` | Mangaba AI → ESP32 | Comandos do ar condicionado |

> **💡 Importante:** Estes tópicos já estão pré-configurados nos arquivos do projeto.

---

## 🚀 Tutorial de Configuração (Demo MVP)

### 🧠 Passo 1: Configurar a Mangaba AI

Este script simula a inteligência artificial que processa os dados.

1.  **Instale a dependência:**

    ```bash
    pip install paho-mqtt
    ```

2.  **Execute a Mangaba AI:**

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

1.  **Start:** Execute a Mangaba AI:
    ```bash
    python mangaba_ai_hub.py
    ```

2.  **Wokwi:** Inicie a simulação no Wokwi com os arquivos do repositório.

3.  **Ação:** Clique nos **sensores** no Wokwi (simula diferentes tipos de detecção).

4.  **Reação em Cadeia:**
    - ✅ ESP32 detecta movimento via PIR/IR/mmWave
    - ✅ Dados enviados para `mangaba/sala/sensor`
    - ✅ **Mangaba AI** processa (temperatura + tipo de sensor + histórico)
    - ✅ Comando inteligente enviado para `mangaba/sala/controle`
    - ✅ **LED acende** (Ar Condicionado LIGADO)

5.  **Economia Inteligente:**
    - ⏰ **Mangaba AI** monitora inatividade
    - 🧠 Toma decisão baseada em padrões aprendidos
    - ✅ Comando `OFF` enviado
    - ✅ **LED apaga** (Economia de energia otimizada)

-----

## 🎊 Teste Bem-Sucedido!

Quando funcionar corretamente, você verá:

**No Terminal da Mangaba AI:**
```
🚀 Iniciando Mangaba AI Hub...
✅ Conectado ao MQTT Broker!
🎯 PIR: Presença detectada! | 🌡️ Temperatura: 31°C
🔥 Temperatura ALTA! Ligando ar condicionado via PIR...
💡 Comando ON enviado para o ESP32
```

**No Serial Monitor Wokwi:**
```
🚀 Iniciando Sistema Xingó Cold...
✅ WiFi conectado!
✅ Conectado ao broker!
🎯 PIR: Movimento detectado!
💡 AR CONDICIONADO LIGADO (via Mangaba AI)
```

**E o LED vermelho no Wokwi acenderá!** 🎉

-----

## 🍓 Migração para Raspberry Pi (Roadmap)

### **Por que Raspberry Pi 4/5?**
- ✅ **Processamento local** - Mais rápido que nuvem
- ✅ **Confiabilidade** - Funciona sem internet  
- ✅ **Custo-benefício** - Mais barato que soluções enterprise
- ✅ **Ecossistema** - Amplo suporte para sensores

### **Plano de Migração:**
1. **Raspberry Pi 4** (US$ 35-55) - Custo mais baixo
2. **Raspberry Pi 5** (US$ 60-80) - Melhor performance
3. **Configuração**:
   ```bash
   # No Raspberry Pi
   git clone <repositorio>
   pip install -r requirements.txt
   python mangaba_ai_hub.py  # Agora rodando localmente!
   ```

-----

## 🚨 Solução de Problemas

### ❌ "Conexão MQTT falhou"
- Verifique se está usando `test.mosquitto.org` em ambos os códigos
- Execute o Python primeiro, depois inicie a simulação Wokwi

### ❌ "LED não acende"
- Clique nos sensores no Wokwi (mude o estado)
- Verifique o Serial Monitor no Wokwi para ver as mensagens

### ❌ "Nenhuma mensagem no Python"
- Confirme que os tópicos são exatamente `mangaba/sala/sensor` e `mangaba/sala/controle`

### ✅ Sequência Correta:
1. Execute `python mangaba_ai_hub.py`
2. Inicie simulação no Wokwi
3. Clique nos sensores
4. Observe o LED acender após 2-3 segundos

-----

## 🎯 Características da Demonstração

| Funcionalidade | Status | Observações |
| :--- | :---: | :--- |
| **Detecção multi-sensor** | ✅ Funcional | PIR, IR, mmWave simulados |
| **Mangaba AI** | ✅ Básico | Lógica inteligente de controle |
| **Controle remoto** | ✅ Funcional | LED como simulador de AC |
| **Comunicação MQTT** | ✅ Estável | Broker público Mosquitto |
| **Economia inteligente** | ✅ Automática | Desliga após inatividade |

-----

## 📈 Próximas Evoluções

<div align="left">

**🛠️ Expansões técnicas**

  - [ ] **Migração para Raspberry Pi 4/5**
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

  * Arquitetura modular e escalável
  * Protótipo funcional em ambiente simulado
  * **Mangaba AI** - Diferencial competitivo
  * Potencial claro de economia energética
  * Tecnologias acessíveis e documentadas

### 🚧 Áreas de Evolução

  * Migração para hardware real (Raspberry Pi)
  * Segurança (autenticação MQTT, criptografia)
  * Políticas mais complexas de controle
  * Análise de dados históricos

-----

## 🤝 Como Contribuir

Quer ajudar a melhorar o Xingó Cold? Siga os passos:

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

[📚 Documentação Wokwi](https://docs.wokwi.com/) • [🦟 Mosquitto MQTT](https://mosquitto.org/) • [🐍 Paho-MQTT](https://pypi.org/project/paho-mqtt/) • [🍓 Raspberry Pi](https://www.raspberrypi.com/)

**🧠 Xingó Cold - Powered by Mangaba AI**

*Economia inteligente de energia através de decisões inteligentes*

</div>
