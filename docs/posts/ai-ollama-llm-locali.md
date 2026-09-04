---
date: 2026-08-29
categories:
  - AI
slug: ai-ollama-llm-locali
description: "Cosa possiamo fare con Ollama: comparativa dei modelli che ci possono girare su un hardware consumer"
---

# Ollama, OpenCode e la ricerca del setup giusto

### Introduzione

Il motivo principale per usare un modello locale invece del cloud è la **privacy**: analizzare codice, mail, file e dati personali che non possono uscire dalla macchina. Ci sono altri vantaggi (costo, uso offline, nessun rate limit), ma per capacità pura un modello cloud resta superiore.

---

### Contesto

| | |
|---|---|
| **Hardware principale** | Windows / Linux Mint (dual boot) · Intel i7-13700 · 16GB RAM · NVIDIA RTX 4060 8GB |
| **Modelli di riferimento** | Qwen3.8 Max in cloud: è un modello potente che utilizzo come confronto |
| **Strumenti** | Ollama · OpenCode · Visual Studio / .NET |
| **Modelli testati** | Qwen3, Qwen3.5 4b/9b, Qwen2.5-Coder, Gemma4, Llama3.2, Phi4-mini, Ministral 3b/8b, IBM Granite4.2 |
| **Progetto test** | [Servizio Quarkus con 10 API](https://gitlab.com/koji-ai-projects/ai-test-project/-/tree/53af05efd52e7d05c872b20aa3c1ebed0de77f6c/) in cui le interfacce sono generate in Maven |

---

## Il primo test: generazione di un commit
### Introduzione

Il primo test che ho effettuato è chiedere al modello di generare un commit, semplicemente nella shell di Ollama.

### 1. Qwen3 e il primo ostacolo: il thinking non si disattiva

Il primo test effettuato con il modello **Qwen3:4b**, alla richiesta "genera un messaggio di commit per questo errore", risponde con **2.928 token**: è evidente che il problema è il thinking, e cerco di disabilitarlo passando il parametro `think: false`, documentato ufficialmente da Ollama dalla versione 0.9.0 (maggio 2025). Ma non ottengo alcun effetto: tutto il ragionamento finisce mescolato nel campo `content` della risposta, con un tag `</think>` orfano lasciato a metà nel testo, invece di essere separato correttamente.

**Conseguenza misurata:** un compito banale (generare un messaggio di commit) produceva **2.928 token** invece dei ~300 necessari — quasi dieci volte più lento per un compito che non richiedeva alcun ragionamento.

Interrogare il modello stesso sul problema ha prodotto una risposta sbagliata ma sicura di sé ("non esiste un parametro per il thinking in Ollama") — promemoria diretto di cosa significhi un'allucinazione: un modello non ha visibilità su come il software che lo ospita gestisce la sua risposta.

### 2. Qwen 2.5 come soluzione pragmatica

Non avendo il concetto di thinking, **Qwen 2.5** ha aggirato il problema alla radice.

| Modello | Velocità | VRAM | Note |
|---|---|---|---|
| Qwen 2.5:7b | ~52 tok/s | 4.7 GB | 100% GPU, nessun bug |
| Qwen3:4b (bug) | ~75 tok/s | 3.2 GB | Più veloce per token, ma genera ~10× più token inutili |

Un confronto Windows/Linux sulla stessa macchina ha rivelato un divario enorme sull'overhead della prima chiamata di rete: **~2.150 ms su Windows contro ~78 ms su Linux** (chiamate successive: 2-3 ms in entrambi i casi) — causa più probabile, l'ispezione del traffico da parte dell'antivirus alla prima connessione di un nuovo processo.

### 3. Il salto a Qwen3.5: bug risolto

Qwen3.5 è più recente, quindi decido di provare anche con **Qwen3.5:4b**: il bug del thinking risulta risolto — `think:false` produce risposta pulita (200-320 token), `think:true` genera correttamente il blocco separato.

| Configurazione | Velocità | Token generati |
|---|---|---|
| think:false | ~68 tok/s | ~220-320 |
| think:true | ~67 tok/s | ~1.700+ |

La velocità superiore rispetto a Qwen 2.5:7b (~52 tok/s) è spiegata principalmente dalla dimensione inferiore (4B contro 7B), più che dall'architettura ibrida (linear attention con Gated DeltaNet + MoE sparso), il cui beneficio è marginale a queste dimensioni ridotte — è pensata soprattutto per context lunghissimi su scala enterprise.

### Prime conclusioni

Qwen3.5 4b performa molto bene ed è in grado di eseguire task semplici in poco tempo, senza particolari problemi o incertezze. Disabilitato il thinking, è addirittura migliore di Qwen2.5-coder.

## Il secondo test: OpenCode e l'analisi del codice

### Introduzione

Il secondo test che ho effettuato è configurare i modelli per operare con OpenCode e chiedere l'analisi del progetto di test.
Occorre notare che il progetto di test ha delle complessità: è un progetto Quarkus in cui le interfacce e i modelli sono generati da una specifica OpenAPI durante la build Maven.
In particolare il progetto è un wrapper che espone delle API e ne chiama altre ed è stato scelto per mettere alla prova i vari modelli con task notoriamente difficili.

Il test prevede 2 domande in sequenza:

* "descrivimi il progetto"
* "a quali API risponde il servizio?"

### La quantizzazione

I modelli che ho usato su Ollama sono quantizzati: cosa vuol dire?

I parametri di un modello sono i pesi distribuiti su tutti i layer della rete. La quantizzazione (es. da 16 a 4 bit) riduce la precisione numerica di ciascun peso, non il numero di parametri né la struttura della rete: comprime memoria e velocizza il calcolo, ma non elimina né aggiunge conoscenza. L'assenza di training specifico, non la quantizzazione, spiega i fallimenti osservati.

### 1. Primi problemi con il context

Fin dalle prime richieste mi sono imbattuto in un errore durante l'elaborazione anche di prompt semplici con OpenCode, che causava l'impossibilità di completare la richiesta.
Andando a leggere il log di Ollama ho trovato:
```
msg="truncating input prompt" limit=2051 prompt=7647 keep=4 new=2050
```
Il prompt reale (7.647 token) veniva tagliato a 2.050: il modello perdeva parte delle istruzioni o della cronologia, e questo impediva a OpenCode di eseguire i tool. Con un context troppo corto, Gemma ha persino generato un ragionamento su un'app React inesistente, chiedendo poi "dimmi tu cosa vuoi fare" e ignorando la domanda originale.

Dalla documentazione mi sono accorto che non era un bug dei modelli ma il **context window di default di Ollama (4.096 token)**, spesso insufficiente già solo per il system prompt di OpenCode.
Innalzando `num_ctx` sopra i 16.384 il problema si risolveva sistematicamente.
In particolare, per il progetto di test, 32.768 si è poi rivelato il miglior compromesso.

### 2. Nove modelli a confronto per l'uso agentico

Chiedendo al modello di descrivere il progetto ho avuto i seguenti risultati:

| Modello | Taglia | Tool-calling | Note |
|---|---|---|---|
| **Qwen3.5:4b** | 4B | ✓ Affidabile | Il più equilibrato: veloce, fedele ai fatti, specie a temperatura bassa |
| **Qwen3.5:9b** | 9B | ✓ Affidabile | Più esplorativo, ma più incline a inferenze non verificate e più pesante |
| **Gemma4:e4b** | ~4.5B | ✓ Affidabile | Si è comportato benissimo al pari di Qwen3.5:9b |
| **Qwen2.5-coder:7b** | 7B | ✗ Non affidabile | Non genera tool-call in questo contesto, nonostante il nome |
| **Llama3.2:3b** | 3B | ✗ Non affidabile | Descrive i propri tool invece di usarli |
| **Phi4-mini:3.8b** | 3.8B | ✗ Non affidabile | Suggerisce comandi manuali invece di eseguirli |
| **Ministral-8b** | 8B | ⚠ Inefficiente | Usa i tool ma si disperde: 11 chiamate e ~4 minuti senza rispondere |
| **Ministral-3b** | 3B | ✗ Non affidabile | Applica pattern di ricerca JS/TS su un progetto Java; il peggiore del gruppo |
| **IBM Granite4.2:8b** | 8B | ⚠ Corretto ma lentissimo | Ottima esplorazione tecnica, ma delibera per minuti prima di ogni azione |

Come comparazione di correttezza ho comparato le risposte con quelle fornite da **Qwen3.8 Max** (cloud), che è un modello molto più grosso e capace. Tanto più il modello rispondeva in maniera simile, tanto più è capace, tenendo presente che le risorse a disposizione di Ollama sono fortemente limitate sulla mia macchina.

**La lezione più importante:** la capacità di orchestrare correttamente un ciclo di tool-calling non dipende né dalla dimensione del modello né dalla reputazione del produttore, ma dalla presenza di un addestramento specifico e mirato su questo comportamento. Qwen2.5-coder, pur avendo più parametri di Qwen3.5:4b, fallisce lo stesso compito che il modello più piccolo gestisce senza difficoltà.

### 3. Il vero collo di bottiglia: context window e VRAM

Provando vari modelli mi sono accorto che una context window troppo piccola causava molti problemi. Dopo averla aumentata ho notato che il consumo di VRAM saliva parecchio: in alcuni casi, come quello di Qwen3.5:9b, si arrivava all'offload parziale, che allungava la durata delle operazioni. Chiaramente occorre trovare il giusto bilanciamento.

I numeri di seguito vengono dai log di Ollama della sessione di test (`journalctl -u ollama`): ogni caricamento stampa quanta memoria servono pesi e KV-cache e quanti layer finiscono su GPU contro RAM. La lezione è che **la context window non è gratis**: ad alzarla cresce la KV-cache, non i pesi, e su 8 GB di VRAM è proprio la KV-cache il primo componente a sforare.

| Modello | Context | Pesi (VRAM) | KV-cache | GPU/CPU | Esito |
|---|---|---|---|---|---|
| Qwen3.5:4b | 16.384 | 2.5 GB | 0.5 GB | 34/34 GPU | 100% GPU / Stabile |
| Qwen3.5:4b | 32.768 | 2.5 GB | 1.1 GB | 34/34 GPU | 100% GPU / Stabile |
| Qwen3.5:9b | 16.384 | 4.7 GB | 1.1 GB | 32/34 GPU | 2 layer in RAM |
| Qwen3.5:9b | 32.768 | 4.7 GB | 4.6 GB | 33/37 GPU | Offload parziale, più lento |
| Qwen3.5:9b (num_gpu=0) | 32.768 | RAM (7.2 GB) | — | 100% CPU | 2m35s per compito semplice |
| IBM Granite4.2:8b | 32.768 | 4.9 GB | 5.1 GB | 26/41 GPU | 10.3 GB in RAM, 15m28s |

Osservazioni:
- Passando Qwen3.5:9b da 16k a 32k la KV-cache quadruplica (1.1 → 4.6 GB) mentre i pesi restano uguali: il modello esce quasi interamente dalla VRAM e ogni token paga il bus PCIe.
- Il caso estremo è Granite4.2:8b a 32k: la sola KV-cache (5.1 GB) supera i pesi (4.9 GB), e Ollama scarica in RAM di sistema **10.3 GB** — i suoi tempi lentissimi hanno un colpevole hardware, oltre al comportamento visto oltre.
- A titolo comparativo ho provato a disabilitare la GPU ed eseguire un'operazione solo sulla CPU: i tempi si sono dilatati enormemente.

Per verificarlo sul proprio setup: `ollama ps` mostra lo split GPU/CPU in tempo reale, oppure `journalctl -u ollama -f | grep offloaded` durante il caricamento.

### 4. Note sull'architettura di Qwen 3.5

Analizzando i log dopo aver utilizzato Qwen 3.5 ho notato la riga `forcing full prompt re-processing due to lack of cache data`.

Qwen3.5 usa un'attenzione lineare (non la classica attenzione quadratica con KV-cache piena): in questo setup non riesce a riutilizzare la cache del turno precedente e deve rielaborare l'intero prompt da zero a ogni turno. Su GPU l'impatto è contenuto; su CPU è devastante: in un test il prompt processing è proceduto a **~17-18 token/s**, contro i **~1.800 token/s** misurati su GPU per la stessa fase — un fattore di rallentamento di circa 100×.

### 5. Il caso IBM Granite4.2: competente ma indeciso

Avevo riposto grandi aspettative in Granite4.2 di IBM per l'analisi di codice Java.

Nella pratica: esplorazione tecnica eccellente, individua i path correttamente e risponde in maniera tutto sommato molto corretta, segnalando anche i test incompleti e i problemi nel codice.
La cosa che non mi è piaciuta molto è il fatto che si perde in molti ragionamenti e cicli: un blocco di ragionamento di 4 minuti per decidere come rispondere a "esegui questo test", con dialogo interno ripetuto ("wait, forse intendono... let me think again..."). Anche dopo aver già risposto correttamente a una domanda, tende a continuare a esplorare oltre il necessario.

Con prompt più specifici il comportamento migliora sensibilmente (risposta rapida e corretta), a conferma che parte del problema è la genericità della richiesta — ma la tendenza a "non fermarsi mai" resta un tratto di fondo, probabilmente ereditato proprio dal training agentico intensivo.

A questo si somma un colpevole hardware oggettivo, emerso dai log: a context 32k la sola KV-cache di Granite (5.1 GB) supera i pesi (4.9 GB), e Ollama scarica in RAM di sistema **10.3 GB** su 8 GB di VRAM (e 15.3 GB totali). Due terzi dei suoi layer girano a velocità RAM attraverso il bus PCIe: ogni token del suo thinking paga quel pedaggio, il che spiega perché un blocco di ragionamento impieghi 8m35s e una descrizione completa **15m28s**. Non è solo "indecisione": il modello è contemporaneamente sovraccaricato a livello hardware.

**Verdetto:** competenza tecnica reale (IBM ha decenni di esperienza nell'ecosistema Java enterprise), ma tempi (minuti anziché secondi) che lo rendono poco pratico per uso interattivo quotidiano. Utile solo per analisi una tantum dove il tempo non è un vincolo — a condizione di non spingerlo su context window troppo grandi. Sarebbe interessante provare tagli più grandi del modello avendo un hardware adeguato a disposizione.


## Il terzo test: OpenCode e la richiesta complessa

### Introduzione

Il terzo test che ho effettuato vuole mettere un po' alla prova questi modelli, con poco contesto, ponendo la domanda "a quali API risponde il servizio?" senza prima chiedere "descrivimi il progetto".
Apparentemente può sembrare una domanda più semplice della precedente, ma non è così.
Questa domanda obbliga il modello a navigare la struttura che nel test precedente era già stata messa in memoria dalla prima domanda:
in altre parole deve trovarsi la strada nel marasma del codice per rispondere alla domanda.

### 1. Il caso dei due YAML

Il progetto di test contiene **due specifiche OpenAPI distinte** (`server-api.yaml` per il lato server, `client-api.yaml` per il lato client), e le classi vengono generate da Maven durante la build.

Chiedendo "a quali API risponde il servizio" **a freddo** (senza contesto preliminare):

- **Qwen3.5:4b e Qwen3.5:9b** sono caduti nella stessa ambiguità: hanno letto e riportato lo YAML **sbagliato** (quello del servizio consumato, non quello esposto) — un errore di comprensione architetturale, non di tool-calling.
- **Gemma4:e4b**, delegando a un sub-agente `explore`, ha prodotto categorie concettualmente corrette ma con dettagli tecnici imprecisi (path e verbi HTTP inferiti, non letti dalle vere annotazioni).
- **Qwen3.8 Max (cloud, 2.4T parametri)**, sullo stesso prompt a freddo, ha riconosciuto esplicitamente la possibile ambiguità ("probably generated from an OpenAPI spec inside a dependency... let's check if there's a client vs server spec"), verificato entrambi i riferimenti nel `pom.xml`, e scelto correttamente la fonte giusta — risultato con path e verbi HTTP esatti, letti dalle vere annotazioni.

**Test di controllo decisivo:** ponendo la stessa domanda a **Qwen3.5:9b locale**, ma dopo avergli prima chiesto "cosa fa questo progetto" (costruendo così il quadro architetturale nella cronologia della conversazione), la seconda risposta è risultata **corretta** — stesso modello, stesso identico compito, esito opposto in base al solo contesto conversazionale accumulato.

**Conclusione:** il limite di scala tra un modello locale e uno enterprise non sparisce con nessuna configurazione — ma può essere **compensato nella pratica** fornendo tu stesso il contesto architetturale, in anticipo o tramite una sequenza di domande che costruisce gradualmente il quadro generale prima di scendere nel dettaglio specifico.


## Approfondimenti

Tre nozioni trasversali emerse dai test.

### Il fallimento del tool-calling: come funziona davvero

Il ciclo che permette a un agente di far "agire" un modello locale:

1. OpenCode costruisce il prompt includendo la lista dei tool disponibili (JSON Schema)
2. Il modello genera un output nel formato di tool-call appreso durante il training
3. Ollama (via llama.cpp) deve **riconoscere** quel formato e separarlo nel campo `tool_calls`
4. OpenCode legge quel campo ed esegue realmente l'azione, rispettando i permessi configurati
5. Il risultato torna al modello, che continua o risponde

Il punto 3 è il più fragile: alcune famiglie di modelli usano delimitatori diversi, e il parse di Ollama potrebbe fallire.

### Il ruolo della temperatura

Qwen3.5:9b, nelle analisi, si è lanciato nell'espansione non richiesta di un acronimo — espansione errata e inutile.

Facendo qualche ricerca ho capito che il problema dipende da uno dei parametri del modello: la temperatura.

| Temperatura | Esito | Tempo |
|---|---|---|
| 1.0 (default) | Espansioni di acronimi inventate (diverse ad ogni tentativo) | ~1m44s |
| 0.5 | Comportamento intermedio | — |
| 0.1 | Nessuna invenzione; prosa più asciutta | ~51s |

La temperatura influenza quanto il modello esplora ipotesi alternative prima di convergere.

Con temperatura bassa il modello sceglie più spesso il token statisticamente più probabile, genera meno testo di ragionamento e arriva più rapidamente alla conclusione.

Ragionando da umani, potremmo dire che una temperatura alta rende il modello più libero di esplorare, mentre una bassa lo rende più pragmatico.

Una cosa da notare: il valore empirico determinato per un modello/task non è applicabile a tutti i modelli, ma quello che si può trasportare è il meccanismo:
una temperatura alta fa comodo per esplorare soluzioni meno probabili, tipo un bug poco frequente o una soluzione complessa; una temperatura bassa fa comodo, per esempio, in un'analisi deterministica.

### GPU, CPU e architetture: perché contano così tanto

Una GPU può leggere/scrivere solo nella propria VRAM, una CPU solo nella RAM di sistema. Quando un layer gira su un device diverso da quello dei dati che gli servono, ogni trasferimento passa per il bus PCIe — molto più lento della banda interna della VRAM — e questo si ripete ad ogni token generato.

Le architetture a **memoria unificata** (Apple Silicon) eliminano quel collo di bottiglia condividendo lo stesso pool fisico tra CPU e GPU. Il divario di supporto software resta però enorme: Apple ha investito pesantemente nel backend Metal, ben supportato da Ollama/llama.cpp; le GPU integrate Intel restano poco supportate "di serie" (il percorso più maturo passa per llama.cpp con backend Vulkan, non SYCL).

Un M1 del 2020 con 8GB di RAM condivisa, testato informalmente, si è comportato sorprendentemente bene su questi carichi — controprova pratica di quanto la memoria condivisa (che evita lo split CPU/GPU e il transito PCIe) conti più della potenza di calcolo bruta per questo tipo di workload.

## Conclusioni pratiche e configurazione di riferimento

**Setup consolidato:**
- **Uso quotidiano / velocità:** Qwen3.5:4b, context 16k-32k, temperatura 0.1-0.3 per compiti di analisi fedele
- **Casi complessi che richiedono più profondità:** Qwen3.5:9b, verificando sempre i dettagli interpretativi (acronimi, nomi di dominio, path esatti)
- **Alternativa valida:** Gemma4:e4b, con context esplicitamente esteso (32k per progetti architetturalmente complessi)
- **Per analisi una tantum, tempo non vincolante:** IBM Granite4.2:8b
- **Da evitare per l'uso agentico con OpenCode (in questo setup):** Qwen2.5-coder, Llama3.2, Phi4-mini, Ministral 3b/8b

**Regole pratiche emerse:**

1. Il default di context di Ollama (4.096 token) è quasi sempre insufficiente per l'uso con un agente: 16k è un minimo, non un lusso; per progetti architetturalmente complessi (più moduli, doppie interfacce, sub-agenti) **32k è il vero punto di partenza sicuro**.
2. Verificare sempre con `ollama ps` se un modello gira 100% GPU o se c'è offload: anche un 13% su CPU si traduce in un rallentamento sproporzionato.
3. La context window non è gratis: ad alzarla cresce la KV-cache (non i pesi), e su 8 GB di VRAM è la prima a sforare. Un valore alto va quindi calibrato per modello, non alzato "a caso".
3. Un comportamento anomalo va prima verificato nei log (`journalctl -u ollama -f`) prima di concludere che sia un limite del modello.
4. Chiamare l'API di Ollama direttamente con `curl`, bypassando l'agente, è il modo più rapido per isolare se un problema è nel modello, in Ollama, o nell'integrazione con l'agente.
5. La stessa richiesta può avere esiti diversi in tentativi identici: prevedere un meccanismo di retry, non aspettarsi determinismo assoluto.
6. Nomi e reputazione ("coder", produttore noto, training agentico dichiarato) non garantiscono comportamento agentico affidabile né efficiente: va sempre verificato empiricamente.
7. La temperatura ottimale non è trasferibile da un modello all'altro come numero, ma il meccanismo con cui agisce (esplorazione vs fedeltà) sì — va riverificata caso per caso.
8. Su architetture non ovvie (es. wrapper a doppia interfaccia), fornire il contesto in anticipo o in due passaggi (prima il quadro generale, poi il dettaglio) compensa gran parte del divario di scala rispetto a un modello enterprise.
9. I modelli piccoli non colmano il divario di "conoscenza pregressa" di pattern architetturali rari rispetto a modelli di scala enterprise — nessuna configurazione lo elimina, si può solo compensarlo con il proprio contesto.
10. Quando un modello grande è lento, prima di tacciarlo di "indecisione" verifica nei log se è in offload: un modello che sfora la VRAM (KV-cache + pesi) paga il bus PCIe a ogni token, e i suoi tempi esplodono per cause hardware prima ancora che di comportamento.

Per la configurazione pratica (installazione, provider Ollama, context window, temperatura): vedi il [lab OpenCode con Ollama](ai-opencode-lab.md).

---

*Documento redatto a partire da un'esplorazione pratica condotta in chat, con log reali di Ollama e test ripetuti come fonte primaria dei dati riportati.*
