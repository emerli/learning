# Learning Notes

Blog personale di appunti e sintesi tecniche (in italiano), costruito con
[Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) e il suo plugin `blog`,
pubblicato su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning

## Struttura

```
.
├── .gitlab-ci.yml          # pipeline GitLab Pages (build su push a main)
├── mkdocs.yml              # tema Material + plugin blog/rss + nav
└── docs/
    ├── index.md            # landing
    ├── about.md
    └── blog/
        ├── index.md        # pagina indice del blog (lista generata dal plugin)
        └── posts/          # un file .md per post
```

Categorie ammesse (una per post, campo `categories` nel frontmatter):
`Java`, `AI`, `Containers`, `Agile`, `Sicurezza Informatica`.

## Sviluppo locale

```bash
pip install mkdocs-material mkdocs-rss-plugin
mkdocs serve              # http://localhost:8000
mkdocs build --strict     # riproduce la CI
```

## Aggiungere un post

1. Crea `docs/blog/posts/<slug>.md`.
2. Frontmatter minimo:
   ```yaml
   ---
   date: 2026-08-29
   categories:
     - Java
   tags:
     - esempio
   ---
   ```
   `date` e `categories` sono **obbligatori**: `mkdocs build --strict` fallisce senza `date`
   o con una categoria fuori da quelle ammesse in `mkdocs.yml`.
3. (Opzionale) aggiungi `<!-- more -->` dopo il primo paragrafo per definire l'estratto
   mostrato nell'indice.
4. Commit + push su `main` → deploy automatico. Indice, archivio per mese, pagine per
   categoria e feed RSS (`/feed_rss_created.xml`) sono generati dal plugin.

## Deploy

Job `pages` in `.gitlab-ci.yml`, solo sul branch di default: installa le dipendenze
(versioni pinnate), esegue `mkdocs build --strict`, sposta `site/` in `public/` e lo
pubblica su GitLab Pages.
