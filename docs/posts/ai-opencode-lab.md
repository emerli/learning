---
date: 2026-08-31
categories:
  - AI
slug: ai-opencode-lab
lab: true
draft: true
description: "Configurazione di OpenCode con Ollama come provider e test pratici di un agente di coding completamente locale."
---

# OpenCode: come usarlo al meglio

<!-- more -->

## Introduzione

Vediamo come installare Ollama e Opencode su linux e come configurarli al meglio.

Vale la pena ricordare che Ollama funziona al meglio in modalità GPU su una scheda nvidia.
Ollama può funzionare anche in modalità GPU+CPU o CPU nel caso in cui il modello sia più grande della VRAM a disposizione o sia assente penalizzando di molto le performance.

Vedi [Ollama LLM Locali](ai-ollama-llm-locali.md) per i dettagli.


## Installazione

### Prerequisiti

Il requisito principale è avere i driver proprietari nvidia installati e funzionanti.
Per verificare la presenza e la quantità di VRAM presente:

```bash
nvidia-smi
```

e naturalmente saper utilizzare bash.

### Ollama

Per prima cosa installiamo Ollama

```bash
curl -fsSL https://ollama.com/install.sh | bash
```

Avviamo Ollama:

```bash
ollama serve
```

Scarichiamo il primo modello un modello:

```bash
ollama pull qwen3.5:4b
```

per avere una lista dei modelli disponibili e capirne le differenze si può consultare [Ollama official site](https://ollama.com/library?sort=newest) 

Per verificare la lista dei modelli gia installati:

```bash
ollama list
```

Per eseguire il modello :

```bash
ollama run qwen3.5:4b
```

Ora si può inserire prompt nella shell e ottenere le risposte dal modello eseguito localmente.
Per uscire dal run basta inserire 

```
/bye
```


### OpenCode

Prima cosa installiamo Opencode

```bash
curl -fsSL https://opencode.ai/install | bash
```

Entriamo nella directory di progetto e apriamo Opencode

```bash
opencode
```

Si apre un interfaccia TUI e qui possiamo iniziare a dare prompt nella directory di progetto.

## Configurazione

Le configurazioni di opencode come da standard nel mondo linux sono /home/$USER/.config/opencode.

Per vedere come configurarlo nella pratica potete trovare nel mio repository, le configurazioni che uso per lavoro ai seguenti link [Agents.md](https://gitlab.com/koji-ai-projects/configurazioni-utili-ai/-/blob/0f7b1cb268561799fc6a046efdac05f5d8c0c780/AGENTS.md) e la configurazione degli agenti [agents](https://gitlab.com/koji-ai-projects/configurazioni-utili-ai/-/tree/0f7b1cb268561799fc6a046efdac05f5d8c0c780/agents).

Se volete saperne di piu  sul funzionamento di opencode ho scritto questi articoli [Opencode](ai-opencode.md) e [Agents.md](ai-agents-md.md) .

### `opencode.json`

### Modelli testati

## Test pratici

### Tool calling

### Modifiche multi-file

## Attriti

## Cosa ho notato
