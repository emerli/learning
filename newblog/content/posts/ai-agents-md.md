---
title: "AGENTS.md — il file di contesto per gli agenti"
date: 2026-08-29
categories:
  - AI
slug: ai-agents-md
description: "Cos'è AGENTS.md, cosa scriverci, come si annida in un monorepo e come si rapporta a CLAUDE.md e agli altri file proprietari."
tags:
  - AI
---

# AGENTS.md — il file di contesto per gli agenti

Il README dice cosa fa il progetto; AGENTS.md dice all'agente come lavorarci. E sta diventando lo standard letto da 60k+ repo.

<!--more-->

## Cos'è e perché

`AGENTS.md` è una convenzione aperta e **tool-agnostica**: un file Markdown nella root del repository che dà a un agente di coding il contesto che un README non contiene — come si builda, come si testa, quali convenzioni seguire, quali trappole evitare.

Nasce per risolvere la frammentazione: prima ogni tool voleva il suo file proprietario (`CLAUDE.md`, `.cursorrules`, `.github/copilot-instructions.md`, `.windsurfrules`, …). `AGENTS.md` è il tentativo di averne **uno solo** che leggano tutti.

## Cosa ci va

- **Comandi**: setup, build, test, lint, run locale
- **Struttura del progetto**: dove sta cosa, moduli principali
- **Convenzioni di codice**: stile, naming, pattern preferiti e vietati
- **Testing**: come girano i test, cosa deve passare prima di un commit
- **Commit / PR**: formato dei messaggi, branch, checklist
- **Gotcha**: cose non ovvie che fanno perdere tempo (generazione codice in build, mock necessari, servizi esterni…)

## Cosa NON ci va

- Segreti, token, credenziali
- Copia-incolla integrale del README
- Dump enormi (l'agente ne legge solo le parti rilevanti, ma il rumore costa token e attenzione)
- Istruzioni contraddittorie o stantìe

## AGENTS.md annidati

`AGENTS.md` può stare a più livelli nello stesso repository: uno nella root e altri nelle sottocartelle di un monorepo. L'agente legge quello **più vicino** al file su cui sta lavorando, e quello ha la precedenza; root e locale si compongono. Nel repo di OpenAI, per dire, ci sono decine di `AGENTS.md`.

## Livello progetto e livello utente

`AGENTS.md` è nato come file **di progetto** (nella root del repo, versionato, condiviso col team). Diversi tool supportano *anche* un file di istruzioni **globale/utente** nella propria cartella di config — per esempio `~/.codex/AGENTS.md`, `~/.config/opencode/AGENTS.md`, `~/.claude/CLAUDE.md`. Sono due livelli distinti: il globale porta le tue preferenze personali, quello di progetto il contesto del repo.

## Chi lo legge

`AGENTS.md` è lo standard usato da 60k+ repository, proposto da OpenAI nell'agosto 2025. È letto nativamente (file nella root del repo, senza configurazione) da:

- **OpenAI Codex**, **Cursor**, **Windsurf**, **Zed**, **VS Code**, **JetBrains Junie**
- **Aider**, **goose**, **opencode**, **Warp**
- **Google Jules**, **Gemini CLI**
- **Devin** (Cognition), **Amp**, **Factory**, **RooCode**, **Augment Code**
- il **coding agent** di GitHub Copilot (l'agente asincrono; i suggerimenti inline usano ancora `.github/copilot-instructions.md`)

Lista completa (~24 tool) su [agents.md](https://agents.md).

**Eccezione nota — Claude Code**: carica automaticamente solo `CLAUDE.md`, non `AGENTS.md`. Si collega con un import o un symlink (vedi sotto).


## Rapporto con CLAUDE.md e gli altri

La doc di Claude Code è netta: *"Claude Code reads `CLAUDE.md`, not `AGENTS.md`"* — nessun fallback automatico. Per far leggere lo stesso contesto anche a Claude Code ci sono due strade:

- **import** — una riga `@AGENTS.md` dentro `CLAUDE.md`; l'import viene espanso e caricato all'avvio, e sotto puoi aggiungere istruzioni specifiche per Claude
- **symlink** — `ln -s AGENTS.md CLAUDE.md`, se non ti serve aggiungere nulla di specifico

In più, `/init` e `/import` di Claude Code sanno assorbire il contenuto di `AGENTS.md` (e di `.cursorrules`, `.github/copilot-instructions.md`, …) nel `CLAUDE.md` generato, come copia una tantum.

Il pattern generale quando si usano più tool: un unico file di verità (`AGENTS.md`) e gli altri che lo importano o ci puntano con un symlink, senza duplicare.

## Esempio: un file globale

Questo è il mio file di istruzioni **globale** (in `~/.claude/CLAUDE.md`, ma la forma sarebbe identica in un `~/.codex/AGENTS.md`). Da notare quanto spazio prendono le azioni che l'agente **non** deve mai fare: sono quelle che risparmiano più grattacapi.

```text
# Preferenze Globali

## Environment
- Sono un esperto di Linux

## Code Review
- Quando analizzo codice esistente, segnala solo problemi reali, non style nits
- Priorità: correttezza > leggibilità > performance

## Stile di Lavoro
- Prima di fare modifiche significative, proponi il piano e aspetta la mia approvazione
- Per cambiamenti minori (typo, fix ovvi), agisci direttamente
- Risposte concise: vai al punto, senza preamboli
- Se hai dei dubbi chiedi a me
- Se non è chiaro al 100% cosa fare, fare una domanda diretta e aspettare risposta
- "copia" = lascia originale; "sposta" = rimuovi originale
- Utilizziamo un approccio di tipo Agile
- Ove possibile utilizziamo il pattern Strangler Fig

## Coding
- Soluzioni semplici: non aggiungere astrazioni, helper o utility per casi singoli
- Non rifactorizzare codice che non è stato richiesto
- Non aggiungere commenti al codice che non ho chiesto di modificare
- Non aggiungere gestione errori per scenari impossibili

## Stack
- Backend: Java / C# / Go
- Infra: Docker, Kubernetes, OpenShift, Ansible
- Cloud: AWS

## Git
- Non fare commit automaticamente a meno che non lo chieda esplicitamente
- Non fare push senza conferma esplicita

## Lingua
- Rispondimi sempre in italiano

## Testing
- Approccio TDD: scrivi i test prima dell'implementazione
- Non saltare i test anche per fix piccoli
```

La cosa interessante è che, con le direttive giuste, l'agente prende le stesse decisioni che prenderesti tu: le scelte ricorrenti (proponi il piano, non refactorare a caso, niente commit automatici…) diventano il default, senza doverle ripetere a ogni sessione.

## Anti-pattern

- **Troppo lungo.** Target sotto le ~200 righe: oltre, l'aderenza cala e paghi contesto a ogni sessione. Se cresce, spezza in regole scope-per-path o in skill.
- **Stantìo.** Comandi o percorsi che non esistono più. Un `AGENTS.md` sbagliato è peggio di nessuno: manda l'agente fuori strada *con sicurezza*. Rivedilo quando cambi build o struttura.
- **Istruzioni contraddittorie.** Due regole in conflitto (magari una nella root e una in un file annidato): l'agente ne sceglie una a caso.
- **Segreti dentro.** Token, connection string, chiavi: il file è versionato e finisce nel contesto di ogni agente.
- **Duplicare README / doc.** Copia-incolla di cose che l'agente legge già dal codice. Metti solo ciò che *non* è derivabile: gotcha, convenzioni che divergono dai default, comandi non ovvi.
- **Regole vaghe.** "Scrivi codice pulito", "testa bene" → "indentazione a 2 spazi", "esegui `make test` prima di committare".
- **Preferenze personali in un file di team.** "Rispondimi in italiano", "sono esperto di Linux" vanno nel file utente/globale, non nell'`AGENTS.md` di progetto condiviso.
- **Procedure lunghe multi-step.** Se un workflow serve solo a volte, va in una skill o in un comando, non in context a ogni sessione.

## Riferimenti

- [agents.md](https://agents.md) — sito ufficiale della convenzione: formato, sezioni consigliate, elenco dei tool che la supportano
- [Claude Code — Memory](https://code.claude.com/docs/en/memory) — come Claude Code gestisce `CLAUDE.md` e si collega ad `AGENTS.md`
