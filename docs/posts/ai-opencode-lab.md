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
#### Installazione
Per prima cosa installiamo Ollama

```bash
curl -fsSL https://ollama.com/install.sh | bash
```

Scarichiamo il primo modello:

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


<asciinema-player src="../shell/ollama-setup.cast" speed="1.5"></asciinema-player>


### OpenCode
#### Installazione
Prima cosa installiamo Opencode

```bash
curl -fsSL https://opencode.ai/install | bash
```

Entriamo nella directory di progetto e apriamo Opencode

```bash
opencode
```

Si apre un interfaccia TUI in cui iniziare a scrivere i  prompt  nella directory di progetto.

In basso possiamo vedere la modalità plan o build e di fianco il nome del modello attivo.

#### I providers

Premendo ctrl+p accediamo al menu e selezionando "switch model" si aprirà la finestra per selezionare il modello attivo.

Nella finestra dei modelli accanto ai modelli gratuiti c'è scritto Free e sono i modelli gratuiti del provider "Opencode Zen".
Questi modelli sono molto interessanti perche pur essendo potenti sono gratuiti ed è possibile utilizzarli liberamente per sperimentare con le AI.
Ci sono ovviamente provider a pagamento con diversi piani,modelli e limiti.

Io personalmente utilizzo "Opencode GO" che da diversi modelli comparabili con Claude o ChatGPT ad un prezzo inferiore.

#### Opencode.json

Nella lista non compare il provider Ollama (locale) e il modello Qwen3.5:4b che abbiamo installato su ollama perchè dovrà essere aggiunto manualmente alla configurazione di Opencode:
quindi chiudiamo opencode con ctrl+c ed editiamo opencode.jsonc

```bash
nano ~/.config/opencode/opencode.jsonc
```

incolliamo per questo esempio la configurazione seguente che fa vedere qwen3.5:4b a opencode

```json
{
  "provider": {
    "ollama": {
      "npm": "@ai-sdk/openai-compatible",
      "name": "Ollama",
      "options": {
        "baseURL": "http://127.0.0.1:11434/v1",
        "apiKey": "ollama"
      },
      "models": {
        "qwen3.5:4b": {
          "name": "qwen3.5:4b"
        }
      }
    }
  },
  "permission": {
    "*": "allow",
    "edit": "ask"
  }
}
```


"baseURL": "http://127.0.0.1:11434/v1" è l'indirizzo esposto dall'ollama locale
"qwen3.5:4b": il modello se ne possono mettere diversi
"permission": configura cosa può fare e cosa no opencode

La configurazione che utilizzo personalmente la trovate al  seguente link [opencode.jsonc](https://gitlab.com/koji-ai-projects/configurazioni-utili-ai/-/blob/428edca91cff5bf7ae147761139cf32dca0a65e3/opencode.jsonc) 


Riapriamo Opencode

```bash
opencode
```

Premendo ctrl+p accediamo al menu e selezionando "switch model" si aprirà la finestra per selezionare il modello attivo e ora dovremmo vedere il provider Ollama con sotto qwen3.5:4b.

Ora possiamo utilizzare il modello con opencode.

Possiamo chiedere quello che vogliamo ovviamente ma il punto di forza di opencode è l'integrazione con i tools il che rende possibile analizzare file della cartella,progetti,ricerche con comandi come "Quali file ci sono in questa cartella?","mi puoi descrivere cosa fa questo progetto?"

#### La context window

Quando i prompt iniziano ad essere più corposi la context window di default (4k) non è sufficiente ed è necessario allargarla:

```bash
curl http://localhost:11434/api/create -d '{
      "model": "qwen3.5:4b-32k",
      "from": "qwen3.5:4b",
      "parameters": {
        "num_ctx": 32768,
        "temperature": 1
      }
   }'
```

Questo comando crea un nuovo modello basato sul precedente ma riconfigurato per una context window di 32k.

Chiudiamo Opencode ed editiano opencode.jsonc  

```bash
nano ~/.config/opencode/opencode.jsonc
```

Aggiungiamo il nuovo modello creato

```json
{
  "provider": {
    "ollama": {
      "npm": "@ai-sdk/openai-compatible",
      "name": "Ollama",
      "options": {
        "baseURL": "http://127.0.0.1:11434/v1",
        "apiKey": "ollama"
      },
      "models": {
        "qwen3.5:4b": {
          "name": "qwen3.5:4b"
        },
        "qwen3.5:4b-32k": {
          "name": "qwen3.5:4b-32k"
        }
      }
    }
  },
  "permission": {
    "*": "allow",
    "edit": "ask"
  }
}
```

Riapriamo opencode ora non abbiamo piu problemi.

#### La Temperatura

Chiedendo descrivi il progetto mi sono accorto che qwen3.5:4b tende ad essere un po' fantasioso e dare risposte non sempre corrette.
Ho capito che il problema è legato alla temperatura di default ad 1 che tende a far considerare al modello nella risposta anche cose meno probabili.
Nel mio caso tendeva a sviluppare acronimi inesistenti.

Per cambiare la temperatura del modello è possibile naturalmente crearne un altro modello configurato come abbiamo fatto per la ctx ed aggiungerlo all' opencode.jsonc come nuovo modello

```bash
curl http://localhost:11434/api/create -d '{
      "model": "qwen3.5:4b-32k-lowtemp",
      "from": "qwen3.5:4b",
      "parameters": {
        "num_ctx": 32768,
        "temperature": 0.1
      }
   }'
```


oppure si imposta per-request in opencode.jsonc sul modello esistente:

```json
{
  "provider": {
    "ollama": {
      "npm": "@ai-sdk/openai-compatible",
      "name": "Ollama",
      "options": {
        "baseURL": "http://127.0.0.1:11434/v1",
        "apiKey": "ollama"
      },
      "models": {
        "qwen3.5:4b": {
          "name": "qwen3.5:4b"
        },
        "qwen3.5:4b-32k": {
          "name": "qwen3.5:4b-32k",
          "options": { "temperature": 0.1 }
        }
      }
    }
  },
  "permission": {
    "*": "allow",
    "edit": "ask"
  }
}
```


Con questa configurazione la risposta risultava più prevedibile.


## Conclusione

Opencode permette di utilizzare modelli locali in Ollama con ottimi risultati, ma le risorse sono limitate dalla VRAM della scheda video. In pratica ci si può fare molte cose, ma non è paragonabile con i modelli cloud, molto più grandi e capaci di elaborazioni complesse.

Nello sviluppo quotidiano io utilizzo il provider "Opencode GO" che a fronte di un costo basso mi permette di utilizzare diversi modelli superando il limite della mia scheda video ma mantenendo un buon livello di privacy; tuttavia utilizzo ollama per task in cui la privacy è fondamentale.

## Riferimenti

Le configurazioni di opencode come da standard nel mondo linux sono /home/$USER/.config/opencode.

Per vedere come configurarlo nella pratica potete trovare nel mio repository, le configurazioni che uso per lavoro ai seguenti link [Agents.md](https://gitlab.com/koji-ai-projects/configurazioni-utili-ai/-/blob/0f7b1cb268561799fc6a046efdac05f5d8c0c780/AGENTS.md) e la configurazione degli agenti [agents](https://gitlab.com/koji-ai-projects/configurazioni-utili-ai/-/tree/0f7b1cb268561799fc6a046efdac05f5d8c0c780/agents).

Se volete saperne di piu  sul funzionamento di opencode ho scritto questi articoli [Opencode](ai-opencode.md) e [Agents.md](ai-agents-md.md) .


