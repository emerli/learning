# AGENTS.md — Learning Notes

## Overview

Ispirato dal second brain ho voluto realizzare questo Blog tecnico personale (contenuti in italiano) per salvare e condividere la mia esperienza.
Costruito con **Hugo** + tema **hugo-coder**, pubblicato su **GitLab Pages**.

- **Sito**: https://koji76.gitlab.io/learning
- **Repo**: https://gitlab.com/koji76/learning
- **Stack**: Hugo (binario statico) + tema `hugo-coder` vendored in `themes/` +
  override minimi in `layouts/` + CSS/JS custom in `assets/`. RSS attivo.

---

## Struttura

```
.
├── .gitlab-ci.yml   # pipeline GitLab Pages (hugo --minify)
├── .gitignore       # public/  resources/  .hugo_build.lock
├── hugo.toml        # tema + params + menu + tassonomie
├── assets/
│   ├── css/custom.css              # stile card lista, badge LAB, tabelle, tag cloud
│   ├── css/asciinema-player.min.css # vendored (build ermetica, niente CDN)
│   └── js/custom.js                # ponte asciinema v3 (create())
│   └── js/asciinema-player.min.js  # vendored
├── layouts/
│   ├── _partials/header.html       # avatar tondo + titolo nell'header
│   ├── _partials/list.html        # lista post stile PaperMod + tag cloud
│   ├── _partials/head/extensions.html # CSS asciinema
│   └── posts/single.html          # header post: data, titolo, descrizione, tag
├── i18n/it.toml                    # "Indice" per il TOC
├── static/
│   ├── images/avatar.png
│   └── *.cast                      # screencast asciinema
├── content/
│   ├── about.md
│   ├── projects.md                 # pagina Progetti (da riempire)
│   └── posts/                      # UN .md per post, tutti qui
└── themes/hugo-coder/              # tema vendored (non modificare)
```

La home è il profilo (avatar, nome, info, social). La lista post è su `/posts/`
con tag cloud in cima. Le pagine tag (`/tags/<slug>/`) sono generate dalla
tassonomia nativa di Hugo — non si mantengono a mano.

---

## Modello di un post

Tutto parte dal singolo file `content/posts/<nome>.md`. Dal frontmatter:

| Campo | Effetto | Obbligatorio |
|-------|---------|:---:|
| `date` | ordinamento + data su card/post | **sì** |
| `title` | titolo card + `<title>` (Hugo NON lo prende dall'H1) | **sì** |
| `slug` | URL `/posts/<slug>/` — impostarlo alla creazione e non cambiarlo | no (consigliato) |
| `description` | estratto nella lista + sotto il titolo nel post | no |
| `tags` | pillole su card e post + pagina `/tags/<slug>/`; 1º valore = categoria | **sì** |
| `categories` | tassonomia separata (per ora non usata nel layout) | no |
| `lab: true` | badge "LAB" su card e post + tag `Lab` — contenuto pratico / hands-on | no |
| `draft: true` | escluso dalla build | no |
| `<!--more-->` nel corpo | taglia il summary (non usato in lista: si usa `description`) | no |

```yaml
---
date: 2026-08-29
title: "Titolo del post"
slug: nome-breve
description: "Frase breve"
tags:
  - AI
  - Ollama
  - OpenCode
lab: true
---
```

Convenzione tag: il primo valore è la categoria (`Java`, `AI`, `Containers`,
`Linux`, `Agile`, `Sicurezza Informatica`), poi 2-4 tag specifici. I post
`lab: true` hanno anche la tag `Lab`.

---

## Sviluppo locale

Hugo è un binario singolo in `~/bin/hugo` (v0.165.0 extended, scaricato dalla
release GitHub — non c'è pip sulla macchina).

```bash
~/bin/hugo server          # anteprima su http://localhost:1313/learning/
~/bin/hugo --minify        # build completa in public/ (come la CI)
```

---

## CI / GitLab Pages

Job `pages` in `.gitlab-ci.yml`, **solo su branch di default**:

1. Immagine `klakegg/hugo:0.165.0-ext-alpine` (versione pinnata)
2. `hugo --minify` → genera in `public/` (default Hugo, niente `mv`)
3. Artifact `public/`

Niente pip, niente librerie di sistema, niente cache: build in pochi secondi.

### Trappole note
- `hugo --minify` non fallisce su post senza `date`/`title` — verificare a mano.
- Il tema è vendored: aggiornarlo = sostituire `themes/hugo-coder/` e riallineare
  gli override in `layouts/`.
- Niente file binari nel repo (i `.cast` sono testo, ok).

---

## Convenzioni

- `hugo.toml`: `baseURL` → `https://koji76.gitlab.io/learning/`, `locale = "it"`.
- URL dei post: `/posts/<slug>/` (default Hugo con `content/posts/`).
- Menu: `Home · Blog · Progetti · About` (in `hugo.toml`, `languages.it.menu.main`).
- Home: profilo con avatar, `info` (Cloud Architect / Senior Developer /
  AI-assisted Development), social (GitHub, GitLab, LinkedIn, email, RSS).
- TOC: attivo su tutti i post (`[params.Entry] toc = true`), collassabile
  sotto "Indice" (i18n in `i18n/it.toml`).
- Stile card lista, badge LAB, tabelle, tag cloud: tutto in `assets/css/custom.css`.
- **Screencast**: strumenti = **OBS** per il video dello schermo + **asciinema**
  per i segmenti solo-terminale. Il `.cast` è testo → sta nel repo (in
  `static/`); editing dei tempi morti con `asciinema-edit cut`/`quantize`.
  Per un video: `asciinema play -s 1.5 -i 1` come sorgente in una scena OBS,
  oppure pre-render con `agg` → GIF → `ffmpeg` mp4. Video finito su host
  esterno (niente binari nel repo).
  - **Embed nel post**: tag dichiarativo nel markdown
    `<asciinema-player src="/nome.cast" speed="1.5"></asciinema-player>`
    (path assoluto: i cast sono serviti dalla root del sito). Player JS+CSS
    **vendored** in `assets/` (niente CDN) + `assets/js/custom.js` che fa da
    ponte.
  - **Gotcha**: asciinema-player **v3 non registra più il custom element**
    `<asciinema-player>` (era la API v2); la v3 vuole
    `AsciinemaPlayer.create(src, elemento, opts)`. Lo script ponte traduce i
    tag nel markdown in chiamate `create()` — senza, il tag resta inerte e
    nessun XHR parte. Attributi supportati dal ponte: `src`, `speed`, `loop`,
    `autoplay`.

---

## Stato

- Pubblicati (categoria AI): `ai-ollama-llm-locali`, `ai-opencode`, `ai-agents-md`,
  `ai-sdd-guida-rapida`, `ai-spec-kit-book-api`, `ai-openspec-book-api`,
  `ai-opencode-lab`, `containers-devcontainer-guida-pratica`.
- ~13 post ancora `draft: true` in `content/posts/`, da revisionare
  (`grep -l 'draft: true' content/posts/*.md`).
- Convenzione nome file/slug: prefisso categoria (`ai-…`), per ordinare la cartella.
- Pagina `content/projects.md` da riempire con i link dei progetti.

## Promozione

Strategia per portare traffico al blog. Il contenuto forte è AI locale + pratica
reale (benchmark su hardware consumer, config vere, attriti documentati).

### Canali, in ordine di resa attesa

1. **Reddit — r/LocalLLaMA e r/ollama**: `ai-ollama-llm-locali` (9 modelli su
   RTX 4060, dati reali) è il formato che funziona lì. Regola d'oro: **postare
   i dati/le conclusioni nel post stesso**, link al blog come riferimento —
   il self-promo nudo (solo link) viene downvotato.
2. **LinkedIn**: canale a maggior resa per il pubblico italiano. Formato
   "cosa ho notato" con tabella comparativa inline + link. Il cast asciinema
   renderizzato come GIF è l'hook visivo.
3. **Mastodon (fosstodon.org, mastodon.uno)**: nicchia Linux/privacy/self-hosting
   ricettiva al taglio del blog (distro hopping, Ollama, GitOps).
4. **Link dai repo GitLab**: `configurazioni-utili-ai` e simili → README che
   linka i post correlati. Chi cerca config arriva lì, costo zero.

Da evitare: Hacker News (barriera linguistica, contenuto in italiano), spam di
link nei gruppi Telegram.

### Asset: cast → GIF

Il differenziatore del blog è il formato lab con player asciinema — sui social
si promuove quello, non il testo.

```bash
agg static/nome.cast nome.gif --speed 1.5
# alternativa se la piattaforma comprime male le GIF:
# agg ... | ffmpeg → mp4
```

- La GIF/mp4 **non va nel repo** (vincolo "niente binari"): solo sul social
  o su host esterno.
- `agg` non è ancora installato sulla macchina di sviluppo (al primo uso:
  binario dalla release GitHub o `cargo install agg`).

### Piano di lancio

1. Pubblicare `ai-opencode-lab` (prima `draft: false`; il cross-link dal post
   analitico è già in `main` e punta al lab).
2. Primo lancio: `ai-ollama-llm-locali` su r/LocalLLaMA — il post con più
   potenziale del catalogo attuale.
3. Un post alla volta sui social, mai in blocco.

## TODO

### Revisione dei post in draft

Per ogni post: `slug:` corto e definitivo + `description:`; rivedere linguaggio e
accuratezza; aggiungere `<!--more-->` dopo il primo paragrafo; se è un walkthrough,
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
5. Le tag sono libere: nessuna config da aggiornare (a differenza di MkDocs).

### Articoli categoria Linux (da scrivere)

Categoria `Linux` già usata come tag ma senza post dedicati. Repo sorgente in
locale: `~/Projects/personal/koji76/{arch,debian,suse}-init-ansible` + `nixos-kde`.
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

### Post DevContainer — guida pratica (categoria Containers)

Post introduttivo + pratico (`lab: true`) su come funzionano i DevContainer,
con i 3 esempi reali nel repo (DevOps, Java/Quarkus, Mono/.NET 4.7) come
riferimento per chi vuole approfondire.

- **Contenuto**: cosa sono i DevContainer, struttura (`.devcontainer/`,
  `devcontainer.json`, `Dockerfile`/`Containerfile`), lifecycle
  (`postCreateCommand`, `mounts`, `forwardPorts`), integrazione con VS Code
  e JetBrains.
- **Angolo**: non un tutorial generico — mostra le config reali usate
  dall'autore, i gotcha incontrati (mount bind di `.m2`, `.claude.json`,
  DNS aziendali, hot code replace, estensioni conflittuali), e come
  risolvere.
- **Gotcha da documentare**: `.gitignore` mancante per lo stato IDE dei
  devcontainer (già notato nel repo `debian-init-ansible`), mount di
  config esterne (`${localEnv:HOME}`), porte e DNS, schema JSON per YAML
  (Tekton, Kustomize, ArgoCD).
- **Formato**: `lab: true`, slug `devcontainer-guida-pratica`, tag
  `Containers`, cross-link ai 3 draft esistenti come esempi completi.
- **Nota**: i 3 draft (`devops-devcontainer`, `java-quarkus-devcontainer`,
  `mono-net47-devcontainer`) restano nel repo come reference. Questo post
  è l'articolo introduttivo che li inquadra.

### Lab AI: OpenCode con Ollama — coding agent locale (categoria AI)

La parte pratica dei post pubblicati `ai-ollama-llm-locali` e `ai-opencode`:
mettere a terra la configurazione reale di OpenCode con Ollama come provider e
verificare quanto regge un agente di coding completamente locale.

- Config in `opencode.json`: provider Ollama, base URL, modello, context window.
- Test sui modelli già comparati (Qwen3.5, Qwen2.5-Coder, ecc.) con un task
  reale piccolo: tool calling, modifiche multi-file, dove si inceppa.
- Attriti da documentare: supporto tool-calling per modello, context limitato,
  velocità, thinking on/off, permessi.
- Chiusura "Cosa ho notato": un agente locale è utilizzabile? Per quali task?
- Post: `lab: true`, slug `ai-opencode-ollama-lab`, cross-link ai due post.

### Come scrivere un buon prompt con gli agenti (categoria AI)

Idea nuova. Il tema è emerso lavorando ai lab AI (OpenCode + Ollama): la
qualità dell'output dell'agente dipende molto da come è scritto il prompt.
Angolo: prompt engineering pratico per agenti di coding, non teoria da blog
generico — esempi reali di prompt falliti vs funzionanti, cosa cambia tra
istruzioni vaghe e contesto esplicito, ruolo di AGENTS.md nel condizionare le
risposte (vedi gotcha Strangler Fig del lab).

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
  L'embed asciinema è già risolto (vedi Screencast in Convenzioni); per video
  mp4 servirebbe un modo pulito (shortcode o iframe) — da valutare come feature
  del blog se il video diventa ricorrente.
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

- Valutare una sezione "Reference" separata per i materiali non-articolo (checklist,
  recon report, cheat-sheet).
- Riempire `content/projects.md` con i link dei progetti.

---

**Autore**: koji76
