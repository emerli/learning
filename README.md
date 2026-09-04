# Learning Notes

Ispirato dal second brain ho voluto realizzare questo Blog tecnico personale (contenuti in italiano) per salvare e condividere la mia esperienza.
Costruito con **Hugo** + tema **hugo-coder**, pubblicato su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning

## Struttura

```
.
├── .gitlab-ci.yml   # pipeline GitLab Pages (hugo --minify)
├── hugo.toml        # tema + params + menu + tassonomie
├── assets/          # CSS/JS custom + player asciinema vendored
├── layouts/         # override minimi del tema (lista post, header, post)
├── i18n/it.toml     # traduzioni ("Indice" per il TOC)
├── static/          # avatar, cast asciinema
├── content/
│   ├── about.md
│   ├── projects.md
│   └── posts/       # UN file .md per post, tutti qui
└── themes/hugo-coder/  # tema vendored (non modificare)
```

La home è il profilo (avatar, nome, info, social). La lista post è su `/posts/` con
tag cloud in cima. Le pagine tag (`/tags/<slug>/`) sono generate dalla tassonomia
nativa di Hugo.

## Come funziona un post

Tutto parte da un singolo file `content/posts/<nome>.md`. Dal suo **frontmatter** vengono
ricavati data, titolo, tag e il resto:

| Campo | A cosa serve | Obbligatorio |
|-------|--------------|:---:|
| `date` | ordinamento cronologico + data su card e post | **sì** |
| `title` | titolo su card e `<title>` (Hugo NON lo prende dall'H1) | **sì** |
| `slug` | URL del post → `/posts/<slug>/` | no (consigliato) |
| `description` | estratto nella lista + sotto il titolo nel post | no |
| `tags` | pillole su card e post + pagina `/tags/<slug>/`; 1º valore = categoria | **sì** |
| `lab: true` | badge "LAB" su card e post + tag `Lab` (contenuto pratico / hands-on) | no |
| `draft: true` | escluso dalla build | no |

Convenzione tag: il primo valore è la categoria (`Java`, `AI`, `Docker`, `Linux`,
`Agile`, `Sicurezza Informatica`), poi 2-4 tag specifici. I post `lab: true` hanno anche
la tag `Lab`.

## Aggiungere un post

1. Crea `content/posts/<nome>.md`.
2. Frontmatter minimo:
   ```yaml
   ---
   date: 2026-08-29
   title: "Titolo del post"
   slug: nome-breve-url        # opzionale: senza, lo slug è derivato dal titolo (lungo)
   description: "Frase breve"  # opzionale: estratto in lista
   tags:
     - Java
   ---
   ```
3. Scrivi il contenuto. (Facoltativo) `<!--more-->` dopo il primo paragrafo per fissare
   il summary.
4. Commit + push su `main` → deploy automatico. Lista, pagina tag e menu si generano da
   sole. Nessun indice da aggiornare a mano.

## Sviluppo locale

Hugo è un binario singolo in `~/bin/hugo` (v0.165.0 extended).

```bash
~/bin/hugo server          # anteprima su http://localhost:1313/learning/
~/bin/hugo --minify        # build completa in public/ (come la CI)
```

## Deploy

Job `pages` in `.gitlab-ci.yml`, **solo sul branch di default**: immagine
`klakegg/hugo:0.165.0-ext-alpine` (pinnata), `hugo --minify` → `public/`, pubblicato su
GitLab Pages. Push su altri branch non fa deploy.

## Personalizzazione

- **Tema**: hugo-coder vendored in `themes/` (non modificare: gli override stanno in
  `layouts/`).
- **Stile card lista, badge LAB, tabelle, tag cloud**: `assets/css/custom.css`.
- **Menu**: `Home · Blog · Progetti · About` in `hugo.toml` (`languages.it.menu.main`).
- **TOC**: attivo su tutti i post, collassabile sotto "Indice".
- **Screencast asciinema**: player vendored in `assets/` (niente CDN), ponte in
  `assets/js/custom.js`; embed nel post con
  `<asciinema-player src="/nome.cast" speed="1.5"></asciinema-player>`.
