# AGENTS.md — Learning Notes

## Overview

Blog/wiki tecnico personale (contenuti in italiano) costruito con **MkDocs** e tema
`simple-blog`, pubblicato su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning
- **Tema**: [mkdocs-simple-blog](https://github.com/FernandoCelmer/mkdocs-simple-blog)

---

## Struttura

```
.
├── .gitlab-ci.yml          # pipeline GitLab Pages
├── .gitignore              # ignora site/
├── mkdocs.yml              # configurazione + nav
└── docs/
    ├── index.md            # homepage
    ├── about.md
    ├── blog/index.md       # indice completo (tutte le note per categoria)
    ├── Java/               # Clean Code, SOLID, Java update, Spring Boot
    ├── Containers/         # devcontainer.json (devops, quarkus, mono net47)
    ├── AI/                 # SDD, Spec Kit, OpenSpec, OpenCode, Transformer/ML.NET
    ├── Agile/              # Open Practice Library, Scrum, GoF
    └── SicurezzaInformatica/  # recon, information gathering, exploitation, privesc
```

---

## Sviluppo locale

```bash
pip install mkdocs mkdocs-simple-blog pymdown-extensions
mkdocs serve            # http://localhost:8000
mkdocs build --strict   # riproduce la CI
```

---

## Aggiungere una pagina

1. Crea il `.md` nella cartella tematica sotto `docs/`.
2. Frontmatter YAML opzionale (`title`, `date`, `description`) — presente in gran parte
   delle pagine Java/AI/Containers, assente in Agile/SicurezzaInformatica. Non è richiesto
   dal tema.
3. **Registra la pagina nel `nav` di `mkdocs.yml`.** Obbligatorio: la CI usa
   `mkdocs build --strict` e fallisce se una pagina in `docs/` non è nel `nav`
   (o se un link interno è rotto).
4. Aggiungi il link in `docs/blog/index.md` (indice completo).
5. Commit + push su `main` → deploy automatico.

---

## CI / GitLab Pages

Job `pages` in `.gitlab-ci.yml`, solo su branch di default:

1. `pip install mkdocs mkdocs-simple-blog pymdown-extensions`
2. `mkdocs build --strict`
3. `mv site public` → artifact `public/` pubblicato su Pages

### Impostazioni Pages
- Project visibility: Public
- Pages visibility: Everyone
- Se le modifiche non si applicano: Settings → Pages, disattiva e riattiva.

---

## Convenzioni

- `mkdocs.yml`: `site_url`, `repo_url`, `repo_name` puntano a `koji76/learning`.
- Nessun plugin `blog`: l'elenco dei contenuti è manuale in `docs/blog/index.md`.
- `markdown_extensions`: `pymdownx.highlight`, `pymdownx.mark`, `admonition`, `tables`, `toc`.
  Non aggiungere estensioni non supportate (in passato `pymdownx.todo`/`task` hanno rotto la build).
- Niente file binari nel repo (in passato erano stati committati un `.pdf` e un `.txt`
  ridondanti, poi rimossi).

---

**Autore**: koji76
