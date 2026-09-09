---
date: 2026-09-02
categories:
  - Docker
slug: devcontainer-guida-pratica
description: "Come funzionano i DevContainer: struttura, lifecycle e gotcha reali dai 3 container che uso ogni giorno."
tags:
  - Docker
  - DevContainer
  - VS Code
title: "DevContainer: guida pratica"

---

# DevContainer: guida pratica

Ricostruire l'ambiente di sviluppo a ogni installazione della distro è il trauma di ogni distro hopper. Con i DevContainer il setup diventa codice: censito, replicabile in zero tempo, e "sulla mia macchina funziona" sparisce dal vocabolario del team.

<!--more-->

Sono anni che utilizzo docker e più volte mi sono chiesto se sia possibile utilizzarlo per containerizzare gli ambienti di sviluppo.
Nel tempo ho fatto vari esperimenti in questo senso che si sono rivelati poco comodi e soprattutto non risolvevano il trauma di ogni distro hopper seriale ovvero: ricostruire ogni volta gli ambienti.

Con i Devcontainers ho trovato il modo di:

- Censire le configurazioni
- Replicare gli ambienti di sviluppo in zero tempo
- Azzerare l'installazione dei vari sdk, tool, utility dopo l'installazione della distro
 
la soluzione a tutti i problemi.

## Cosa sono

Un DevContainer è un ambiente di sviluppo containerizzato: il tuo IDE si attacca a un container Docker che contiene **tutto** — toolchain, SDK, CLI, estensioni, config. Chi lo apre trova lo stesso identico ambiente, indipendentemente dalla macchina host.

Chi sviluppa con diversi linguaggi sa che è necessario sulla stessa macchina installare diversi SDK, diverse versioni dello stesso SDK, diverse configurazioni dell'ambiente:
nel mio caso per lo stesso cliente ho Java, .NET 10, .NET 4 e devo condividere il tutto con i miei colleghi, possibilmente evitando il classico "sulla mia macchina funziona".

Con DevContainer, il setup è codice nel repo del singolo progetto.

## Struttura

La configurazione del DevContainer risiede nella directory `.devcontainer/` nella root del progetto.

Due file essenziali:

| File | Ruolo |
|---|---|
| `devcontainer.json` | La configurazione: nome, build/image, mount, porte, estensioni, comandi |
| `Dockerfile` / `Containerfile` | L'immagine del container (se non usi un'image pre-costruita) |

### devcontainer.json

È il cuore. Ecco le sezioni che contano davvero:

```json
{
  "name": "Nome leggibile",
  "build": { "dockerfile": "Containerfile" },
  "remoteUser": "root",
  "containerUser": "root",
  "mounts": [...],
  "forwardPorts": [...],
  "customizations": { "vscode": {...}, "jetbrains": {...} },
  "postCreateCommand": "...",
  "containerEnv": { "PATH": "..." }
}
```

### Dockerfile / Containerfile

Serve se vuoi customizzare l'immagine del container; se usi un'immagine pre-costruita (es. `maven:3.9-eclipse-temurin-17`), non ti serve.

Ovviamente è un dockerfile standard quindi puoi configurare l'immagine di base o i vari `RUN` che vengono eseguiti alla creazione del container:

```dockerfile
FROM docker.io/library/fedora:43

RUN dnf install -y bash curl wget git jq unzip \
    && dnf clean all
```

## Lifecycle

Quando apri il progetto nell'IDE, viene letto il devcontainer.json e succede automaticamente questo:

1. **Build** — Docker builda l'immagine (o scarica l'image se già definita)
2. **Create** — Crea il container dal image
3. **Mount** — Monta il workspace locale nel container (`workspaceMount`)
4. **postCreateCommand** — Esegue il comando di setup (es. `mvn dependency:resolve`, `nuget restore`)
5. **Attach IDE** — L'IDE si connette al container, installa le estensioni, applica le settings
6. **Ready** — Puoi lavorare

Da questo momento ogni operazione che l'IDE fa è reindirizzata dentro al container che sta girando.

## Key concepts

### Mounts vs WorkspaceMount

`workspaceMount` monta la root del workspace nel container. I `mounts` aggiuntivi servono per config esterne:

```json
"mounts": [
  "source=${localEnv:HOME}/.m2,target=/root/.m2,type=bind,consistency=cached",
  "source=${localEnv:HOME}/.config/opencode,target=/root/.config/opencode,type=bind,consistency=cached",
]
```

Uso il mount di `.m2` per non riscaricare tutte le dipendenze Maven a ogni rebuild. Mount di `opencode` per le config degli AI assistant.

Fatto questo ci ritroviamo la directory del progetto montata in /workspace e .m2 e opencode condivise con l'host.

### ForwardPorts

Espone le porte del container alla macchina host, se vuoi accedere a quelle porte dall'host:

```json
"forwardPorts": [8080, 5005],
"portsAttributes": {
  "8080": { "label": "Service", "onAutoForward": "notify" },
  "5005": { "label": "Debug", "onAutoForward": "notify" }
}
```

`onAutoForward: "notify"` evita che l'IDE apra il browser automaticamente quando la porta è in uso.


### containerUser / remoteUser

Chi è l'utente dentro il container. `root` va bene per dev, ma in produzione non lo faresti mai e inoltre, non tutte le immagini utilizzano gli stessi utenti per il container.

### containerEnv

Variabili d'ambiente del container. Utile per PATH custom o config specifiche:

```json
"containerEnv": {
  "QUARKUS_HTTP_HOST": "0.0.0.0",
  "PATH": "/root/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
}
```

## IDE support

Nel funzionamento dei devcontainer si apre il progetto nell'ide che si accorge del devcontainer e chiede se si vuole aprire il progetto nel devcontainer.
A quel punto, una volta creato il container, l'IDE si riapre attaccandosi al container.

In questa sezione si possono dichiarare le estensioni ed i settings dell'IDE che viene aperto nel container.

### VS Code

Supporto nativo, ben consolidato da tempo.

Le estensioni si installano nel container, non sulla macchina host. 
Le settings si applicano solo al workspace nel container.

### JetBrains

Il supporto è più recente e si basa sul JetBrains Gateway. Funziona, ma è meno maturo.

Nel `devcontainer.json`:

```json
"customizations": {
  "jetbrains": {
    "backend": "IntelliJ"
  }
}
```

## I 3 esempi che uso

Ho diversi DevContainer reali, che uso ormai praticamente in ogni progetto che sviluppo, ciascuno per uno stack diverso:

- ArgoCD, kubectl, Tekton: [DevContainer DevOps: lab](/learning/posts/containers-devcontainer-devops-lab/)
- Java, Quarkus, Maven: [DevContainer Java/Quarkus: lab](/learning/posts/containers-devcontainer-java-lab/)
- Mono 4.7 con configurazioni ad hoc: [DevContainer Mono/.NET 4.7: lab](/learning/posts/containers-devcontainer-mono-lab/)

## Gotcha

### DNS aziendali

Normalmente sviluppo nella VPN aziendale del mio cliente.

Ho dovuto aggiungere `runArgs` con il DNS esplicito per risolvere gli altri host aziendali:

```json
"runArgs": ["--dns=10.16.0.31"]
```

### .gitignore mancante per stato IDE

Se monti la config IDE da fuori (es. `.m2`, `.claude`), il container può creare file di stato che finiscono nel git. Il `.gitignore` dovrebbe escludere questi file. 

### Estensioni conflittuali

Non mi piace molto il modo in cui funziona GitHub Copilot e utilizzo, perciò, Opencode o Claude direttamente da bash non nell'ide 

Ho cercato un modo per disabilitarlo nel container:

```json
"customizations": {
  "vscode": {
    "extensions": ["sst-dev.opencode", "continue.continue"],
    "unwanted": ["GitHub.copilot", "GitHub.copilot-chat"],
    "settings": {
      "github.copilot.enable": { "*": false }
    }
  }
}
```

### Schema JSON per YAML

Per avere autocompletamento e validazione dei file YAML (Tekton, Kustomize, ArgoCD), devi configurare gli schema nel `devcontainer.json`:

```json
"yaml.schemas": {
  "https://json.schemastore.org/kustomization.json": ["kustomization.yaml"],
  "https://raw.githubusercontent.com/redhat-developer/vscode-tekton/refs/heads/main/scheme/tekton.dev/v1_Pipeline.json": ["**/pipeline-*.yaml"]
}
```

### Hot code replace

In Java, il debug hot code replace richiede porta esposta e configurazione esplicita:

```json
"forwardPorts": [5005],
"settings": {
  "java.debug.settings.hotCodeReplace": "auto"
}
```

## Cosa ho notato

**Pro:**
- Setup riproducibile al 100% — chi clona il repo ha lo stesso ambiente
- Niente più "funziona sulla mia macchina"
- Le config IDE sono nel repo, non sparse per le macchine dei dev
- Mount di `.m2` e cache simili evita riscaricare tutto a ogni rebuild

**Contro:**
- Primo avvio lento (build + postCreateCommand + install estensioni)
- JetBrains Gateway è meno maturo di VS Code
- Mount di config esterne (`${localEnv:HOME}`) può creare conflitti se le versioni dei tool non matchano
- DNS e network custom sono necessari se sei in ambienti corporate

**Verdetto:** se lavori in team o su stack multipli, i DevContainer valgono il setup iniziale. Una volta cablati, il costo di onboarding di un nuovo dev o di una nuova macchina è `git clone` + apri l'IDE. Fine.


