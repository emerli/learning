---
date: 2026-04-24
categories:
  - Containers
slug: containers-devcontainer-java-lab
lab: true
description: "Il mio devcontainer per Java/Quarkus: Maven, JDK 17, hot code replace e mount .m2."
tags:
  - Containers
  - DevContainer
  - Java
  - Quarkus
  - Lab
title: "DevContainer Java/Quarkus: lab"

---

# DevContainer Java/Quarkus: lab

Moltissimi progetti Java/Maven/Quarkus identici tra loro, un solo setup: mount .m2, generated-sources e hot code replace. La configurazione perfezionata nel tempo, pronta per il team.

<!--more-->

Questo devcontainer l'ho scritto per un cliente con molti progetti Java/Maven/Quarkus molto simili tra loro. Nel tempo è stato perfezionato fino alla versione che vedete qui.

Per capire meglio il funzionamento dei DevContainer ho scritto [DevContainer: guida pratica](/containers-devcontainer-guida-pratica/).

## devcontainer.json

Ecco il contenuto del mio file `devcontainer.json` (.devcontainer/devcontainer.json):

```json
{
  "name": "Progetto - Java 17",
  "runArgs": [
    "--name",
    "test-project-java",
    "--dns=10.16.0.31",
    "-p",
    "8080:8080",
    "-p",
    "5005:5005"
  ],
  "image": "docker.io/library/maven:3.9-eclipse-temurin-17",
  "remoteUser": "root",
  "containerUser": "root",
  "mounts": [
    "source=${localEnv:HOME}/.m2,target=/root/.m2,type=bind,consistency=cached",
    "source=${localEnv:HOME}/.config/opencode,target=/root/.config/opencode,type=bind,consistency=cached"
  ],
  "forwardPorts": [
    8080,
    5005
  ],
  "portsAttributes": {
    "8080": {
      "label": "Service",
      "onAutoForward": "notify"
    },
    "5005": {
      "label": "Debug",
      "onAutoForward": "notify"
    }
  },
  "customizations": {
    "jetbrains": {
      "backend": "IntelliJ"
    },
    "vscode": {
      "extensions": [
        "vscjava.vscode-java-pack",
        "redhat.vscode-quarkus",
        "redhat.vscode-xml",
        "redhat.vscode-yaml",
        "ryanluker.vscode-coverage-gutters",
        "eamodio.gitlens",
        "pkief.material-icon-theme",
        "sst-dev.opencode",
        "continue.continue"
      ],
      "unwanted": [
        "GitHub.copilot",
        "GitHub.copilot-chat"
      ],
      "settings": {
        "github.copilot.enable": {
          "*": false
        },
        "java.jdt.ls.java.home": "/opt/java/openjdk",
        "java.configuration.runtimes": [
          {
            "name": "JavaSE-17",
            "path": "/opt/java/openjdk",
            "default": true
          }
        ],
        "java.project.sourcePaths": [
          "src/main/java",
          "src/test/java",
          "target/generated-sources/annotations",
          "target/generated-sources/openapi",
          "target/generated-sources"
        ],
        "editor.formatOnSave": true,
        "editor.tabSize": 4,
        "java.debug.settings.hotCodeReplace": "auto",
        "quarkus.tools.debug.terminateProcessOnExit": "Ask",
        "coverage-gutters.showLineCoverage": true,
        "coverage-gutters.showRulerCoverage": true,
        "java.debug.settings.showStaticVariables": true,
        "java.debug.settings.showQualifiedNames": false,
        "java.debug.settings.maxStringLength": 0,
        "java.debug.settings.forceBuildBeforeLaunch": false,
        "java.debug.settings.expandLazyVariables": true
      }
    }
  },
  "postCreateCommand": "mvn dependency:resolve && curl -fsSL https://opencode.ai/install | bash",
  "containerEnv": {
    "QUARKUS_HTTP_HOST": "0.0.0.0",
    "PATH": "/root/.opencode/bin:/root/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
  }
}
```

## Spiegazione

Gli elementi del `devcontainer.json` che vale la pena spiegare:

- **`name`** — il nome del progetto mostrato nell'IDE.
- **`image`** — l'immagine ufficiale Maven con JDK 17 (Eclipse Temurin): contiene già tutto il necessario per lo sviluppo Java, quindi niente `Containerfile`.
- **`runArgs`** — nome esplicito del container, `--dns` aziendale (senza, in VPN dal cliente il container non risolve gli host interni) e publish diretto delle porte 8080 (servizio) e 5005 (debug).
- **`mounts`** — il repository Maven locale (`~/.m2`) condiviso col container per non riscaricare le dipendenze a ogni rebuild, più la config di OpenCode.
- **`forwardPorts`** — 8080 per il servizio, 5005 per il debug remoto, entrambe con notifica al forward.
- **`customizations.jetbrains.backend`** — lo stesso container si apre anche con IntelliJ via JetBrains Gateway.
- **`extensions`** — Java Extension Pack, tooling Quarkus, XML/YAML, coverage gutters, GitLens, OpenCode e Continue.
- **`unwanted`** — Copilot e Copilot Chat esplicitamente disinstallati: uso OpenCode/Continue e le estensioni entrano in conflitto.
- **`settings`** — runtime JavaSE-17 puntato a `/opt/java/openjdk`, source path che includono le generated-sources (OpenAPI generator, annotation processor), hot code replace `auto` per il debug live.
- **`postCreateCommand`** — `mvn dependency:resolve` scalda la cache Maven, poi installa OpenCode nel container così può lavorare sui sorgenti.
- **`containerEnv`** — `QUARKUS_HTTP_HOST=0.0.0.0` per rendere il servizio raggiungibile da fuori il container.

## Immagine

Il devcontainer utilizza l'immagine `maven:3.9-eclipse-temurin-17`, che contiene già tutto il necessario per lo sviluppo Java: niente `Containerfile` o `Dockerfile`, meno manutenzione.

## Video

Come si vede nel filmato, occorre solo aprire il progetto e autorizzare l'esecuzione nel container e parte la creazione del container e il reload dell'ide

{{< video src="/screen/devcontainer-java-lab-create.mp4" >}}

Riaprendo successivamente l'ide si nota come sia immediato il caricamento in quanto il container esistente viene risvegliato.

{{< video src="/screen/devcontainer-java-lab-reopen.mp4" >}}

## Cosa ho notato

- L'immagine ufficiale Maven basta e avanza: saltare il `Containerfile` dimezza la manutenzione.
- Il mount di `~/.m2` è il vero risparmio: senza, ogni rebuild del container riscarica tutte le dipendenze.
- Il `--dns` aziendale è stato il primo gotcha incontrato: senza, il container non risolveva gli host interni del cliente.
- L'hot code replace sulla 5005 funziona bene: si debugga come in locale.
- Disabilitare Copilot via `unwanted` + settings evita i conflitti con OpenCode/Claude.
