# AGENTS.md — Learning Notes

## Overview

Blog tecnico personale (contenuti in italiano) costruito con **Material for MkDocs** e il
plugin `blog`, pubblicato su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning
- **Stack**: `mkdocs-material` + plugin `blog` (nativo) + `mkdocs-rss-plugin`

---

## Struttura

```
.
├── .gitlab-ci.yml          # pipeline GitLab Pages
├── .gitignore              # ignora site/
├── mkdocs.yml              # tema + plugin + nav + markdown_extensions
└── docs/
    ├── index.md            # landing
    ├── about.md
    └── blog/
        ├── index.md        # indice blog (lista generata dal plugin)
        └── posts/          # un .md per post, tutti allo stesso livello
```

Non ci sono più cartelle tematiche: la tassonomia è data dal campo `categories` nel
frontmatter di ogni post. Indice, archivio per mese (`blog/archive/<anno>/`), pagine per
categoria (`blog/category/<slug>/`) e feed RSS (`feed_rss_created.xml`) sono **generati** dal
plugin — non si mantengono a mano.

---

## Frontmatter dei post

```yaml
---
date: 2026-08-29          # obbligatorio
categories:               # obbligatorio, valori ammessi in mkdocs.yml
  - Java
tags: [opzionale]
title: "Titolo opzionale" # se assente usa l'H1
description: "opzionale"
---
```

Categorie ammesse (`categories_allowed` in `mkdocs.yml`):
`Java`, `AI`, `Containers`, `Agile`, `Sicurezza Informatica`.
Aggiungerne una nuova = aggiornare `categories_allowed`.

`<!-- more -->` nel corpo definisce l'estratto in homepage/indice (`post_excerpt: optional`).

---

## Sviluppo locale

```bash
pip install mkdocs-material mkdocs-rss-plugin
mkdocs serve
mkdocs build --strict     # come in CI
```

---

## CI / GitLab Pages

Job `pages` in `.gitlab-ci.yml`, solo su branch di default:

1. `pip install "mkdocs-material==9.7.7" "mkdocs-rss-plugin==1.19.0"` (versioni pinnate)
2. `mkdocs build --strict`
3. `mv site public` → artifact `public/`

### Trappole note
- `--strict` fallisce se un post non ha `date` o ha una categoria non ammessa.
- `rss` plugin configurato con `use_git: false` + `date_from_meta` → non dipende dalla
  profondità del clone CI (shallow).
- Il warning "MkDocs 2.0 / Material" a inizio build è informativo, non blocca.
- Niente file binari nel repo.

---

## Convenzioni

- `mkdocs.yml`: `site_url`, `repo_url`, `repo_name` → `koji76/learning`.
- `post_url_format: "{slug}"` → URL dei post senza data (`/blog/<slug>/`).
- `markdown_extensions`: admonition, tables, attr_list, md_in_html, pymdownx
  (highlight/inlinehilite/snippets/superfences/mark), toc.

---

## Stato / TODO

- I contenuti dei post sono appunti di studio da rivedere: linguaggio, accuratezza,
  aggiunta di estratti `<!-- more -->`, tag.
- Valutare una sezione "Reference" separata dal flusso blog per i materiali non-articolo
  (checklist, recon report, cheat-sheet).

---

**Autore**: koji76
