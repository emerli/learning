# AGENTS.md — Learning Notes

## Overview

Ispirato dal second brain ho voluto realizzare questo Blog tecnico personale (contenuti in italiano) per salvare e condividere la mia esperienza.
Per minimizzare l'effort necessario è stato costruito con **Material for MkDocs** e il plugin `blog`, pubblicato su **GitLab Pages**.

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
│       ├── post.html                 # card compatta (cover per categoria + badge "Lab")
│       ├── content.html              # banner categoria in cima al post (+ badge "Lab")
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
| `lab: true` | badge "LAB" su cover della card e banner del post — segnala un contenuto pratico / hands-on | no |
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

Categorie attuali: `Java`, `AI`, `Containers`, `Linux`, `Agile`, `Sicurezza Informatica`.

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

## Stato

- Pubblicati (categoria AI): `ai-ollama-llm-locali`, `ai-opencode`, `ai-agents-md`,
  `ai-sdd-guida-rapida`, `ai-spec-kit-book-api`, `ai-openspec-book-api`.
- ~16 post ancora `draft: true` in `docs/posts/`, da revisionare (`grep -l 'draft: true' docs/posts/*.md`).
- Convenzione nome file/slug: prefisso categoria (`ai-…`), per ordinare la cartella.

## TODO

### Revisione dei post in draft

Per ogni post: `slug:` corto e definitivo + `description:`; rivedere linguaggio e
accuratezza; aggiungere `<!-- more -->` dopo il primo paragrafo; se è un walkthrough,
aggiungere una sezione di chiusura **"Cosa ho notato"** (verdetto reale, non doc). Poi
`draft: false`. Se il post cross-linka altri draft, pubblicarli nello stesso commit.

### Nuovi articoli dai vecchi repo GitLab (reverse)

L'utente ha molti repo GitLab usati per imparare: trasformarli in articoli.

1. L'utente fornisce la lista (`glab repo list --per-page 100`) + una frase per repo.
2. Triage: tenere solo quelli con frizione reale / un'opinione maturata / un "aha"
   fuori dalla doc. Scartare spike e tutorial seguiti alla lettera.
3. L'utente clona i repo scelti in locale; per ciascuno analizzare
   **README + `git log` (dead-end, "fix", "revert") + config non ovvia + stack/versioni**.
4. Bozza articolo: frontmatter completo, corpo = contesto → cosa ho fatto → gotcha →
   "cosa ho notato" → riferimenti. `draft: true` fino a revisione dell'utente.
5. Se serve una categoria nuova: aggiornare `categories_allowed` + `.md-cover--<slug>`
   in `extra.css` + un `elif` in `overrides/partials/category-icon.html`.

### Articoli categoria Linux (da scrivere)

Categoria `Linux` già configurata ma senza post. Repo sorgente in locale:
`~/Projects/personal/koji76/{arch,debian,suse}-init-ansible` + `nixos-kde`.
Tre articoli:

- **distro hopping** — pezzo riflessivo "cosa ho notato": il percorso
  Arch → Debian/Fedora → openSUSE → NixOS. Perché ho smesso di reinstallare.
- **Ansible cross-distro** (`lab: true`) — lo stesso playbook
  (`base → podman → dotfiles → flatpak → devcontainers → development → tabaccai`,
  `hosts: localhost`, vault per le chiavi SSH) portato su 4 distro: cosa resta
  uguale e cosa si rompe (nomi pacchetti, KDE vs GNOME, btrfs, `ansible_user`).
  Gotcha già nel repo: `debian-init-ansible` ha committato lo stato IDE dei
  devcontainer (`.gitignore` mancante nel ruolo `devcontainers`).
- **NixOS** — articolo a sé ("è un mondo"). `nixos-kde`: 206 commit, README già
  quasi un tutorial (config dichiarativa, generazioni, home-manager, agenix per
  i segreti nel Nix store). Storia di attrito: `xdg portal` ×8, `smb` ×8,
  `emoji font` ×6, `kmail` ×6, `hyprland` ×5. Eventuale `lab:` o due parti
  (concetti + la settimana a litigare con xdg-desktop-portal / SMB / emoji).

### Lab GitOps: GitLab CE → Tekton → Argo CD → kind (categoria Containers)

Idea nuova (da costruire da zero, **niente specifico del cliente**). Dal lavoro
in cliente l'utente ha visto la catena GitLab ↔ Tekton (CI) ↔ ArgoCD (CD) su
OpenShift. Lab `lab: true`: ricostruire parzialmente la catena, **tutto in
locale su Docker**, per **far vedere come funziona il GitOps end-to-end** — dal
commit al pod in esecuzione, con Git come unica fonte di verità e Argo CD che
riconcilia lo stato desiderato (Tekton è la CI che lo alimenta) — a chi non
l'ha mai visto cablato. In cliente è OpenShift + operator Pipelines/GitOps; qui
l'equivalente upstream.

- **Stack**: `kind` (cluster k8s in Docker) + container `gitlab/gitlab-ce` +
  Tekton Pipelines/Triggers + Argo CD, tutti sulla stessa rete Docker.
- **App**: la più banale possibile (immagine statica), il punto è la catena.
- **Flusso**: push su GitLab CE → webhook → EventListener Tekton →
  `PipelineRun` (build + push immagine al registry di GitLab CE) → bump tag nei
  manifest kustomize → Argo CD sync → pod su kind.
- **Punti da spiegare**: Trigger/EventListener/TriggerBinding di Tekton; modello
  *pull* di Argo CD; dove vivono le credenziali (registry, repo Git, cluster).
- **Attriti attesi da documentare** (è il valore dell'articolo): GitLab CE pesa
  (~4 GB RAM, boot lento); i nodi kind devono fidarsi del registry di GitLab CE
  (insecure registry su containerd); raggiungibilità webhook GitLab→EventListener
  (stessa rete Docker o ingress-nginx con `extraPortMappings`); Argo CD verso il
  repo in http self-signed.
- **Formato video**: la catena è dimostrabile a schermo → registrare e pubblicare
  anche come video. Registrazione con **OBS** (già lo strumento abituale
  dell'utente); i segmenti solo-terminale meglio con **asciinema** (file di
  testo, player embeddabile, `agg` per GIF). Vincolo repo: niente binari → il
  video va su host esterno (YouTube/PeerTube/GitLab) ed embeddato nel post.
  Servirebbe un modo pulito per l'embed (partial/hook o snippet `attr_list` +
  iframe) — da valutare come feature del blog se il video diventa ricorrente.
- **Probabile serie, non un pezzo unico** (da decidere): taglio possibile in
  3 — (1) ambiente: kind + GitLab CE su Docker; (2) CI con Tekton: Task/Pipeline/
  Trigger + webhook + build&push; (3) CD con Argo CD: modello pull, Application,
  kustomize, chiusura del loop. Eventuale 4º di recap end-to-end + mapping agli
  operator OpenShift + video. Slug con prefisso comune (`gitops-…`), pubblicati
  insieme con cross-link.

### Altri spunti dai repo in `~/Projects/personal/` (categoria Java, priorità bassa)

I repo Java in `debosciaty/` e `koji-java-projects/` sono quasi tutti spike o
prove tecniche incompiute: nessuna opinione maturata, niente "aha" fuori dalla
doc → per il triage vanno scartati **finché non vengono finiti** con un taglio
preciso. I tre sotto sono gli unici con un angolo possibile.

- `telemetry-ingress` — spike Quarkus **solo producer** (REST → topic
  `telemetry.takeovers`), client Vert.x Kafka diretto + `ObjectSerializer`
  custom, gotcha di serializzazione (`BigDecimal`, `java.time`). Java 18 /
  Quarkus 2.9 (2022). Da finire (manca il consumer) prima di farne un articolo.
- `idea-plugin-demo` — sperimentazione plugin IntelliJ. Visione: da OpenAPI/Swagger
  generare codice + documentazione e poi **eseguire quelle API integrandosi con
  Postman**, tutto dentro l'IDE. Nel repo è arrivato il primo pezzo: un runner di
  collection Postman (package `it.koji.postman`, reimplementa `pm`/console, action
  + tool window). Il generatore da OpenAPI è rimasto un'idea. Angolo: imparare
  l'IntelliJ Platform SDK / cosa serve per rifare il runtime `pm.*`.
- `newman-reporter-summary` — reporter Newman pubblicato su npm, tabella
  solo-ASCII. Articolo breve.
- `spring-test-generator-maven-plugin` — spike per studiare **come si scrive un
  plugin Maven**. Un solo dump da 1536 righe poi abbandonato, ma con sostanza:
  Mojo (`GenerateTests`, `ProbeTests`), `ControllerInterceptor`, e gli
  integration test in `src/it/` (harness `maven-invoker-plugin` + `verify.groovy`).
  Idea: generare test Spring intercettando i controller. Angolo se ripreso:
  anatomia di un Mojo + IT con l'invoker plugin.
- `rest-kotlin-service` — spike per **imparare Kotlin**: porting su Quarkus +
  Kotlin di un wrapper che l'utente aveva per un cliente (integrazione REST, spec
  OpenAPI da Apicurio, MapStruct). Nel `pom.xml` è visibile l'attrito nello
  scegliere le estensioni REST/serializzazione giuste per Kotlin
  (`rest-kotlin-serialization` vs `resteasy-jackson` vs `jackson-module-kotlin`).
  Angolo: cosa cambia portando un wrapper Java a Kotlin (data class, null-safety
  al confine API, MapStruct con Kotlin). **Se scritto: anonimizzare** — il
  package cita un cliente reale.

- `test-k8` (+ `kube-test1`, fratello che si sovrappone) — spike per capire il
  **deploy di un pod su k8s e le secrets**. Allo stato è esile: un Deployment
  base + `secret-regcred` (`imagePullSecrets` per registry GitLab privato) + un
  file `commands` di kubectl. Da **ampliare o rifare** prima di un articolo:
  aggiungere Service, probe, `resources`, secret come env / volume, e raccontare
  i modi di gestire le secret (opaque, docker-registry, `envFrom`, montaggio).
  Angolo: "cosa ho capito facendo il primo deploy su k8s a mano".

### Minori

- I pulsanti Precedente/Successivo in fondo alle pagine seguono un ordine vecchio
  (il plugin li cabla prima del hook `on_nav`).
- Valutare una sezione "Reference" separata per i materiali non-articolo (checklist,
  recon report, cheat-sheet).
- Se il post più recente sarà di categoria Sicurezza (cover rossa), l'anello rosso
  `--latest-accent` della card featured avrà poco contrasto — eventualmente cambiare tinta.

---

**Autore**: koji76
