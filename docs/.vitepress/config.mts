import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'

// Projekt-Site auf GitHub Pages liegt unter /<repo-name>/
const base = '/paintball-game-start-buzzer/'

export default withMermaid(defineConfig({
  title: 'Paintball Start Timer',
  lang: 'de-DE',
  base,
  // head-Links bekommen den base-Pfad nicht automatisch vorangestellt
  // (anders als z.B. das hero.image in index.md), daher hier manuell.
  head: [['link', { rel: 'icon', href: `${base}buzzer-icon.png`, type: 'image/png' }]],

  locales: {
    root: {
      label: 'Deutsch',
      lang: 'de-DE',
      description: 'ESP32-basierter Taster-Sound-Trigger fuer Paintball-Spielstarts',
      themeConfig: {
        nav: [
          { text: 'Home', link: '/' },
          {
            text: 'Erste Schritte',
            activeMatch: '^/hardware|^/flash|^/setup-anleitung',
            items: [
              { text: 'Hardware & Verkabelung', link: '/hardware' },
              { text: 'Firmware flashen', link: '/flash' },
              { text: 'Initiales Setup', link: '/setup-anleitung' },
            ],
          },
          {
            text: 'Entwicklung',
            activeMatch: '^/entwicklung|^/architektur|^/troubleshooting',
            items: [
              { text: 'Entwicklung', link: '/entwicklung' },
              { text: 'Architektur & technische Details', link: '/architektur' },
              { text: 'Lessons Learned', link: '/troubleshooting' },
            ],
          },
        ],
        sidebar: [
          {
            text: 'Erste Schritte',
            collapsed: false,
            items: [
              { text: 'Hardware & Verkabelung', link: '/hardware' },
              { text: 'Firmware flashen', link: '/flash' },
              { text: 'Initiales Setup', link: '/setup-anleitung' },
            ],
          },
          {
            text: 'Entwicklung',
            collapsed: false,
            items: [
              { text: 'Entwicklung', link: '/entwicklung' },
              { text: 'Architektur & technische Details', link: '/architektur' },
              { text: 'Lessons Learned', link: '/troubleshooting' },
            ],
          },
        ],
      },
    },
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      title: 'Paintball Start Timer',
      description: 'ESP32-based push-button sound trigger for paintball game starts',
      themeConfig: {
        nav: [
          { text: 'Home', link: '/en/' },
          {
            text: 'Get Started',
            activeMatch: '^/en/hardware|^/en/flash|^/en/setup-guide',
            items: [
              { text: 'Hardware & Wiring', link: '/en/hardware' },
              { text: 'Flash Firmware', link: '/en/flash' },
              { text: 'Initial Setup', link: '/en/setup-guide' },
            ],
          },
          {
            text: 'Development',
            activeMatch: '^/en/development|^/en/architecture|^/en/troubleshooting',
            items: [
              { text: 'Development', link: '/en/development' },
              { text: 'Architecture & Technical Details', link: '/en/architecture' },
              { text: 'Lessons Learned', link: '/en/troubleshooting' },
            ],
          },
        ],
        sidebar: [
          {
            text: 'Get Started',
            collapsed: false,
            items: [
              { text: 'Hardware & Wiring', link: '/en/hardware' },
              { text: 'Flash Firmware', link: '/en/flash' },
              { text: 'Initial Setup', link: '/en/setup-guide' },
            ],
          },
          {
            text: 'Development',
            collapsed: false,
            items: [
              { text: 'Development', link: '/en/development' },
              { text: 'Architecture & Technical Details', link: '/en/architecture' },
              { text: 'Lessons Learned', link: '/en/troubleshooting' },
            ],
          },
        ],
      },
    },
  },

  themeConfig: {
    // Default is h2-only; several pages (e.g. flash.md) use h3 subsections
    // that should stay reachable via the right-hand "On this page" outline.
    outline: [2, 3],
    socialLinks: [
      { icon: 'github', link: 'https://github.com/Roy0815/paintball-game-start-buzzer' },
    ],
  },

  // <esp-web-install-button> (docs/flash.md, docs/en/flash.md) ist ein
  // Web Component von esp-web-tools, keine Vue-Komponente - Vue soll nicht
  // versuchen, sie aufzuloesen.
  vue: {
    template: {
      compilerOptions: {
        isCustomElement: (tag) => tag === 'esp-web-install-button',
      },
    },
  },
}))
