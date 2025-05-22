<template>
  <div class="config-container">
    <div class="header">
      <button class="back-button" @click="$router.push('/')">
        <i class="fas fa-arrow-left"></i> Voltar
      </button>
    </div>
    <div class="content">
      <h1>Configurações</h1>
      <div class="wifi-config">
        <h2>Configuração do ESP32</h2>
        
        <div v-if="connectionStatus === 'Desconectado'" class="connection-instructions">
          <h3>Para configurar o ESP32:</h3>
          <ol>
            <li>Conecte-se à rede WiFi "ESP32-Config"</li>
            <li>Senha: 12345678</li>
            <li>Após conectar, a página de configuração aparecerá automaticamente</li>
          </ol>
          <div class="wifi-icon">
            <i class="fas fa-wifi"></i>
          </div>
        </div>

        <div v-else class="iframe-container">
          <iframe 
            src="http://192.168.4.1" 
            frameborder="0"
            width="100%"
            height="500px"
            title="Configuração do ESP32"
          ></iframe>
        </div>
      </div>
      <div class="connection-status">
        <p>Status: {{ connectionStatus }}</p>
        <p v-if="connectionStatus === 'Conectado'">
          IP: {{ deviceIP }}
        </p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Configuracao',
  data() {
    return {
      connectionStatus: 'Desconectado',
      deviceIP: '',
      checkInterval: null
    }
  },
  mounted() {
    // Verifica o status da conexão a cada 5 segundos
    this.checkInterval = setInterval(this.checkConnection, 5000)
    this.checkConnection()
  },
  beforeUnmount() {
    // Limpa o intervalo quando o componente é destruído
    if (this.checkInterval) {
      clearInterval(this.checkInterval)
    }
  },
  methods: {
    async checkConnection() {
      try {
        // Tenta conectar ao ESP32
        const response = await fetch('http://192.168.4.1', { 
          method: 'HEAD',
          mode: 'no-cors',
          timeout: 2000
        })
        this.connectionStatus = 'Conectado'
        this.deviceIP = '192.168.4.1'
      } catch (error) {
        this.connectionStatus = 'Desconectado'
        this.deviceIP = ''
      }
    }
  }
}
</script>

<style scoped>
.config-container {
  max-width: 800px;
  margin: 0 auto;
  padding: 20px;
}

.header {
  margin-bottom: 20px;
}

.back-button {
  background: none;
  border: none;
  color: #666;
  cursor: pointer;
  font-size: 16px;
  padding: 5px 10px;
}

.back-button:hover {
  color: #333;
}

.content {
  background: white;
  border-radius: 8px;
  padding: 20px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

h1 {
  color: #333;
  margin-bottom: 20px;
}

h2 {
  color: #666;
  margin-bottom: 15px;
}

.connection-instructions {
  background: #f8f9fa;
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 20px;
  margin-bottom: 20px;
  text-align: center;
}

.connection-instructions h3 {
  color: #333;
  margin-bottom: 15px;
}

.connection-instructions ol {
  text-align: left;
  margin: 0 auto;
  max-width: 300px;
  padding-left: 20px;
}

.connection-instructions li {
  margin-bottom: 10px;
  color: #666;
}

.wifi-icon {
  font-size: 48px;
  color: #666;
  margin-top: 20px;
}

.iframe-container {
  border: 1px solid #ddd;
  border-radius: 4px;
  overflow: hidden;
  margin-bottom: 20px;
}

.connection-status {
  margin-top: 20px;
  padding: 10px;
  background: #f5f5f5;
  border-radius: 4px;
}

.connection-status p {
  margin: 5px 0;
  color: #666;
}
</style> 