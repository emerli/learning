---
date: 2026-04-24
categories:
  - Containers
slug: java-quarkus-devcontainer
description: "Il mio devcontainer per Java/Quarkus: Maven, JDK 17, hot code replace e mount .m2."
---

# devcontainer.json — java quarkus devcontainer

Ecco il contenuto del mio file `devcontainer.json` generico (.devcontainer/devcontainer.json):

<!-- more -->

```text
{
  "name": "Sir TPL Cotral - Java 17",
  "runArgs": [
    "--name",
    "sir-tpl-cotral-java",
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
    "source=${localEnv:HOME}/Projects/Tabaccai/m2,target=/root/.m2,type=bind,consistency=cached",
    "source=${localEnv:HOME}/.config/opencode,target=/root/.config/opencode,type=bind,consistency=cached",
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
      "label": "Service",
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
