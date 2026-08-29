# Learning Notes

Ispirato dal second brain ho voluto realizzare questo Blog tecnico personale (contenuti in italiano) per salvare e condividere la mia esperienza.
Per minimizzare l'effort necessario è stato costruito con [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) e il suo plugin `blog`,
pubblicato su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning

## Struttura

```
.
├── .gitlab-ci.yml   # pipeline GitLab Pages (build su push a main)
├── mkdocs.yml       # tema Material + plugin blog/social + font
├── hooks.py         # rende il menu piatto: Home / About / categorie
├── overrides/       # override dei template Material (griglia, banner, icone)
└── docs/
    ├── index.md     # home: solo frontmatter, mostra la griglia dei post
    ├── about.md
    ├── stylesheets/extra.css   # griglia, card, banner, colori categoria, tipografia
    └── posts/       # UN file .md per post, tutti qui, niente sottocartelle
```

Con `blog_dir: .` il blog **è** la home. Non ci sono cartelle tematiche: il tema di un
post lo dà il campo `categories` nel frontmatter.

## Come funziona un post

Tutto parte da un singolo file `docs/posts/<nome>.md`. Dal suo **frontmatter** vengono
ricavati categoria, data e il resto:

| Campo | A cosa serve | Obbligatorio |
|-------|--------------|:---:|
| `date` | ordinamento cronologico + data su card e post | **sì** |
| `categories` (il 1º valore) | pagina categoria, **colore + icona** di cover e banner, filtro | **sì** |
| `title` (o l'`# H1`) | titolo su card e `<title>`; genera lo `slug` se assente | no |
| `slug` | URL del post → `/<slug>/` | no (consigliato) |
| `description` | estratto nella griglia + sottotitolo della social card | no |
| corpo Markdown | il post; `<!-- more -->` taglia l'estratto mostrato in griglia | no |

`mkdocs build --strict` (usato in CI) **fallisce** se manca `date` o se la categoria non è
tra quelle ammesse.

Categorie ammesse (`categories_allowed` in `mkdocs.yml`):
`Java`, `AI`, `Containers`, `Agile`, `Sicurezza Informatica`.

## Aggiungere un post

1. Crea `docs/posts/<nome>.md`.
2. Frontmatter minimo:
   ```yaml
   ---
   date: 2026-08-29
   categories:
     - Java
   slug: nome-breve-url        # opzionale: senza, lo slug è derivato dal titolo (lungo)
   description: "Frase breve"  # opzionale: estratto in griglia
   ---
   ```
3. Scrivi il contenuto sotto un `# Titolo`. (Facoltativo) `<!-- more -->` dopo il primo
   paragrafo per fissare l'estratto in griglia.
4. Commit + push su `main` → deploy automatico. Griglia in home, pagina della categoria,
   voce di menu e social card si generano da sole. Nessun indice da aggiornare a mano.

## Aggiungere una categoria

Servono tre punti allineati:

1. `mkdocs.yml` → aggiungila a `categories_allowed`.
2. `docs/stylesheets/extra.css` → `.md-cover--<slug>` con il gradiente colore.
3. `overrides/partials/category-icon.html` → un `elif` con lo `<slug>` e l'icona Material.

Lo slug categoria = nome minuscolo, spazi → trattini (`Sicurezza Informatica` →
`sicurezza-informatica`).

## Sviluppo locale

Serve `mkdocs-material[imaging]` (il plugin `social` richiede anche Cairo/Pango a livello
di sistema — vedi `.gitlab-ci.yml`).

```bash
pip install "mkdocs-material[imaging]"

# anteprima veloce, senza social card
# (il plugin social va in crash sul rebuild di `mkdocs serve`)
MKDOCS_CARDS=false mkdocs serve

# build completa, come la CI
mkdocs build --strict
```

## Deploy

Job `pages` in `.gitlab-ci.yml`, **solo sul branch di default**: installa le dipendenze
(versioni pinnate), esegue `mkdocs build --strict`, sposta `site/` in `public/`, pubblica
su GitLab Pages. Push su altri branch non fa deploy.

## Personalizzazione

- **Tema**: Material, font `IBM Plex Sans` / `JetBrains Mono` (`theme.font` in `mkdocs.yml`).
- **Layout griglia / card / banner categoria**: `overrides/` + `docs/stylesheets/extra.css`.
- **Menu**: `hooks.py` appiattisce la nav in `Home · About · ─── · categorie`.
- **Social card** (immagine anteprima per la condivisione dei link): generate dal plugin
  `social`, una per pagina; non compaiono nel sito, solo nei metadati `og:image`.
