# Controle de Elevador com ESP32 e Vue.js

Este projeto consiste em uma aplicação web para controle de um elevador usando ESP32 e Vue.js.

## Componentes

- Frontend: Vue.js
- Backend: API REST
- Hardware: ESP32
- Protocolo: MQTT

## Configuração do ESP32

1. Conecte o ESP32 ao computador
2. Configure as credenciais do WiFi no arquivo `esp32.ino`:
   ```cpp
   const char* ssid = "seu_wifi";
   const char* password = "sua_senha";
   ```
3. Configure o IP do broker MQTT:
   ```cpp
   const char* mqtt_server = "ip_do_broker";
   ```
4. Faça upload do código para o ESP32

## Configuração do Frontend

1. Instale as dependências:
   ```bash
   npm install
   ```

2. Execute o servidor de desenvolvimento:
   ```bash
   npm run dev
   ```

## Funcionalidades

- Botão "Subir": Liga o LED (porta 4) do ESP32
- Botão "Descer": Desliga o LED (porta 4) do ESP32

## Estrutura do Projeto

- `src/`: Código fonte do frontend Vue.js
- `esp32.ino`: Código do ESP32
- `public/`: Arquivos estáticos

# Vue 3 + Vite

This template should help get you started developing with Vue 3 in Vite. The template uses Vue 3 `<script setup>` SFCs, check out the [script setup docs](https://v3.vuejs.org/api/sfc-script-setup.html#sfc-script-setup) to learn more.

Learn more about IDE Support for Vue in the [Vue Docs Scaling up Guide](https://vuejs.org/guide/scaling-up/tooling.html#ide-support).
