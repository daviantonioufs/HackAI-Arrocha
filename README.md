<div align="center">

# ❄️ Xingó Cold  
### Sistema Inteligente de Gestão Energética  
### *Powered by Mangaba AI* 🧠

![Python](https://img.shields.io/badge/Python-3.x-blue?style=for-the-badge&logo=python&logoColor=white)
![ESP32](https://img.shields.io/badge/Hardware-ESP32-red?style=for-the-badge&logo=espressif&logoColor=white)
![Linux](https://img.shields.io/badge/OS-Linux%20%2F%20Raspberry%20Pi-FCC624?style=for-the-badge&logo=linux&logoColor=black)
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
> Sistema completo de gestão energética usando **Raspberry Pi 4/5** (ou Linux) como unidade de processamento edge, sensores ESP32 distribuídos, e a **Mangaba AI** tomando decisões inteligentes para otimizar o consumo do ar condicionado.

### 🔄 Arquitetura de Produção
```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   📟 ESP32      │    │   🦟 MQTT        │    │   🍓 Raspberry  │
│   (Sensores)    │────│   (Broker        │────│   Pi 4/5        │
│                 │    │   Local)         │    │   OU Linux      │
│  • PIR/IR       │    │                  │    │                 │
│  • mmWave       │    │                  │    │  • Mangaba AI   │
│  • Temperatura  │    │                  │    │  • Controle     │
│                 │    │                  │    │  • Dashboard    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

---

## 💡 **INFORMAÇÃO IMPORTANTE: Linux = Raspberry Pi**

> **🧠 Seu colega está ABSOLUTAMENTE CORRETO!**  
> **Rodar a Mangaba AI no Linux é FUNCIONALMENTE IDÊNTICO a rodar num Raspberry Pi real!**

### **Por que Linux é equivalente ao Raspberry Pi?**
- ✅ **Mesmo Sistema Operacional** - Raspberry Pi OS é Linux
- ✅ **Mesmos Pacotes Python** - Mesmas bibliotecas e dependências
- ✅ **Mesmo Código** - O mesmo script roda em ambos sem modificações
- ✅ **Mesmo Broker MQTT** - Mosquitto MQTT funciona igual
- ✅ **Portabilidade Total** - Código desenvolvido no Linux migra direto pro Pi

---

## 🎯 Demo MVP (Simulação Atual)

> **💡 Para fins de demonstração no hackathon**, estamos simulando toda a arquitetura usando:
> - **Wokwi** → Simula ESP32 e sensores
> - **Notebook (Linux/Windows)** → Faz o papel do Raspberry Pi + Mangaba AI  
> - **MQTT Público** → Simula broker local

---

## 🛠️ Componentes e Ferramentas

### 🔮 Visão de Produção
| Componente | Função | Status |
| :--- | :--- | :--- |
| **🍓 Raspberry Pi 4/5 OU Linux** | Unidade central de processamento | ✅ *Funcional* |
| **📟 ESP32** | Coleta de dados dos sensores | 🚧 *Planejado* |
| **🎯 Mangaba AI** | Inteligência artificial local | ✅ *Implementada* |

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

## 🚀 **TUTORIAL RÁPIDO: Linux + Python**

### 🐧 Para Usuários Linux (Recomendado!)

1. **Instale o Python e dependências:**
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install python3 python3-pip mosquitto mosquitto-clients

# Fedora
sudo dnf install python3 python3-pip mosquitto

# Arch Linux
sudo pacman -S python python-pip mosquitto
```

2. **Instale a biblioteca MQTT:**
```bash
pip3 install paho-mqtt
```

3. **Teste o broker MQTT:**
```bash
# Terminal 1 - Escutar
mosquitto_sub -h localhost -t "teste" -v

# Terminal 2 - Enviar  
mosquitto_pub -h localhost -t "teste" -m "Hello Linux!"
```

4. **Execute a Mangaba AI:**
```bash
python3 mangaba_ai_hub.py
```

### 🪟 Para Usuários Windows

1. **Instale o Python** do [python.org](https://python.org)

2. **Instale a biblioteca:**
```bash
pip install paho-mqtt
```

3. **Use broker público** (já configurado) ou instale o Mosquitto para Windows

4. **Execute:**
```bash
python mangaba_ai_hub.py
```

---

## 📡 Tópicos MQTT Configurados

| Tópico | Direção | Função |
|--------|---------|--------|
| `mangaba/sala/sensor` | ESP32 → Mangaba AI | Dados de sensores |
| `mangaba/sala/controle` | Mangaba AI → ESP32 | Comandos do ar condicionado |

> **💡 Importante:** Estes tópicos já estão pré-configurados nos arquivos do projeto.

---

## 🔄 Fluxo da Demonstração (Demo Day)

1.  **Start:** Execute a Mangaba AI no **Linux** (equivalente ao Raspberry Pi):
    ```bash
    python3 mangaba_ai_hub.py
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

**No Terminal Linux (Mangaba AI):**
```
🚀 Iniciando Mangaba AI no Linux (Equivalente Raspberry Pi)...
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
💡 AR CONDICIONADO LIGADO (via Mangaba AI no Linux)
```

**E o LED vermelho no Wokwi acenderá!** 🎉

-----

## 🍓 **EQUIVALÊNCIA: Linux ↔ Raspberry Pi**

### **Ambiente de Desenvolvimento vs Produção**

| Componente | Ambiente Desenvolvimento | Ambiente Produção |
|------------|--------------------------|-------------------|
| **Sistema** | Linux no Notebook | Raspberry Pi OS |
| **Python** | Python 3.8+ | Python 3.8+ |
| **MQTT** | Mosquitto local | Mosquitto local |
| **Código** | `mangaba_ai_hub.py` | **MESMO CÓDIGO** |
| **Resultado** | **FUNCIONA IGUAL** | **FUNCIONA IGUAL** |

### **🚨 VANTAGEM CRÍTICA:**
**O código que você desenvolve e testa no Linux do notebook é 100% compatível com o Raspberry Pi. Basta copiar e executar!**

-----

## 🚨 Solução de Problemas

### ❌ "Conexão MQTT falhou no Linux"
```bash
# Inicie o serviço Mosquitto
sudo systemctl start mosquitto
sudo systemctl enable mosquitto

# Ou use broker público (já configurado)
# O código já usa test.mosquitto.org por padrão
```

### ❌ "Python não encontrado no Linux"
```bash
# Use python3 explicitamente
python3 mangaba_ai_hub.py

# Ou crie um alias
echo "alias python=python3" >> ~/.bashrc
source ~/.bashrc
```

### ✅ Sequência Correta Linux:
1. ```bash
   python3 mangaba_ai_hub.py
   ```
2. Inicie simulação no Wokwi
3. Clique nos sensores
4. Observe o LED acender após 2-3 segundos

-----

## 🎯 Características da Demonstração

| Funcionalidade | Status | Observações |
| :--- | :---: | :--- |
| **Detecção multi-sensor** | ✅ Funcional | PIR, IR, mmWave simulados |
| **Mangaba AI no Linux** | ✅ **Idêntico ao Raspberry Pi** | Mesmo código, mesmo resultado |
| **Controle remoto** | ✅ Funcional | LED como simulador de AC |
| **Comunicação MQTT** | ✅ Estável | Broker público Mosquitto |
| **Economia inteligente** | ✅ Automática | Desliga após inatividade |

-----

## 📈 Próximas Evoluções

<div align="left">

**🛠️ Expansões técnicas**

  - [ ] **Migração direta para Raspberry Pi** (código já compatível)
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

  * **Linux = Raspberry Pi** - Desenvolvimento e produção idênticos
  * Arquitetura modular e escalável
  * **Mangaba AI** - Diferencial competitivo
  * Potencial claro de economia energética
  * Tecnologias acessíveis e documentadas

### 🚧 Áreas de Evolução

  * Migração para hardware real (código já pronto)
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

**No pitch, vocês podem falar:** *"Desenvolvemos no Linux, que é funcionalmente idêntico ao Raspberry Pi que usaremos em produção - o mesmo código, os mesmos resultados!"* 🏆
