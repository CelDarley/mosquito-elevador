<template>
  <div class="elevador-container">
    <div class="header">
      <button class="back-button" @click="voltar">
        <i class="fas fa-arrow-left"></i>
        Voltar
      </button>
    </div>
    <div class="container">
      <div class="half top" @click="enviar('1')" :class="{ active: activeButton === 'top' }">
        <span>subir</span>
      </div>
      <div class="half bottom" @click="enviar('0')" :class="{ active: activeButton === 'bottom' }">
        <span>descer</span>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import axios from 'axios'
import { useRouter } from 'vue-router'

const router = useRouter()
const activeButton = ref(null)

const voltar = () => {
  router.push('/')
}

const enviar = async (valor) => {
  try {
    console.log('Valor recebido:', valor)
    activeButton.value = valor === '1' ? 'top' : 'bottom'
    
    console.log('Enviando requisição com valor:', valor)
    const response = await axios.post('https://api.catenasystem.com.br/api/enviar-mensagem', 
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
      console.error('Dados da resposta:', error.response.data)
      console.error('Status da resposta:', error.response.status)
      console.error('Headers da resposta:', error.response.headers)
    } else if (error.request) {
      console.error('Erro na requisição:', error.request)
    } else {
      console.error('Erro:', error.message)
    }
    alert('Erro ao enviar comando: ' + (error.response?.data?.message || error.message))
  }
}
</script>

<style scoped>
.elevador-container {
  height: 100vh;
  display: flex;
  flex-direction: column;
}

.header {
  padding: 15px;
  background: #fff;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.back-button {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 16px;
  border: none;
  background: #f5f5f5;
  border-radius: 8px;
  cursor: pointer;
  font-size: 1rem;
  color: #333;
  transition: background-color 0.2s;
}

.back-button:hover {
  background: #e0e0e0;
}

.back-button i {
  font-size: 1.2rem;
}

.container {
  flex: 1;
  display: flex;
  flex-direction: column;
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