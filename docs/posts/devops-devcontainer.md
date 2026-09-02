---
date: 2026-04-24
categories:
  - Containers
slug: devops-devcontainer
description: "Il mio devcontainer per DevOps: Fedora 43 con kubectl, oc, tkn, argocd e kustomize."
---

# devcontainer.json — devops devcontainer

Ecco il contenuto del mio file `devcontainer.json` generico (.devcontainer/devcontainer.json):

<!-- more -->

```text
{
  "name": "DevOps - Tekton/OpenShift/ArgoCD",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "remoteUser": "root",
  "containerUser": "root",
  "mounts": [
    // "source=${localEnv:HOME}/.kube,target=/root/.kube,type=bind,consistency=cached",
    "source=${localEnv:HOME}/.claude,target=/root/.claude,type=bind,consistency=cached",
    "source=${localEnv:HOME}/.claude.json,target=/root/.claude.json,type=bind,consistency=cached"
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
          ],
          // "https://json.schemastore.org/argo-cd-application.json": [
          //   "apps/**/*.yaml",
          //   "apps/**/*.yml"
          // ],
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

# Containerfile — devops container

Ecco il contenuto del mio file `Dockerfile` generico (.devcontainer/Dockerfile):

```text
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
