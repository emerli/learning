---
title: "Clean Code - Sintesi"
date: 2026-04-24
description: "Sintesi del libro Clean Code di Robert C. Martin (Uncle Bob)"
categories:
  - Java
draft: true
---

# Clean Code - Sintesi

> Autore: Robert C. Martin (Uncle Bob)

---

## 1. Cos'è il Codice Pulito

Il codice pulito si legge come una prosa ben scritta. Le sue caratteristiche principali sono:
- **Leggibilità**: chiaro, semplice, senza ambiguità.
- **Focalizzazione**: fa una cosa sola e la fa bene.
- **Assenza di duplicazione** (principio DRY).
- **Testabilità**: deve avere unit test.
- **Minimalismo**: non contiene nulla di superfluo.

---

## 2. I Nomi (Naming)

- **Dai nomi significativi e pronunciabili**: `elapsedTimeInDays` è meglio di `d` o `time`.
- **Evita disinformazione**: non chiamare `accountList` una variabile che non è una lista.
- **Usa nomi ricercabili**: evita variabili a singolo carattere (tranne contatori locali).
- **Classi e oggetti**: nomi sostantivi (es. `Customer`, `Account`).
- **Metodi**: nomi verbi o frasi verbali (es. `postPayment`, `deletePage`).
- **Usa una parola per concetto**: scegli `get`, `fetch` o `retrieve` e usala sempre, non mischiarle.

---

## 3. Le Funzioni

- **Piccole**: idealmente non più di 20 righe, meglio se 4-5.
- **Fanno una sola cosa** (Single Responsibility): se devi dire "e" o "poi" per descriverla, fa più cose.
- **Un livello di astrazione per funzione**: non mischiare logica di alto livello con dettagli di basso livello.
- **Numero di argomenti**: 0 è ideale, 1 ok, 2 tollerabile, 3 da evitare, più di 3 richiede giustificazione.
- **Evita side effects nascosti**: una funzione deve fare ciò che dice nel nome, senza sorprese.
- **Command Query Separation**: una funzione o fa qualcosa (command) o risponde a qualcosa (query), non entrambe.
- **Preferisci eccezioni a codici di errore**: così il codice principale resta pulito.

---

## 4. I Commenti

- **Il codice deve essere auto-esplicativo**: i commenti sono spesso una scusa per codice cattivo.
- **Non commentare codice obsoleto**: cancellalo (c'è il version control).
- **Commenti accettabili**: intento legale, spiegazioni di algoritmi complessi, TODO temporanei (da risolvere), amplificazione di cose che sembrano ovvi ma non lo sono.
- **Evita**: commenti ovvii, commenti fuorvianti, commenti di chiusura `// end if`.

---

## 5. Formattazione

- **La formattazione è importante**: comunica struttura e organizzazione.
- **Densità verticale**: concetti correlati devono essere vicini.
- **Distanza verticale**: dichiarazioni di variabili vicine al loro uso; funzioni dipendenti vicine.
- **Lunghezza file**: idealmente non più di 200-500 righe.
- **Indentazione**: essenziale per la leggibilità gerarchica.

---

## 6. Oggetti e Strutture Dati

- **Legge di Demeter**: un modulo non dovrebbe conoscere i dettagli interni degli oggetti che manipola. Parla solo con "amici stretti" (`obj.getX().getY().doSomething()` è un anti-pattern).
- **Data/Object Anti-Symmetry**: le strutture dati esponogno dati e non hanno comportamenti; gli oggetti nascondo dati ed esponogno comportamenti. Non mischiare i due approcci senza motivo.
- **Evita "Feature Envy"**: una funzione che usa più dati di un altro oggetto che del proprio.

---

## 7. Gestione degli Errori

- **Usa eccezioni, non codici di ritorno**: separa la logica dal flusso di errore.
- **Scrivere il `try-catch-finally` per primo**: aiuta a definire i confini e a gestire le risorse.
- **Non restituire `null`**: genera `NullPointerException` e aumenta il codice di controllo. Restituisci oggetti vuoti (Null Object pattern) o lancia eccezioni.
- **Non passare `null`**: stesso motivo.

---

## 8. Unit Test e TDD

- **Le tre leggi del TDD**:
  1. Non scrivere codice di produzione finché non hai un test che fallisce.
  2. Scrivi solo il test sufficiente per fallire.
  3. Scrivi solo il codice di produzione sufficiente per far passare il test.
- **FIRST**: i test devono essere **F**ast, **I**ndipendent, **R**epeatable, **S**elf-validating, **T**imely (scritti prima o subito dopo).
- **Un assert per test**: idealmente un concetto per test.
- **I test sono importanti quanto il codice di produzione**: devono essere puliti, evolvono con il codice.

---

## 9. Classi

- **Ordinamento**: variabili statiche, variabili d'istanza, pubbliche/private, metodi.
- **Piccole**: la prima regola è che le classi dovrebbero essere piccole; la seconda è che dovrebbero essere *ancora più piccole*.
- **Responsabilità singola (SRP)**: una classe dovrebbe avere un solo motivo per cambiare. Meglio molte classi piccole che una grande.
- **Coesione**: i metodi dovrebbero manipolare le variabili d'istanza della classe. Se pochi metodi usano poche variabili, forse servono sottoclassi.
- **Isolamento**: dipendi da astrazioni, non da concretezze (DIP - Dependency Inversion Principle).

---

## 10. Principi SOLID

- **S**ingle Responsibility: una responsabilità per classe/modulo.
- **O**pen/Closed: aperto all'estensione, chiuso alla modifica.
- **L**iskov Substitution: classi figlie sostituibili alle classi padri.
- **I**nterface Segregation: interfacce specifiche, non generali.
- **D**ependency Inversion: dipendi da astrazioni.

---

## 11. Code Smells ed Euristiche

Il libro elenca decine di "odore di codice" tra cui:
- **Rigidity**: il codice è difficile da cambiare.
- **Fragility**: un cambiamento rompe parti non correlate.
- **Immobility**: difficile riutilizzare codice in altri progetti.
- **Viscosity**: è più facile fare il "hack" che la cosa giusta.
- **Opacità**: il codice è difficile da capire.
- **Needless Complexity**: complessità senza beneficio.
- **Needless Repetition**: duplicazione di codice.

---

## 12. Conclusioni

Scrivere codice pulito è una disciplina continua che richiede pratica e attenzione. Il codice pulice non è scritto seguendo un set di regole rigide, ma sviluppando un senso estetico e professionale per la qualità del software. Come dice Martin:

> "Il codice pulito è quello che è stato fatto con cura da qualcuno che si è preoccupato."