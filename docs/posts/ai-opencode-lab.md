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

<!-- more -->

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


## Il test: sette modelli a confronto

I risultati della [comparativa](ai-ollama-llm-locali.md) li avevo raccolti un po' durante l'uso quotidiano. Prima di scrivere il post ho rifatto tutto con un metodo fisso, per essere sicuro di non pubblicare cose imprecise: stesso identico progetto, stesse domande, stesso ordine, sessione pulita a ogni run.

### Il metodo

Progetto di test: l'adapter Quarkus della comparativa, con le sue due specifiche OpenAPI (quella esposta e quella del provider wrappato). Due domande in italiano, in sequenza, nello stesso giro di conversazione:

1. **"descrivimi il progetto"** — richiede esplorazione: il modello deve usare i tool per guardarsi attorno, leggere README e `pom.xml`, sintetizzare.
2. **"quali api espone"** — richiede di capire quale delle due specifiche OpenAPI è quella giusta e leggerla fino in fondo.

Sette modelli, tre context window ciascuno (default 4k, 16.384, 32.768).

### I risultati

| Modello | 4k (default) | 16.384 | 32.768 |
|---|---|---|---|
| **qwen3.5:9b** | ✗ 57s — scrive i tool-call come testo, non li esegue | ⚠ 50s — buono, ma nel ragionamento "sente" istruzioni mai date | ✓ 1m58s — **il migliore**: endpoint con parametri e codici d'errore (tempo gonfiato dall'offload: 37% dei layer in RAM, vedi log) |
| **qwen3.5:4b** | ✗ 51s — si convince che il progetto sia OpenCode stesso | ⚠ 53s — ottima descrizione, ma la seconda risposta si tronca a metà frase | ✓ 9s + 15s — eccellente, al secondo tentativo |
| **qwen3:4b** | ✗ 2m39s — loop di meta-thinking, risposta vaga | ✓ 1m25s — corretto e conciso | ✗ 2m43s — risponde in inglese senza aprire un file, poi "nessuna API trovata" |
| **qwen2.5:7b** | ✗ 9s — chiede chiarimenti invece di esplorare | ✗ 10s — chiede a me di passargli il README | ⚠ 26s — esplora l'albero dei file ma non legge nulla |
| **gemma4:e4b** | ✗ 13s — chiede chiarimenti | ✓ 25s — la sorpresa: accurato e veloce | ✓ 20s — corretto |
| **granite4.2:8b** | ✗ loop di thinking, nessun risultato | ⚠ 1m52s — corretto ma lento | ⚠ 15m28s — corretto ma inutilizzabile |
| **ministral-3:3b** | ✗ 16s — risponde con un template generico inventato | ✗ 9s — confonde il progetto con OpenCode | — non provato |

### I tempi

Ogni cella della griglia ha un tempo misurato da OpenCode (thinking ed esecuzione dei tool compresi). Un chiarimento importante prima di leggerli: i valori della prima risposta **non includono un caricamento del modello lungo** come si potrebbe pensare — dai log di Ollama il caricamento (`llama-server started in…`) richiede **1.3–6.6 secondi** in tutti i 26 caricamenti della sessione di test. Il vero costo del "freddo" è un altro: alla prima richiesta il modello processa da zero tutto il prompt di sistema di OpenCode (system prompt + lista tool in JSON Schema, ~550 token) e poi, a ogni giro di tool, l'intera cronologia che cresce; nei log si vede il prompt processing partire a ~2.600-4.200 tok/s su GPU e la prima risposta totale occupare 30-60s **di generazione**, non di I/O. La differenza tra la prima e la seconda domanda in tabella è quindi: stessa VRAM occupata, ma il modello deve produrre più output e riprocessare più input.

Le due domande, separate:

**"descrivimi il progetto"** — prima domanda, modello appena caricato:

| Modello | 4k | 16k | 32k |
|---|---|---|---|
| qwen3.5:9b | 57s | 50s | 1m58s |
| qwen3.5:4b | 51s | 53s | 31s → 9s\* |
| qwen3:4b | 2m39s | 1m25s | 2m43s |
| qwen2.5:7b | 9s | 10s | 26s |
| gemma4:e4b | 13s | 25s | 20s |
| granite4.2:8b | — | 1m52s | 15m28s |
| ministral-3:3b | 16s | 9s | — |

**"quali api espone"** — seconda domanda, stessa sessione:

| Modello | 4k | 16k | 32k |
|---|---|---|---|
| qwen3.5:9b | 3s | 31s | 26s |
| qwen3.5:4b | 2s | 10s | 15s |
| qwen3:4b | 2m54s | 11s | 1m29s |
| qwen2.5:7b | — | — | — |
| gemma4:e4b | 6s | 3s | 2s |
| granite4.2:8b | — | 23s | — |
| ministral-3:3b | 3s | 18s | — |

\* primo tentativo fallito (31s): il modello ha stampato `ls` invece di eseguirlo; il tempo indicato è quello della ripetizione.
"—" = domanda non posta (run interrotto dopo la prima risposta) o nessuna risposta prodotta (loop).

I tempi vanno letti anche nella direzione opposta: **basso non significa bravo**. Il 9s record di qwen2.5:7b è di chi non fa niente; i 2-3s di gemma sulla seconda domanda sono di chi risponde a memoria senza rileggere niente; i 2s dei qwen3.5 a 4k sono risposte abortite sul nascere. Il tempo misura quanto lavoro il modello decide di fare (letture, thinking, tool), non quanto è veloce. E quando un modello va dritto al README, la taglia conta più della context window: gemma-16k chiude in 25s quello che qwen3.5:4b-32k porta a termine in 31s più riprompt.

### Cosa dicono i log di Ollama

Il `journalctl -u ollama` della sessione di test vale più di ogni impressione: ogni caricamento stampa quanta memoria serve e **quanti layer finiscono su GPU contro RAM**. La tabella che manca a tutti i benchmark:

| Modello | ctx | Pesi (VRAM) | KV-cache ctx | Layer su GPU | Esito |
|---|---|---|---|---|---|
| qwen3.5:4b | 16k | 2.5 GB | 0.5 GB | 34/34 | 100% GPU |
| qwen3.5:4b | 32k | 2.5 GB | 1.1 GB | 34/34 | 100% GPU |
| qwen3.5:9b | 16k | 4.7 GB | 1.1 GB | 32/34 | 2 layer in RAM |
| qwen3.5:9b | 32k | 4.7 GB | 4.6 GB | 33/37 | **37% dei layer in RAM** |
| gemma4:e4b | 32k | 2.8 GB | 0.5 GB | 43/43 | 100% GPU |
| granite4.2:8b | 16k | 4.9 GB | 2.6 GB | 36/41 | 5 layer in RAM |
| granite4.2:8b | 32k | 4.9 GB | 5.1 GB | 26/41 | **63% in RAM: 10.3 GB su Host** |
| qwen3:4b | 32k | 2.4 GB | 4.6 GB | 33/37 | 4 layer in RAM |
| qwen2.5:7b | 32k | 4.2 GB | 1.8 GB | 29/29 | 100% GPU |

La riga di granite spiega i suoi 15m28s meglio di ogni altra considerazione: a 32k il KV-cache da solo (5.1 GB) supera i pesi del modello (4.9 GB), e Ollama scarica in RAM di sistema **10.3 GB** — su un laptop con 15.3 GB totali. Il modello non è solo "indeciso": è che due terzi dei suoi layer girano a velocità RAM attraverso il bus PCIe, e ogni token paga quel pedaggio. È il motivo per cui nel test comparativo i suoi blocchi di thinking erano così lenti: **la context window non è gratis**, cresce il KV-cache, non i pesi, e su 8 GB di VRAM è il KV-cache il primo a sforare.

Il caso opposto è la 9b a 4k/16k: 2 layer in RAM appena, e infatti il suo comportamento a 16k (50s) è comparabile a quello della 4b. La correlazione è netta: quando il modello sta interamente in VRAM (4b, gemma, qwen2.5 a 32k) i tempi della griglia sono onesti; quando inizia lo sforamento, i tempi esplodono ben prima che il comportamento peggiori.

Per verificarlo sul proprio setup: `ollama ps` mostra GPU/CPU split in tempo reale, oppure `journalctl -u ollama -f | grep offloaded` durante il caricamento — la riga `offloaded 26/41 layers to GPU` dice più di qualsiasi benchmark.

### I modi in cui falliscono

Più ancora delle singole celle della griglia, il valore di questi test è il catalogo dei modi in cui un modello locale fallisce quando deve fare l'agente. Sei pattern ricorrenti:

**1. Il loop di thinking.** granite4.2:8b a 4k non produce nessun risultato. qwen3:4b a 4k ragiona per 2m39s citando il proprio system prompt ("Wait, the problem says...") come se fosse il compito. Non a caso i loop peggiori sono a 4k: come visto nella comparativa, lì il prompt di OpenCode arriva troncato, e il modello si mette a fare meta-ragionamento su istruzioni monche invece che sul progetto.

**2. Il tool-call finto.** qwen3.5:9b a 4k scrive "read_file:path=README.md" come testo normale; qwen3.5:4b a 32k al primo tentativo stampa `ls -la` invece di eseguirlo. Il modello *descrive* l'azione invece di compierla: sembra competente, ma è prosa. E questo non dipende solo dal contesto tagliato — qwen3.5:4b a 32k il contesto ce l'ha tutto — è una fragilità del formato del tool-calling, il punto 3 del ciclo descritto nella comparativa.

**3. "Il progetto sono io".** qwen3.5:4b a 4k vuole andare a leggere opencode.ai per descrivere "il progetto"; ministral-3:3b a 16k risponde parlando di "opencode.ai e/o workspace di sviluppo AI generativa". Il system prompt di OpenCode parla molto di OpenCode: i modelli piccoli si convincono che il progetto da descrivere sia l'agente stesso, non la cartella in cui si trovano.

**4. Lo stack per partito preso.** qwen3:4b (a 4k e a 32k) cerca route Express.js — `router.get(...)`, `--include="*.js,*.ts"` — in un progetto Java/Maven, non trova niente e conclude "no API endpoints found"; ministral risponde con template React/Node/Express. Cercano il pattern più frequente nel training set invece di guardare il `pom.xml` che hanno davanti.

**5. Zero iniziativa.** gemma e qwen2.5:7b, a context bassa, rispondono alla domanda con una domanda: "quale progetto?", "potresti fornirmi il README?". Con i tool di esplorazione disponibili, un `ls` sarebbe bastato. Non è mancanza di conoscenza: è non considerare l'esplorazione come risposta possibile alla propria incertezza.

**6. Più contesto non è un upgrade gratuito.** qwen3:4b passa da ✓ a 16k a ✗ a 32k: con 32k a disposizione risponde subito, in inglese, senza aprire un solo file. granite a 32k ci si perde in modo diverso: 8m35s di thinking che legge *entrambe* le specifiche OpenAPI per intero, 15m28s totali per una descrizione — ma qui il log aggiunge un colpevole hardware: a 32k granite sfora i 7.6 GB di VRAM e gira con 10.3 GB in RAM (vedi tabella dei log), quindi ogni token del suo thinking paga il bus PCIe. Comportamento e hardware si sommano. Lo stesso 32k su qwen3.5 è invece il punto dove danno il meglio. La context window non è un cursore "più = meglio": va calibrata per modello.

### Chi vince legge la specifica vera

Sulla seconda domanda c'è un gradino chiaro tra i modelli che funzionano. gemma e qwen3:4b rispondono "V1Api" — corretto, ma è la riga 8 del README, che avevano già in memoria dal turno prima. Solo i qwen3.5 a 32k aprono `anime-releases-openapi.yaml` e elencano gli endpoint veri: path, parametri, codici d'errore.

Nota di coerenza con la comparativa: nel test a freddo (domanda sulle API senza preavviso) i qwen3.5 leggevano la specifica *sbagliata*. Qui la stessa domanda arriva dopo "descrivimi il progetto" nello stesso giro, e scelgono quella giusta. È la conferma pratica della regola della comparativa: costruire prima il quadro generale, poi scendere nel dettaglio.

### Cosa ho notato

- **A 4k non si salva nessuno**: sette su sette fuori gioco, ognuno col proprio modo. Il primo intervento su un agente locale non è cambiare modello: è alzare `num_ctx`.
- **16k è il minimo sindacale**, ed è già il punto dolce per gemma e qwen3:4b.
- **32k è dove i qwen3.5 danno il meglio**, e dove qwen3:4b e granite affogano: il valore di una context window grande dipende dal modello, non solo dai token disponibili.
- Il migliore in assoluto è **qwen3.5:9b-32k** (descrizione più completa, endpoint con codici d'errore in 26s), col caveat che sulla mia 4060 a 32k sfora e gira con il 37% dei layer in RAM — i suoi tempi ne risentono. Il miglior compromesso resta **qwen3.5:4b-32k**, che conferma la scelta fatta nella comparativa come setup quotidiano. La sorpresa è **gemma4:e4b-16k**: 25s per una descrizione accurata.
- Un agente di coding completamente locale, su una RTX 4060, per analizzare e descrivere un progetto **è utilizzabile davvero** — a condizione di dargli il contesto giusto: context window adeguata e domande in progressione, non una richiesta complessa a freddo.


## Conclusione

Opencode permette di utilizzare modelli locali in Ollama con ottimi risultati, ma le risorse sono limitate dalla VRAM della scheda video. In pratica ci si può fare molte cose, ma non è paragonabile con i modelli cloud, molto più grandi e capaci di elaborazioni complesse.

Nello sviluppo quotidiano io utilizzo il provider "Opencode GO" che a fronte di un costo basso mi permette di utilizzare diversi modelli superando il limite della mia scheda video ma mantenendo un buon livello di privacy; tuttavia utilizzo ollama per task in cui la privacy è fondamentale.

## Riferimenti

Le configurazioni di opencode come da standard nel mondo linux sono /home/$USER/.config/opencode.

Per vedere come configurarlo nella pratica potete trovare nel mio repository, le configurazioni che uso per lavoro ai seguenti link [Agents.md](https://gitlab.com/koji-ai-projects/configurazioni-utili-ai/-/blob/0f7b1cb268561799fc6a046efdac05f5d8c0c780/AGENTS.md) e la configurazione degli agenti [agents](https://gitlab.com/koji-ai-projects/configurazioni-utili-ai/-/tree/0f7b1cb268561799fc6a046efdac05f5d8c0c780/agents).

Se volete saperne di piu  sul funzionamento di opencode ho scritto questi articoli [Opencode](ai-opencode.md) e [Agents.md](ai-agents-md.md) .


