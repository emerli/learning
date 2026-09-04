---
date: 2026-04-24
categories:
  - Containers
slug: containers-devcontainer-mono-lab
lab: true
description: "Il mio devcontainer per Mono/.NET 4.7: Debian bookworm, NuGet, NUnit e OmniSharp legacy."
tags:
  - Containers
  - DevContainer
  - .NET
  - Mono
  - Lab
title: "DevContainer Mono/.NET 4.7: lab"

---

# DevContainer Mono/.NET 4.7: lab

.NET Framework 4.7 su Linux non esiste — ecco perché serve Mono, e perché serve un container dedicato per toccare un progetto legacy senza sporcarsi la macchina.

<!--more-->

Questo devcontainer l'ho scritto per un cliente quando mi sono trovato a modificare un progetto .NET Framework 4.7.

Microsoft non rilascia .NET Framework 4.x per Linux, quindi la community ha sviluppato Mono. Oggi Mono è meno mantenuto e non tutte le distribuzioni Linux lo pacchettizzano in maniera completa — da qui la necessità di un container ad hoc.

Per capire meglio il funzionamento dei DevContainer ho scritto [DevContainer: guida pratica](/containers-devcontainer-guida-pratica/).

## devcontainer.json

Ecco il contenuto del mio file `devcontainer.json` (.devcontainer/devcontainer.json):

```json
{
  "name": "Progetto - Mono",
  "build": {
    "context": ".",
    "dockerfile": "Containerfile"
  },
  "workspaceFolder": "/workspace",
  "workspaceMount": "source=${localWorkspaceFolder},target=/workspace,type=bind,consistency=cached",
  "remoteUser": "root",
  "containerUser": "root",
  "mounts": [
    "source=${localEnv:HOME}/.config/opencode,target=/root/.config/opencode,type=bind,consistency=cached"
  ],
  "containerEnv": {
    "PATH": "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
  },
  "forwardPorts": [
    5000,
    55555
  ],
  "portsAttributes": {
    "5000": {
      "label": "Service",
      "onAutoForward": "silent"
    },
    "55555": {
      "label": "Debugger",
      "onAutoForward": "silent"
    }
  },
  "customizations": {
    "jetbrains": {
      "backend": "Rider"
    },
    "vscode": {
      "extensions": [
        "ms-dotnettools.csharp",
        "ms-dotnettools.vscode-dotnet-runtime",
        "ms-vscode.mono-debug",
        "fernandoescolar.vscode-solution-explorer",
        "jmrog.vscode-nuget-package-manager",
        "eamodio.gitlens",
        "pkief.material-icon-theme",
        "anthropic.claude-code"
      ],
      "settings": {
        "dotnet-test-explorer.testProjectPath": "Progetto.IntegrationTests/Progetto.IntegrationTests.csproj",
        "omnisharp.useModernNet": false,
        "omnisharp.monoPath": "/usr/bin/mono",
        "omnisharp.path": "latest",
        "csharp.suppressDotnetInstallWarning": true,
        "csharp.suppressDotnetRestoreNotification": true,
        "editor.formatOnSave": true,
        "editor.tabSize": 4,
        "[csharp]": {
          "editor.defaultFormatter": "ms-dotnettools.csharp"
        }
      }
    }
  },
  "postCreateCommand": "nuget restore Progetto.sln || true"
}
```

## Spiegazione

Gli elementi del `devcontainer.json` che vale la pena spiegare:

- **`name`** — il nome del progetto mostrato nell'IDE.
- **`build`** — punta al `Containerfile` custom: non esiste un'immagine pubblica completa per lo sviluppo Mono (vedi sotto).
- **`workspaceFolder` / `workspaceMount`** — il workspace dell'host viene montato in `/workspace` nel container.
- **`mounts`** — condivido la config di OpenCode (`~/.config/opencode`) tra host e container.
- **`forwardPorts`** — la 5000 espone il servizio in run, la 55555 è il debugger Mono. Entrambe con `onAutoForward: silent`.
- **`customizations.jetbrains.backend`** — lo stesso container si apre anche con Rider via JetBrains Gateway.
- **`extensions`** — le estensioni per il .NET legacy: C# con OmniSharp, runtime .NET, debugger Mono, solution explorer, gestione pacchetti NuGet, GitLens.
- **`settings`** — il punto chiave è `omnisharp.useModernNet: false`: forza OmniSharp in modalità legacy su Mono, con `omnisharp.monoPath` puntato a `/usr/bin/mono`. Senza, il language server non gestisce i progetti .NET Framework 4.7.
- **`postCreateCommand`** — `nuget restore Progetto.sln || true`: a container creato scarica le dipendenze NuGet; `|| true` evita che il setup si fermi se un package non è risolvibile.

## Immagine

Non ho trovato un'immagine già pronta e completa per lo sviluppo Mono, quindi ho scritto un `Containerfile` ad hoc (.devcontainer/Containerfile):

```dockerfile
FROM docker.io/debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    curl \
    unzip \
    sed \
    dirmngr \
    ca-certificates \
    gnupg \
    nodejs \
    npm \
    && apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 \
        --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF \
    && echo "deb https://download.mono-project.com/repo/debian/ stable-buster main" \
        > /etc/apt/sources.list.d/mono-official-stable.list \
    && apt-get update \
    && apt-get install -y \
        mono-complete \
        mono-xsp4 \
        mono-dbg \
        msbuild \
        nunit-console \
        git \
        nano \
        bash-completion

# NuGet aggiornato da Microsoft (quello Debian è troppo vecchio)
RUN curl -o /usr/local/bin/nuget.exe https://dist.nuget.org/win-x86-commandline/latest/nuget.exe \
    && printf '#!/bin/bash\nmono /usr/local/bin/nuget.exe "$@"\n' > /usr/local/bin/nuget \
    && chmod +x /usr/local/bin/nuget

RUN nuget install NUnit.ConsoleRunner -Version 3.18.3 -OutputDirectory /opt/nunit && \
    echo '#!/bin/bash\nmono /opt/nunit/NUnit.ConsoleRunner.3.18.3/tools/nunit3-console.exe --framework=mono-4.0 "$@"' > /usr/local/bin/nunit3-console && \
    chmod +x /usr/local/bin/nunit3-console

RUN curl -fsSL https://opencode.ai/install | bash
```

In sostanza parto da Debian, aggiungo il repository ufficiale di Mono e installo i pacchetti che servono: `mono-complete`, `xsp4` (il web server), il debugger, `msbuild` e la console di NUnit. Poi:

- **NuGet**: quello nei repo Debian è troppo vecchio, quindi scarico l'exe ufficiale da Microsoft e lo wrappo in uno script che lo esegue con Mono.
- **NUnit**: installo il ConsoleRunner via NuGet e creo il wrapper `nunit3-console` che gira su `mono-4.0`.
- **OpenCode**: installato nel container per lavorare sui sorgenti.

## Cosa ho notato

- Senza il `Containerfile` ad hoc non si va da nessuna parte: non esiste un'immagine pubblica completa per Mono con msbuild e NUnit.
- `omnisharp.useModernNet: false` è lo snodo di tutto: senza, il language server cerca il .NET moderno e sui progetti 4.7 non capisce nulla.
- Il primo avvio è lento (`mono-complete` pesa), dal secondo in poi è immediato.
- Aprire un progetto .NET Framework 4.7 su Linux senza passare da Windows è fattibile, ma il setup va conosciuto: non è "apri e vai".
