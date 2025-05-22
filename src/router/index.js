import { createRouter, createWebHistory } from 'vue-router'
import Home from '../components/Home.vue'
import Elevador from '../components/Elevador.vue'
import Configuracao from '../components/Configuracao.vue'

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Home
  },
  {
    path: '/elevador',
    name: 'Elevador',
    component: Elevador
  },
  {
    path: '/configuracao',
    name: 'Configuracao',
    component: Configuracao
  }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

export default router 