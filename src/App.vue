<template>
  <div class="container">
    <div class="half top" @click="enviar('1')" :class="{ active: activeButton === 'top' }">
      <span>subir</span>
    </div>
    <div class="half bottom" @click="enviar('0')" :class="{ active: activeButton === 'bottom' }">
      <span>descer</span>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import axios from 'axios'

const activeButton = ref(null)

const enviar = async (valor) => {
  try {
    console.log('Valor recebido:', valor)
    activeButton.value = valor === '1' ? 'top' : 'bottom'
    
    console.log('Enviando requisição com valor:', valor)
    const response = await axios.post('http://10.100.0.21:8000/api/enviar-mensagem', 
      { deny: valor },
      {
        headers: {
          'Content-Type': 'application/json',
          'Accept': 'application/json'
        }
      }
    )
    console.log('Resposta do servidor:', response.data)
    
    setTimeout(() => {
      activeButton.value = null
    }, 200)
  } catch (error) {
    console.error('Erro completo:', error)
    if (error.response) {
      // O servidor respondeu com um status de erro
      console.error('Dados da resposta:', error.response.data)
      console.error('Status da resposta:', error.response.status)
      console.error('Headers da resposta:', error.response.headers)
    } else if (error.request) {
      // A requisição foi feita mas não houve resposta
      console.error('Erro na requisição:', error.request)
    } else {
      // Erro na configuração da requisição
      console.error('Erro:', error.message)
    }
    alert('Erro ao enviar comando: ' + (error.response?.data?.message || error.message))
  }
}
</script>

<style>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

html, body {
  height: 100%;
  overflow: hidden;
  position: fixed;
  width: 100%;
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  user-select: none;
  -webkit-tap-highlight-color: transparent;
}

#app {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.container {
  flex: 1;
  display: flex;
  flex-direction: column;
  height: 100%;
  max-width: 430px;
  margin: 0 auto;
  width: 100%;
  background: #fff;
}

.half {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 2.5rem;
  font-weight: bold;
  cursor: pointer;
  transition: all 0.2s ease;
}

.top {
  background: #f5f5f5;
  color: #333;
}

.bottom {
  background: #e0e0e0;
  color: #333;
}

.active {
  opacity: 0.7;
  transform: scale(0.98);
}

span {
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
}
</style>
