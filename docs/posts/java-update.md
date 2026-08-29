---
title: "Java 8 → 25: Cambiamenti rilevanti per lo sviluppo"
date: 2026-04-24
description: "Panoramica delle novità più importanti da Java 9 a Java 25, con focus su Quarkus"
categories:
  - Java
draft: true
---

# Java 8 → 25: Cambiamenti rilevanti per lo sviluppo

## Java 9-11 (LTS 11)

- **Jigsaw/JPMS** (9) — Sistema a moduli
- **`var`** per variabili locali (10)
- **`HttpClient`** API moderna, async/reactive (11)
- Multicatch migliorato, `String.isBlank()`, `Files.readString()`

## Java 12-17 (LTS 17)

- **Switch expressions** (14) — `yield`, arrow syntax
- **Text blocks** (15) — Stringhe multilinea `"""`
- **Pattern matching `instanceof`** (16) — `if (obj instanceof String s)`
- **Records** (16) — `record Point(int x, int y) {}`
- **Sealed classes** (17) — `sealed interface Shape permits Circle, Rect`

## Java 18-21 (LTS 21)

- **Virtual Threads** (21) — Thread ultraleggeri, game-changer per I/O concorrente
- **Pattern matching per switch** (21)
- **Record patterns** (21) — `if (obj instanceof Point(int x, int y))`
- **SequencedCollections** (21) — `getFirst()`, `getLast()` sulle collection
- **Structured Concurrency** (21, incubator)

## Java 22-24

- **Unnamed variables** (22) — `var _ = list.remove(0)`
- **Statements before `super()`** (22)
- **String templates** (23, preview) — `"Hello \{name}"` *(non ancora final)*
- **Primitive types in patterns** (23, preview)
- **Stream Gatherers** (24) — APIExt per operazioni intermedie custom sugli Stream
- **Class-file API** (24) — sostituisce ASM per manipolazione bytecode
- **Structured Concurrency** (24, terza preview)

## Più importanti per Quarkus

1. **Virtual Threads** — Quarkus li supporta nativamente, abilita `quarkus.virtual-threads.enabled=true`
2. **Records** — Perfetti come DTO
3. **Pattern matching** — Riduce il boilerplate
4. **Sealed classes** — Ideali per modelli di dominio ristretti

## Java 25 (prevista settembre 2026)

Probabilmente final le String templates e ulteriori miglioramenti ai Virtual Threads.