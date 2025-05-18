<template>
  <div class="home-container">
    <div class="grid">
      <!-- Primeira linha -->
      <div class="card" @click="navegarPara('/elevador')">
        <i class="fas fa-elevator"></i>
        <span>Elevador TV</span>
      </div>
      <div class="card" @click="navegarPara('/portao-garagem')">
        <i class="fas fa-garage"></i>
        <span>Portão Garagem</span>
      </div>

      <!-- Segunda linha -->
      <div class="card" @click="navegarPara('/portao-social')">
        <i class="fas fa-door-open"></i>
        <span>Portão Social</span>
      </div>
      <div class="card" @click="navegarPara('/ar-condicionado')">
        <i class="fas fa-snowflake"></i>
        <span>Ar Condicionado</span>
      </div>

      <!-- Terceira linha -->
      <div class="card" @click="navegarPara('/banheira')">
        <i class="fas fa-bath"></i>
        <span>Banheira</span>
      </div>
      <div class="card" @click="navegarPara('/configuracao')">
        <i class="fas fa-cog"></i>
        <span>Configuração</span>
      </div>
    </div>
  </div>
</template>

<script setup>
import { useRouter } from 'vue-router'

const router = useRouter()

const navegarPara = (rota) => {
  router.push(rota)
}
</script>

<style scoped>
.home-container {
  padding: 20px;
  height: 100vh;
  background: #f5f5f5;
}

.grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  grid-template-rows: repeat(3, 1fr);
  gap: 20px;
  max-width: 600px;
  margin: 0 auto;
  height: calc(100vh - 40px);
}

.card {
  background: white;
  border-radius: 15px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 20px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  cursor: pointer;
  transition: transform 0.2s, box-shadow 0.2s;
}

.card:hover {
  transform: translateY(-5px);
  box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15);
}

.card i {
  font-size: 2.5rem;
  margin-bottom: 10px;
  color: #2196F3;
}

.card span {
  font-size: 1.2rem;
  font-weight: 500;
  color: #333;
  text-align: center;
}
</style> 