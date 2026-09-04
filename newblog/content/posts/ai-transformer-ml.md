---
date: 2026-08-05
categories:
  - AI
draft: true
tags:
  - AI
title: "Note su AI moderne, Transformer e progetto ML.NET"

---

# Note su AI moderne, Transformer e progetto ML.NET

Riepilogo di una conversazione esplorativa su come funzionano le AI moderne e su un possibile progetto pratico con ML.NET.

---

## 1. Architettura Transformer

Le AI moderne (LLM inclusi) restano reti neurali profonde, ma con l'architettura **Transformer** (2017, "Attention Is All You Need") al posto delle vecchie RNN/LSTM.

### Self-attention
Ogni token calcola quanto è rilevante ogni altro token per interpretare il proprio significato nel contesto.

Meccanismo:
- **Query (Q)**: cosa sto cercando
- **Key (K)**: cosa posso offrire
- **Value (V)**: l'informazione che porto

Si confronta la Query di un token con la Key di tutti gli altri (prodotto scalare) → softmax → pesi che sommano a 1 → media pesata dei Value = nuova rappresentazione del token, arricchita dal contesto.

**Importante**: i pesi di attention sono **calcolati al volo** in base all'input specifico (dinamici), diversi dai **pesi della rete** (le matrici W_Q, W_K, W_V, W_O, i pesi del feed-forward) che sono **appresi durante il training e fissi durante l'inferenza**.

### Multi-head attention
Diverse "teste" di attention in parallelo (8, 16+), ognuna libera di specializzarsi su pattern diversi (sintattici, semantici, posizionali). I risultati vengono concatenati.

### Struttura di un layer
Attention → feed-forward (stessa rete applicata indipendentemente a ogni token) → residual connections + layer normalization. Il blocco si ripete per decine di layer; i layer più profondi catturano astrazioni via via più alte.

---

## 2. Comprensione vs Generazione

- **Comprensione (encoding)**: il modello può vedere tutto il contesto insieme, in un solo forward pass.
- **Generazione (decoding)**: richiede la **maschera causale** — ogni token può guardare solo sé stesso e i token precedenti, mai quelli futuri (altrimenti "barerebbe" prevedendo qualcosa che già vede).

### Ciclo di generazione autoregressiva
1. Prompt in input → tokenizzazione → embedding
2. Passaggio nei layer transformer (con maschera causale)
3. Distribuzione di probabilità sul vocabolario per il **prossimo** token
4. Sampling (temperature, top-p) → si sceglie un token
5. Il token scelto viene riattaccato alla sequenza
6. Si ripete da capo per il token successivo, finché non c'è un token di stop

**Costo**: leggere un prompt = 1 forward pass su tutto il testo. Generare N token = N forward pass, uno per token → la generazione è molto più costosa della lettura.

**KV cache**: ottimizzazione che salva i vettori Key/Value già calcolati per i token passati, evitando di ricalcolarli ad ogni nuovo token generato.

---

## 3. Progetto ML.NET — generazione dataset sintetici

Idea di partenza: usare un LLM per generare dataset di training per compiti semplici (classificazione, sentiment) da usare con ML.NET.

### Perché ML.NET (e non una rete neurale da zero)
Per compiti "semplici" (categorizzazione, sentiment), una rete neurale addestrata da zero è spesso overkill: servono molti dati, tuning di iperparametri, rischio di overfitting. ML.NET offre algoritmi ML classici (regressione logistica, alberi decisionali, SVM, ecc.) che spesso funzionano meglio con dataset piccoli, si addestrano in secondi/minuti senza GPU, e hanno text featurization pronta all'uso (bag-of-words, TF-IDF).

Supporta anche AutoML (prova automaticamente diverse configurazioni) e importazione di modelli pre-addestrati via ONNX/TensorFlow.

### Come migliorare la generazione sintetica (il problema principale riscontrato)
Il rischio è l'**omogeneità**: prompt generici producono frasi con pattern ripetitivi (stessa struttura, stesso vocabolario), che portano il modello a imparare scorciatoie inutili.

Mitigazioni:
1. **Assi di variazione espliciti**: registro (formale/informale/slang), lunghezza, dominio, fenomeni linguistici specifici (sarcasmo, negazione, sentiment misto)
2. **Esempi reali come seed**: anche 10-20 esempi veri del dominio migliorano molto il risultato
3. **Generazione a piccoli lotti con istruzioni diverse** invece di un unico batch enorme
4. **Bilanciamento esplicito delle classi**, incluso qualche caso ambiguo/borderline
5. **Mescolare sempre dati sintetici con dati reali**, se disponibili

Esempio di prompt efficace:
> "Genera 30 recensioni di ristoranti in italiano, registro informale, 10 positive/10 negative/10 neutre, lunghezza 1-2 frasi, includi almeno 5 casi con sarcasmo o ironia, evita di ripetere le stesse strutture sintattiche"

### Altre strategie per il problema del dataset
- **Dataset pubblici pronti**: IMDB reviews, Amazon reviews, Sentipolc/SentITA (italiano), Hugging Face Datasets
- **Weak supervision**: regole euristiche per etichettare automaticamente dati grezzi
- **Transfer learning / fine-tuning**: partire da un modello pre-addestrato (es. UmBERTo, AlBERTo per l'italiano) invece che allenare da zero — riduce drasticamente il fabbisogno di dati (centinaia invece di decine di migliaia di esempi). Probabile causa delle difficoltà avute in passato con TensorFlow: training da pesi casuali richiede troppi dati.
- **Data augmentation**: sinonimi random, back-translation, piccole perturbazioni

### Nota su sistemi real-time / safety-critical
Un LLM esterno non è mai adatto per decisioni time-critical (es. guida autonoma): latenza troppo alta (500ms-2s di round trip), dipendenza da rete, non-determinismo incompatibile con certificazioni safety (es. ISO 26262). Il mio ruolo (Claude) in questi workflow è solo nella fase di **creazione offline del dataset**, mai nell'inferenza in produzione — il modello addestrato gira sempre localmente.

---

## 4. Dimensionamento delle reti neurali

Non esiste una formula chiusa per determinare numero di layer/neuroni — è un processo **euristico e sperimentale**, non teorico-deterministico. Anche ricercatori esperti procedono per tentativi.

### Cosa è fissato vs cosa si sceglie
- Input/output: fissati dal problema
- Profondità (numero layer) e larghezza (neuroni per layer): variabili "artistiche", senza formula esatta

### Euristiche pratiche esistenti
- Il vincolo più concreto è il **rapporto parametri/dati disponibili**: troppi parametri rispetto ai dati → overfitting (probabile causa dei problemi avuti in passato con TensorFlow)
- Problemi con relazioni gerarchiche (visione, linguaggio) beneficiano di più layer; problemi tabellari semplici spesso no (a volte i modelli classici battono le reti profonde)
- Si parte quasi sempre da architetture note/pubblicate (transfer learning) invece di progettare da zero
- Si usa una rete "abbastanza grande da poter overfittare" come test di sanity check iniziale
- Si applica regolarizzazione (dropout, weight decay, early stopping) invece di cercare la dimensione "perfetta" a mano
- Grid search / random search / Bayesian optimization / AutoML per esplorare sistematicamente le configurazioni

### Il punto filosofico
Il campo si basa su principi teorici parziali (bias-variance tradeoff, capacità del modello) ma nessuno di questi dà una formula pratica generale. È un ciclo iterativo "provo → misuro (errore di validazione) → aggiusto", diverso dal sizing più deterministico tipico di un contesto IT/DevOps (CPU, RAM, storage).

---

## 5. Prossimi passi (aperti)

- Individuare un task concreto per il progetto ML.NET (categorizzazione email, sentiment su un dominio di interesse, rilevamento urgenza in ticket, classificazione log — quest'ultimo in linea con il background DevOps)
- Una volta scelto il task, costruire un primo dataset sintetico applicando le strategie di variazione discusse
- Strutturare uno scheletro di progetto ML.NET (pipeline di featurization + classificatore)
