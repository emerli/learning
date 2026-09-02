---
date: 2026-04-24
categories:
  - Containers
slug: mono-net47-devcontainer
description: "Il mio devcontainer per Mono/.NET 4.7: Debian bookworm, NuGet, NUnit e OmniSharp legacy."
---

# devcontainer.json — mono net47 devcontainer

Ecco il contenuto del mio file `devcontainer.json` generico (.devcontainer/devcontainer.json):

<!-- more -->

```text
{
  "name": "ToscanaFei Mono",
  "build": {
    "context": ".",
    "dockerfile": "Containerfile"
  },
  "workspaceFolder": "/workspace",
  "workspaceMount": "source=${localWorkspaceFolder},target=/workspace,type=bind,consistency=cached",
  "remoteUser": "root",
  "containerUser": "root",
  "runArgs": [
    "--network=tabaccai_default"
  ],
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
        "dotnet-test-explorer.testProjectPath": "ToscanaFei.IntegrationTests/ToscanaFei.IntegrationTests.csproj",
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
  "postCreateCommand": "nuget restore Toscana.sln || true"
}

```

# Containerfile — mono net47 devcontainer

Ecco il contenuto del mio file `Containerfile` generico (.devcontainer/Containerfile):

```text
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
