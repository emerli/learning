---
date: 2026-04-24
categories:
  - Docker
slug: containers-devcontainer-devops-lab
lab: true
description: "Il mio devcontainer per DevOps: Fedora 43 con kubectl, oc, tkn, argocd e kustomize."
tags:
  - Docker
  - DevContainer
  - Kubernetes
  - Tekton
  - ArgoCD
  - Ansible
  - Lab
title: "DevContainer DevOps: lab"

---

# DevContainer DevOps: lab

Tekton, ArgoCD, OpenShift, Ansible: il Dependency Hell di un ambiente DevOps multi-tool, rinchiuso in un container. Un rebuild e hai le versioni che servono per il cliente — con l'host immacolato.

<!--more-->

Il cliente per cui lavoro utilizza OpenShift con pipeline che parte da GitLab che chiama Tekton e ArgoCD, varie automatizzazioni con Ansible.
Per ognuno di questi tool è richiesta una versione specifica di Python, Node ecc.
Ho scritto questo devcontainer per progetti DevOps: pipeline Tekton, deploy su OpenShift, GitOps con Argo CD e Ansible per le automatizzazioni. Zero Dependency Hell sull'host.

Per capire meglio il funzionamento dei DevContainer ho scritto [DevContainer: guida pratica](/containers-devcontainer-guida-pratica/).

## devcontainer.json

Ecco il contenuto del mio file `devcontainer.json` (.devcontainer/devcontainer.json):

```jsonc
{
  "name": "DevOps - Tekton/OpenShift/ArgoCD",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "remoteUser": "root",
  "containerUser": "root",
  "mounts": [
    "source=${localEnv:HOME}/.kube,target=/root/.kube,type=bind,consistency=cached",
    "source=${localEnv:HOME}/.config/opencode,target=/root/.config/opencode,type=bind,consistency=cached"
  ],
  "customizations": {
    "vscode": {
      "extensions": [
        "redhat.vscode-yaml",
        "ms-kubernetes-tools.vscode-kubernetes-tools",
        "timonwong.shellcheck",
        "foxundermoon.shell-format",
        "eamodio.gitlens",
        "pkief.material-icon-theme",
        "anthropic.claude-code"
      ],
      "settings": {
        "editor.formatOnSave": true,
        "editor.tabSize": 2,
        "yaml.schemaStore.enable": true,
        "yaml.schemas": {
          "https://json.schemastore.org/kustomization.json": [
            "kustomization.yaml",
            "kustomization.yml"
          ],
          "https://raw.githubusercontent.com/redhat-developer/vscode-tekton/refs/heads/main/scheme/tekton.dev/v1_Pipeline.json": [
            "**/pipeline-*.yaml",
            "**/pipeline-*.yml"
          ],
          "https://raw.githubusercontent.com/redhat-developer/vscode-tekton/refs/heads/main/scheme/tekton.dev/v1_Task.json": [
            "tekton/tasks/*.yaml",
            "tekton/tasks/*.yml"
          ],
          "https://raw.githubusercontent.com/redhat-developer/vscode-tekton/refs/heads/main/scheme/tekton.dev/v1_PipelineRun.json": [
            "**/trigger-*.yaml",
            "**/trigger-*.yml"
          ]
          // kubernetes schema non disponibile su schemastore - gestito da ms-kubernetes-tools quando connesso al cluster
        },
        "vs-kubernetes": {
          "vs-kubernetes.kubectl-path": "/usr/local/bin/kubectl"
        }
      }
    }
  },
  "containerEnv": {
    "PATH": "/root/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
  }
}
```

## Spiegazione

Gli elementi del `devcontainer.json` che vale la pena spiegare:

- **`name`** — il nome del progetto mostrato nell'IDE.
- **`build`** — punta al `Dockerfile` con Fedora 43 e tutte le CLI (vedi sotto).
- **`mounts`** — le config di Opencode condivise tra host e container. Il mount di `.kube` : il kubeconfig contiene credenziali e contesti
- **`extensions`** — YAML con schema store, Kubernetes tools, shellcheck e shell-format per gli script, GitLens.
- **`yaml.schemas`** — il punto chiave: gli schema JSON per i file YAML di Kustomize e Tekton (Pipeline, Task, PipelineRun). Senza, scrivi YAML di CRD senza autocompletamento né validazione. Lo schema di Kubernetes non è su schemastore: ci pensa l'estensione `ms-kubernetes-tools` quando sei connesso al cluster.
- **`vs-kubernetes.kubectl-path`** — dice all'estensione Kubernetes dove trovare kubectl nel container.

## Immagine

Ho preferito utilizzare come immagine di base Fedora 43 che contiene le ultime versioni di Ansible e dei tool cloud.

Il `Dockerfile`  installa tutte le CLI dal sito ufficiale di ciascuna (.devcontainer/Dockerfile):

```dockerfile
FROM docker.io/library/fedora:43

RUN dnf install -y \
    bash \
    curl \
    wget \
    git \
    jq \
    unzip \
    tar \
    gzip \
    openssh-clients \
    && dnf clean all

# kubectl
RUN curl -LO "https://dl.k8s.io/release/$(curl -Ls https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl" \
    && chmod +x kubectl \
    && mv kubectl /usr/local/bin/

# oc (OpenShift CLI)
RUN curl -LO https://mirror.openshift.com/pub/openshift-v4/clients/ocp/stable/openshift-client-linux.tar.gz \
    && tar -xzf openshift-client-linux.tar.gz \
    && mv oc kubectl /usr/local/bin/ \
    && rm openshift-client-linux.tar.gz

# tkn (Tekton CLI)
RUN curl -LO https://mirror.openshift.com/pub/openshift-v4/clients/pipeline/latest/tkn-linux-amd64.tar.gz \
    && tar -xzf tkn-linux-amd64.tar.gz --no-same-owner \
    && mv tkn /usr/local/bin/ \
    && rm tkn-linux-amd64.tar.gz

# argocd CLI
RUN curl -sSL -o /usr/local/bin/argocd https://github.com/argoproj/argo-cd/releases/latest/download/argocd-linux-amd64 \
    && chmod +x /usr/local/bin/argocd

# kustomize
RUN KUSTOMIZE_VERSION=$(curl -s https://api.github.com/repos/kubernetes-sigs/kustomize/releases/latest | jq -r '.tag_name' | sed 's/kustomize\///') \
    && curl -LO "https://github.com/kubernetes-sigs/kustomize/releases/download/kustomize%2F${KUSTOMIZE_VERSION}/kustomize_${KUSTOMIZE_VERSION}_linux_amd64.tar.gz" \
    && tar -xzf kustomize_${KUSTOMIZE_VERSION}_linux_amd64.tar.gz --no-same-owner \
    && mv kustomize /usr/local/bin/ \
    && rm kustomize_${KUSTOMIZE_VERSION}_linux_amd64.tar.gz
```

Le CLI installate:

| CLI | A cosa serve |
|---|---|
| `kubectl` | Gestire risorse Kubernetes |
| `oc` | CLI di OpenShift (kubectl + comandi OpenShift) |
| `tkn` | Gestire Pipeline e Task Tekton |
| `argocd` | Gestire le Application di Argo CD |
| `kustomize` | Renderizzare le overlay di Kustomize |

## Cosa ho notato

- Tutte le CLI nel container tengono l'host pulito: per aggiornare le versioni basta un rebuild.
- Gli schema YAML per Tekton e Kustomize fanno la differenza: senza, scrivi YAML di CRD al buio.
- Le CLI vengono scaricate "latest" al momento del build: un rebuild a distanza di mesi può cambiare versione. Se serve riproducibilità, conviene pinnare le versioni.
- Il mount di `.kube` è comodo ma lo lascio commentato di default: il kubeconfig contiene credenziali, meglio decidere esplicitamente quando condividerlo.
