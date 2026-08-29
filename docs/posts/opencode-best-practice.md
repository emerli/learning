---
title: "OpenCode — Best Practice per Agenti e Workflow"
date: 2026-04-24
description: "Guida pratica per usare al meglio gli agenti primari, i subagenti e la pianificazione in OpenCode"
categories:
  - AI
draft: true
---

# OpenCode — Best Practice per Agenti e Workflow

## Panoramica

OpenCode offre un'architettura multi-agente composta da agenti **primari** e **subagenti** specializzati. Capire quando e come usarli è fondamentale per ottenere risultati ottimali.

---

## Agenti Primari

Gli agenti primari sono quelli con cui interagisci direttamente. Puoi switchare tra loro premendo `Tab`.

| Agente | Scopo | Permessi |
|--------|-------|----------|
| **Build** | Sviluppo attivo, scrittura codice | Pieno accesso a file e Bash |
| **Plan** | Analisi e pianificazione | `ask` su scrittura/modifiche per evitare cambiamenti accidentali |

**Consiglio**: usa sempre `Plan` prima di iniziare task complessi. Un errore architetturale nel piano, moltiplicato per N componenti, è costoso da correggere.

---

## Subagenti

I subagenti vengono invocati automaticamente dall'agente primario o manualmente con `@nome`.

### @explore — Esplorazione in sola lettura

Ideale per cercare file, pattern o informazioni nel codebase **senza modificare nulla**.

**Quando usarlo:**
- Trovare dove è definita una funzione
- Analizzare la struttura del progetto
- Cercare riferimenti a un servizio o API

**Esempi:**
```
@explore dove viene validato l'input dell'utente nel modulo di login?
@explore trova tutte le funzioni che chiamano l'API di pagamento
```

### @general — Task multi-step complessi

Subagente general-purpose con pieno accesso agli strumenti (tranne `todo`). Esegue task multi-step in autonomia.

**Quando usarlo:**
- Analisi comparativa di più funzioni
- Refactoring su più file
- Task paralleli delegati dall'agente primario

**Esempi:**
```
@general analizza queste tre funzioni e proponi un'ottimizzazione comune
```

---

## Pianificazione e Workflow

### Pattern: Plan → Build → Verify

```
1. @plan       → analizza e genera un piano dettagliato
2. @build      → esegue fase per fase
3. @general    → verifica il risultato (test, build, lint)
```

Il piano diventa il **contesto condiviso** per tutti gli agenti coinvolti. L'agente primario o un orchestratore come **Sisyphus** (in Oh My Opencode) gestisce le dipendenze tra i passi, eseguendo in parallelo o sequenza in base alle necessità.

### Gestione delle dipendenze

| Approccio | Uso |
|-----------|-----|
| **Plan** | Genera roadmap con fasi logiche e dipendenze |
| **Sisyphus** | Orchestratore avanzato che delega a subagenti specializzati (@oracle per architettura, @general per esecuzione) |

---

## Configurazione Avanzata

### Configurare agenti

Puoi configurare agenti tramite:
- `~/.config/opencode/opencode.json` (globale)
- `.opencode/agents/*.md` (per progetto)
- `~/.config/opencode/agents/*.md` (globale)

**Parametri utili:**
- `temperature`: bassa (0.0-0.2) per analisi deterministiche, alta (0.6-1.0) per brainstorming creativo
- `default_agent`: seleziona automaticamente l'agente primario preferito
- `max_iterations`: limita i passaggi massimi per task complessi

### File AGENTS.md

In progetti complessi, crea un file `AGENTS.md` nella root del repository per fornire contesto gerarchico. Gli agenti leggeranno solo le parti rilevanti, migliorando la precisione.

---

## Strumenti e Permessi

### Strumenti principali

| Strumento | Funzione |
|-----------|----------|
| `write` | Crea o sovrascrive file |
| `edit` | Modifica parti specifiche di un file |
| `bash` | Esegue comandi nel terminale |

### Livelli di permesso

- **`true`** — l'agente usa lo strumento senza chiedere
- **`false`** — l'agente non può usarlo
- **`ask`** — l'agente chiede conferma prima di usarlo (default per `Plan`)

**Regole pratiche:**
- Inizia con permessi conservativi (`ask` per `edit` e `bash`)
- Rimuovi i vincoli quando hai fiducia nel template e nel flusso
- Non invocare mai gli strumenti direttamente dal prompt: descrivi il compito in linguaggio naturale e lascia che l'agente scelga lo strumento giusto

---

## Personalizzazione

### Agenti custom via Markdown

Puoi definire agenti esclusivamente come file `.md` — nessun codice richiesto.

**Struttura del file:**
```markdown
---
description: Analizza codice legacy senza modificarlo. 
             Usami PRIMA di qualsiasi migrazione.
mode: subagent
model: anthropic/claude-sonnet
permission:
  read: allow
  write: deny
  edit: deny
  bash: deny
---

Sei uno specialista nell'analisi di codice .NET 4.7 legacy.
Il tuo compito è solo analizzare e descrivere, mai modificare.
```

Salva in `.opencode/agents/review.md` o `~/.config/opencode/agents/review.md`.

### Strumenti custom MCP

È possibile aggiungere strumenti personalizzati in JavaScript, Python, ecc. e integrarli tramite il protocollo MCP (Model Context Protocol), poi abilitarli per agenti specifici nella configurazione.

---

## Checklist Rapida

| Step | Azione |
|------|--------|
| 1 | Inizia sempre con `Plan` per task complessi |
| 2 | Usa `@explore` per capire il codebase prima di modificare |
| 3 | Delega task paralleli a `@general` |
| 4 | Verifica il risultato con test/build prima di proseguire |
| 5 | Crea `AGENTS.md` per contesto progettuale nei repo grandi |
| 6 | Usa temperatura bassa per refactoring, alta per design |
| 7 | Inizia con permessi conservativi, allenta col tempo |

---

## Riferimenti

- [Oh My Opencode](https://github.com/ibm/opencode) — Estensione con orchestratore Sisyphus per multi-agente parallelo
- [OpenCode Documentation](https://opencode.ai/docs) — Documentazione ufficiale