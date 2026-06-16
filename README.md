# GS---Physical-computing-IoT-IoB---SUBSTITUTIVA-
Nome do aluno: Lucas de Freitas Pagung -RM-553242 - Turma: 3ESPR - Polo: av. paulista - Turno: Noturno.
# 🛰️ Sistema de Manutenção Preditiva para CubeSats (TinyML & IoT)

## 🎯 Objetivo do Projeto (ODS 9 e ODS 13)
Este projeto simula o monitoramento interno da infraestrutura de um micro-satélite (CubeSat). Visando a **economia espacial** e a **resiliência**, desenvolvemos um sistema capaz de detectar falhas de hardware baseadas em anomalias térmicas (simuladas por um sensor DHT22), processando os dados localmente (TinyML) e comunicando a Estação Terrestre (via MQTT) apenas em caso de falha, economizando banda e energia.

## 🛠️ Arquitetura e Tecnologias
- **Edge Impulse:** Utilizado para analisar os datasets de comportamento "Nominal" e de "Falha", extraindo a árvore de decisão matemática.
- **Wokwi (ESP32 + C++):** Onde a simulação do satélite ocorre. As regras do Edge Impulse foram otimizadas e implementadas via estruturas condicionais (`if/else`), dispensando a inclusão de bibliotecas pesadas (`.h`).
- **Protocolo MQTT:** Comunicação assíncrona entre órbita e Terra.
- **Node-RED:** Estação de Controle Terrestre que intercepta os alertas, gera logs em arquivos texto e emite avisos visuais.

## 🔗 Links Importantes
- **Projeto Wokwi:** https://wokwi.com/projects/467014167272232961
  
## 🚀 Como testar (Reprodução)
1. Acesse o link do Wokwi e inicie a simulação.
2. Com o mouse, clique no DHT22 e modifique a temperatura.
3. Variações lentas (<5°C) são lidas como estado Nominal.
4. Arraste o slider bruscamente para causar um aumento repentino. O console exibirá o envio da mensagem MQTT de Falha.
5. No Node-RED, importando o arquivo `node_red_flow.json` incluso neste repositório, você verá o alerta chegando na mesma hora.
