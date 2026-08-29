# Learning Notes

Raccolta personale di appunti e sintesi tecniche (in italiano), pubblicata come sito statico
[MkDocs](https://www.mkdocs.org/) su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning
- **Tema**: [mkdocs-simple-blog](https://github.com/FernandoCelmer/mkdocs-simple-blog)

## Contenuti

| Area | Argomenti |
|------|-----------|
| Java | Clean Code, SOLID, Java 8 → 25, migrazione e update Spring Boot 2.7 → 4.0 |
| Containers | `devcontainer.json` per DevOps, Java/Quarkus, Mono .NET 4.7 |
| AI | Spec Driven Development, Spec Kit, OpenSpec, agenti OpenCode, Transformer e ML.NET |
| Agile | Open Practice Library, ripasso Scrum, pattern GoF |
| Sicurezza Informatica | Information gathering con Kali, recon Metasploitable 2, exploitation checklist, Linux privilege escalation |

## Sviluppo locale

```bash
pip install mkdocs mkdocs-simple-blog pymdown-extensions
mkdocs serve      # anteprima su http://localhost:8000
mkdocs build --strict   # come in CI
```

## Struttura

```
.
├── .gitlab-ci.yml   # pipeline GitLab Pages (build su push a main)
├── mkdocs.yml       # configurazione e navigazione
└── docs/
    ├── index.md         # homepage
    ├── about.md
    ├── blog/index.md    # indice completo di tutte le note
    ├── Java/            AI/            Agile/
    ├── Containers/      SicurezzaInformatica/
```

## Aggiungere una nota

1. Crea il file `.md` nella cartella tematica sotto `docs/`.
2. (Opzionale) Aggiungi il frontmatter YAML:
   ```yaml
   ---
   title: "Titolo"
   date: 2026-08-29
   description: "Breve descrizione"
   ---
   ```
3. Registra la pagina nel `nav` di `mkdocs.yml` — **obbligatorio**: la CI gira `mkdocs build --strict`
   e fallisce su ogni pagina non inclusa nel `nav`.
4. Aggiungi il link in `docs/blog/index.md`.
5. Commit e push su `main`: il deploy è automatico.

## Deploy

La pipeline `pages` in `.gitlab-ci.yml` gira solo sul branch di default: installa le dipendenze,
esegue `mkdocs build --strict`, sposta `site/` in `public/` e lo pubblica su GitLab Pages.
