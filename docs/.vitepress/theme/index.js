import DefaultTheme from 'vitepress/theme'
import Dot from './Dot.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('Dot', Dot)
  }
}
