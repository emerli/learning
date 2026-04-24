---
title: "CLAUDE.md — Preferenze Globali"
date: 2026-04-24
description: "Il mio file CLAUDE.md con le preferenze e regole per l'interazione con gli agenti AI"
---

# CLAUDE.md — Preferenze Globali

Ecco il contenuto del mio file `CLAUDE.md` così come è configurato:

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
- Non refactorare codice che non è stato richiesto
- Non aggiungere commenti al codice che non ho chiesto di modificare
- Non aggiungere gestione errori per scenari impossibili

## Stack
- Backend: [Java / C# / Go ]
- Infra: [Docker, Kubernetes, Openshift / Ansible]
- Cloud: [AWS ]

## Git
- Non fare commit automaticamente a meno che non lo chieda esplicitamente
- Non fare push senza conferma esplicita

## Lingua
- Rispondimi sempre in italiano

## Testing
- Approccio TDD: scrivi i test prima dell'implementazione
- Non saltare i test anche per fix piccoli
```