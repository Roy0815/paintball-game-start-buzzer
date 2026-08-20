<template>
  <div ref="container" class="dot-graph"></div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import { useData } from 'vitepress'
import { instance } from '@viz-js/viz'

const props = defineProps({ srcFile: String })
const container = ref(null)
const { isDark } = useData()

// Matches VitePress's own default-theme color tokens (vars.css) so the
// diagram blends into the page in both themes.
const THEMES = {
  light: {
    EDGE: '#1a237e',
    TEXT: '#3c3c43',
    BORDER: '#3c3c43',
    HEADER_BG: '#0f172a',
    HEADER_FG: '#ffffff'
  },
  dark: {
    EDGE: '#a8b1ff',
    TEXT: '#dfdfd6',
    BORDER: '#98989f',
    HEADER_BG: '#1e293b',
    HEADER_FG: '#ffffff'
  }
}

let viz = null
let dotTemplate = ''

function renderGraph() {
  if (!viz || !dotTemplate || !container.value) return
  const theme = isDark.value ? THEMES.dark : THEMES.light
  const dotSource = dotTemplate.replace(
    /\{\{(\w+)\}\}/g,
    (_, key) => theme[key] ?? ''
  )
  const svg = viz.renderSVGElement(dotSource)
  container.value.replaceChildren(svg)
}

onMounted(async () => {
  const url = import.meta.env.BASE_URL + props.srcFile.replace(/^\//, '')
  const [vizInstance, response] = await Promise.all([instance(), fetch(url)])
  viz = vizInstance
  dotTemplate = await response.text()
  renderGraph()
})

watch(isDark, renderGraph)
</script>

<style>
.dot-graph svg { max-width: 100%; height: auto; }
</style>
