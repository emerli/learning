---
title: "AGENTS.md — il file di contesto per gli agenti"
date: 2026-08-29
categories:
  - AI
draft: true
slug: ai-agents-md
description: "Cos'è AGENTS.md, cosa scriverci, come si annida in un monorepo e come si rapporta a CLAUDE.md e agli altri file proprietari."
---

# AGENTS.md — il file di contesto per gli agenti

> Bozza — scheletro da riempire con esempi e note dall'uso reale.

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

In un monorepo puoi mettere un `AGENTS.md` nella root e altri nelle sottocartelle. Vale quello **più vicino** al file su cui l'agente sta lavorando; root e locale si compongono, il locale ha priorità sulle regole in conflitto.

## Chi lo legge

<!-- TODO: elenco aggiornato dei tool che supportano AGENTS.md nativamente
     (Codex, Cursor, Aider, Zed, Jules, …) e stato del supporto in Claude Code -->

## Rapporto con CLAUDE.md e gli altri

<!-- TODO: CLAUDE.md = preferenze globali/utente; AGENTS.md = contesto di progetto.
     Strategie: symlink, file separati, un solo file. Cosa fa OpenCode. -->

## Esempio commentato

<!-- TODO: un AGENTS.md reale, sezione per sezione, con commento sul perché di ogni riga -->

## Anti-pattern

<!-- TODO: troppo lungo, stantìo, segreti, istruzioni contraddittorie, duplicare il README -->

## Riferimenti

<!-- TODO: sito agents.md, eventuale spec/guida ufficiale -->
