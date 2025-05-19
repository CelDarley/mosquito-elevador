# ESP32 Elevador Control

Sistema de controle de elevador usando ESP32 com interface web para configuração e controle via MQTT.

## Funcionalidades

- Configuração inicial via Access Point (AP)
- Interface web para configuração do WiFi
- Controle via MQTT
- Botão de reset para reconfiguração
- Controle de relés para subida e descida do elevador

## Requisitos

### Hardware
- ESP32
- 2 Relés
- Botão BOOT/EN (para reset)

### Bibliotecas Arduino
- WiFi.h
- PubSubClient.h
- EEPROM.h
- DNSServer.h
- WebServer.h

## Configuração

1. Faça upload do código para o ESP32
2. Conecte-se à rede WiFi "ESP32-Config" (senha: 12345678)
3. Acesse 192.168.4.1 no navegador
4. Configure o WiFi da sua rede
5. O ESP32 reiniciará e se conectará à sua rede

## Uso

### Controle via MQTT
- Tópico: `esp32/acionar`
- Comandos:
  - `ON`: Ativa os relés por 1 segundo
  - `OFF`: Desativa os relés

### Reset
- Mantenha o botão BOOT pressionado por 5 segundos para reativar o modo AP

## Configurações MQTT
- Servidor: 10.100.0.21
- Porta: 1883
- Tópico: esp32/acionar

## Pinagem
- Relé 1: GPIO 4
- Relé 2: GPIO 16
- Botão Reset: GPIO 0 (BOOT/EN)

## Licença
Este projeto está sob a licença MIT.

# Vue 3 + Vite

This template should help get you started developing with Vue 3 in Vite. The template uses Vue 3 `<script setup>` SFCs, check out the [script setup docs](https://v3.vuejs.org/api/sfc-script-setup.html#sfc-script-setup) to learn more.

Learn more about IDE Support for Vue in the [Vue Docs Scaling up Guide](https://vuejs.org/guide/scaling-up/tooling.html#ide-support).
