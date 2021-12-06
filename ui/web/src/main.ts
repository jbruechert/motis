import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import MOTISPostService from './services/MOTISPostService';

var app = createApp(App)
app.use(router)
app.use(MOTISPostService);

app.mount('#app')
