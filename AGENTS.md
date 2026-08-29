# AGENTS.md — Learning Notes

## Overview

Blog tecnico personale (contenuti in italiano) costruito con **Material for MkDocs** e il
plugin `blog`, pubblicato su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning
- **Stack**: `mkdocs-material[imaging]` + plugin nativi `blog` e `social` + `hooks.py` +
  override di template in `overrides/`. Nessun RSS.

---

## Struttura

```
.
├── .gitlab-ci.yml   # pipeline GitLab Pages
├── .gitignore       # site/  .cache/  __pycache__/
├── mkdocs.yml       # tema + plugin + font + nav
├── hooks.py         # on_nav: appiattisce il menu (Home / About / categorie)
├── overrides/
│   ├── blog.html                     # lista post come griglia di card
│   └── partials/
│       ├── post.html                 # card compatta (cover generata per categoria)
│       ├── content.html              # banner categoria in cima al post
│       └── category-icon.html        # icona Material per slug categoria (condivisa)
└── docs/
    ├── index.md              # home: SOLO frontmatter → mostra la griglia
    ├── about.md
    ├── stylesheets/extra.css # griglia, card, banner, colori categoria, tipografia
    └── posts/                # UN .md per post, tutti qui, niente sottocartelle
```

`blog_dir: .` → il blog **è** la home. Non ci sono cartelle tematiche: la tassonomia è il
campo `categories` nel frontmatter. Griglia in home, pagine categoria
(`/category/<slug>/`), voce di menu e social card sono **generate** — non si mantengono a
mano. L'archivio per data è disattivato (`archive: false`).

---

## Modello di un post

Tutto parte dal singolo file `docs/posts/<nome>.md`. Dal frontmatter:

| Campo | Effetto | Obbligatorio |
|-------|---------|:---:|
| `date` | ordinamento + data su card/post | **sì** |
| `categories` (1º valore) | pagina categoria, **colore + icona** di cover e banner, filtro | **sì** |
| `title` / `# H1` | titolo card + `<title>`; genera lo `slug` se assente | no |
| `slug` | URL `/<slug>/` — impostarlo alla creazione e non cambiarlo | no (consigliato) |
| `description` | estratto nella griglia + sottotitolo social card | no |
| `<!-- more -->` nel corpo | taglia l'estratto mostrato in griglia (`post_excerpt: optional`) | no |

```yaml
---
date: 2026-08-29
categories:
  - Java
slug: nome-breve
description: "Frase breve"
---
```

---

## Aggiungere una categoria

Tre punti da allineare (slug = nome minuscolo, spazi → trattini):

1. `mkdocs.yml` → `plugins.blog.categories_allowed`
2. `docs/stylesheets/extra.css` → `.md-cover--<slug>` con il gradiente
3. `overrides/partials/category-icon.html` → un `elif slug == "<slug>"` con l'icona

Categorie attuali: `Java`, `AI`, `Containers`, `Agile`, `Sicurezza Informatica`.

---

## Sviluppo locale

Sulla macchina di sviluppo manca `pip`; usare un venv dedicato.

```bash
pip install "mkdocs-material[imaging]"

# anteprima veloce SENZA social card
MKDOCS_CARDS=false mkdocs serve

# build completa come la CI (con social card)
mkdocs build --strict
```

Il plugin `social` va in **crash sul rebuild incrementale** di `mkdocs serve`
(`AttributeError: card_pool`) → per l'anteprima usare `MKDOCS_CARDS=false`, oppure
`mkdocs build` + un server statico su `site/`.

---

## CI / GitLab Pages

Job `pages` in `.gitlab-ci.yml`, **solo su branch di default**:

1. `apt-get install` delle librerie di sistema per `social` (Cairo, Pango, freetype, font)
2. `pip install "mkdocs-material[imaging]==9.7.7"` (pinnato)
3. `mkdocs build --strict`
4. `mv site public` → artifact `public/`
5. `cache: .cache/` — il primo render delle social card è lento (~20-30s), poi cache

Immagine: `python:3.12-slim` (non alpine: Pango su alpine è problematico).

### Trappole note
- `--strict` fallisce se un post non ha `date` o ha una categoria non in `categories_allowed`.
- `social` richiede librerie di sistema (vedi `before_script`); offline fallisce il fetch
  del font da Google Fonts.
- Il warning "MkDocs 2.0 / Material" a inizio build è informativo, non blocca.
- Niente file binari nel repo.

---

## Convenzioni

- `mkdocs.yml`: `site_url`, `repo_url`, `repo_name` → `koji76/learning`.
- `post_url_format: "{slug}"` → URL dei post senza data (`/<slug>/`).
- Font: `theme.font` = `IBM Plex Sans` (testo) / `JetBrains Mono` (codice).
- Menu piatto via `hooks.py`: `Home · About · ─── · categorie` (la sezione collassabile
  "Categorie" del plugin viene smontata e le categorie diventano link di primo livello;
  "About" rispedito in fondo). Divisore in `extra.css` (`nth-child(3)` della nav primaria).
- Tipografia titoli e stile card/banner: tutto in `docs/stylesheets/extra.css`.
- `markdown_extensions`: admonition, tables, attr_list, md_in_html, pymdownx
  (highlight/inlinehilite/snippets/superfences/mark), toc.

---

## Stato / TODO

- Aggiungere `slug:` e `description:` ai 22 post esistenti (ora gli slug sono derivati
  dai titoli e vengono lunghi, es. `devcontainerjson--devops-devcontainer`).
- I contenuti dei post sono appunti di studio da rivedere: linguaggio, accuratezza,
  estratti `<!-- more -->`.
- I pulsanti Precedente/Successivo in fondo alle pagine seguono ancora un ordine vecchio
  (il plugin li cabla prima del hook).
- Valutare una sezione "Reference" separata per i materiali non-articolo (checklist,
  recon report, cheat-sheet).

---

**Autore**: koji76
