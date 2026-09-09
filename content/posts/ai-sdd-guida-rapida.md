---
title: "Spec Driven Development (SDD) — Guida Rapida"
date: 2026-04-29
description: "Come usare la spec come fonte della verità: contratto vs logica, quando aggiornarla, scaffolding a build time, e dove entra l'AI."
categories:
  - AI
draft: false
slug: ai-sdd-guida-rapida
tags:
  - AI
  - SDD
  - Spec-Driven Development
---

# Spec Driven Development (SDD) — Guida Rapida

Prima la spec, poi il codice — a patto che la spec non sia un documento morto. La differenza sta nel renderla eseguibile: stub generati a build time e test che la fanno rispettare.

<!--more-->

## 1. Cos'è lo SDD

Lo **Spec Driven Development** è un approccio in cui la **specifica** viene scritta **prima** del codice e funge da **fonte della verità** per tutto il ciclo di sviluppo.

Ci sono due scuole, a seconda di cosa sia la "spec":

- **Contratto formale** — un file macchina, tipicamente **OpenAPI** (`api.yaml`), da cui si genera lo scaffolding. È il taglio di questa guida.
- **Spec in linguaggio naturale** — requisiti e scenari scritti in Markdown, che un **agente AI** legge ed esegue. Vedi gli esempi pratici con [Spec Kit](/learning/posts/ai-spec-kit-book-api/) e [OpenSpec](/learning/posts/ai-openspec-book-api/).

I principi sotto (contratto vs logica, quando aggiornare la spec) valgono per entrambe.

---

## 2. La regola d'oro

> **La spec (`api.yaml`) dice COME si comunica con il servizio.**
> **La logica di business dice COSA fa il servizio dentro.**
> **Markdown/IA implementa il COSA solo dopo avere lo scheletro del COME.**

---

## 3. Flusso di lavoro corretto

| Fase | Chi/Cosa | Output | Descrizione |
|------|----------|--------|-------------|
| **1. Contratto** | Umano o AI | `api.yaml` | Definisci endpoint, schemi, parametri, errori |
| **2. Scaffolding** | OpenAPI Generator / AI | Stub vuoti | Genera controller, DTO, modelli vuoti |
| **3. Requisiti funzionali** | Umano | `docs/logica-*.md` o prompt | Descrivi la logica di business da implementare |
| **4. Implementazione** | AI o umano | Codice completo | Scrivi la logica dentro gli stub |
| **5. Test** | Umano o AI | Test JUnit | Verifica che la logica rispetti i requisiti |

---

## 4. Cosa mettere nella spec (`api.yaml`)

- Endpoint e metodi HTTP
- Schemi JSON di richiesta/risposta
- Parametri query/path
- Codici errore
- Vincoli di validazione (`minLength`, `pattern`, `minimum`, ecc.)

**Esempio:**
```yaml
schemas:
  Libro:
    type: object
    properties:
      titolo:
        type: string
        minLength: 1
        maxLength: 200
      prezzo:
        type: number
        minimum: 0
```

---

## 5. Cosa NON mettere nella spec

- Regole di calcolo interne
- Flussi condizionali complessi
- Decisioni architetturali
- Logica che non cambia il contratto esterno

**Esempio di logica interna (NON va in `api.yaml`):**
> "Se il prezzo è superiore a 50€, il libro viene marcato come raro"

Questa è una regola di dominio. La spec si limita a dire che esiste il campo `raro: boolean` nella risposta, non **come** viene calcolato.

---

## 6. Quando aggiornare la spec PRIMA del codice

| Cambiamento | Aggiorni `api.yaml` prima? |
|-------------|---------------------------|
| Nuovo endpoint | ✅ Sì |
| Nuovo campo nel JSON di risposta | ✅ Sì |
| Cambio URL o parametro | ✅ Sì |
| Nuovo errore HTTP restituito | ✅ Sì |
| Regola interna di calcolo/stato | ❌ No |
| Ottimizzazione query DB | ❌ No |
| Nuovo controllo condizionale | ❌ No |

---

## 7. Come usare l'AI nel flusso SDD

### A. AI scrive la spec
```
Prompt: "Scrivi una spec OpenAPI per un API di libreria con libri e autori"
→ Output: api.yaml
```

### B. AI genera lo scaffolding
```bash
openapi-generator-cli generate -i api.yaml -g spring -o ./server
→ Output: Controller vuoti, DTO, modelli
```

### C. AI implementa la logica
**Input per l'AI:**
1. Lo stub del controller/service
2. Un documento (o prompt) con le regole di business

```
Prompt: "Implementa il metodo createLibro con queste regole:
- Se prezzo > 50€ setta raro = true
- Se genere GIALLO e anno < 1950, lancia errore
- Invia notifica admin se copieDisponibili > 0"
→ Output: Codice Java completo nel metodo
```

---

## 8. Generazione dello scaffolding: build time vs sviluppo manuale

### Il problema della generazione "una tantum"

Generare lo scaffolding una volta e metterlo in repo sembra comodo, ma nasconde un rischio: **la spec e il codice possono disallinearsi**. Se modifichi `api.yaml` ma dimentichi di rigenerare gli stub, il compilato non corrisponde più al contratto.

### La soluzione corretta: generazione a build time

Nel progetto reale l'approccio è:

```xml
<!-- pom.xml -->
<plugin>
    <groupId>org.openapitools</groupId>
    <artifactId>openapi-generator-maven-plugin</artifactId>
    <executions>
        <execution>
            <goals><goal>generate</goal></goals>
            <configuration>
                <inputSpec>${project.build.directory}/specs/api.yaml</inputSpec>
                <generatorName>jaxrs-spec</generatorName>
                <configOptions>
                    <interfaceOnly>true</interfaceOnly>
                </configOptions>
            </configuration>
        </execution>
    </executions>
</plugin>
```

Il codice generato finisce in `target/` e **non viene committato**. Ogni `mvn clean compile` riparte dalla spec.

### Versioning efficace della spec

Affinché questo funzioni serve che la spec sia **versionata e recuperabile in modo deterministico**:

| Approccio | Pro | Contro |
|-----------|-----|--------|
| **File in repo** (`src/main/resources/openapi.yaml`) | Sempre offline, versionato con Git | Devi ricordarti di aggiornarlo manualmente |
| **Registry centrale** (Apicurio, Artifactory) | Single source of truth per tutti i team | Serve rete, problemi se registry non raggiungibile |
| **Artifact Maven** | Gestito dal dependency management | Più complesso da configurare |

### Sviluppo offline con registry esterno

Se usi un registry centrale (es. Apicurio), il build fallisce senza rete. Soluzione: permettere di saltare il download e usare lo YAML già scaricato.

```xml
<properties>
    <spec.download.skip>false</spec.download.skip>
</properties>
```

```bash
# 1. Scarichi la spec (una volta o quando cambia versione)
mvn apicurio-registry:download

# 2. Sviluppi offline usando lo YAML già presente in target/specs/
mvn clean compile -Dspec.download.skip=true
```

Questo permette anche di **modificare temporaneamente lo YAML in locale** per fare prove (aggiungere un campo, testare un flusso) senza toccare il registry condiviso.

---

## 9. Verificare che il codice rispetti la spec

La generazione a build time tiene allineata l'**interfaccia** (gli stub nascono dalla spec), ma non garantisce che il **comportamento** corrisponda. Servono due controlli in CI, complementari:

**1. Contract testing** — genera richieste dalla spec e le lancia contro il servizio in esecuzione, verificando che status e corpo rispettino gli schemi.

```bash
# il servizio è in piedi (Testcontainers / docker compose)
schemathesis run api.yaml --url http://localhost:8080 --checks all
```

- **Schemathesis** — property-based, molto efficace su OpenAPI; trova risposte fuori schema, 500 non dichiarati, validazioni mancanti
- **Dredd** — più semplice, verifica gli esempi della spec
- **Pact** — contract testing consumer-driven, quando più servizi si parlano

**2. Diff di compatibilità** — a ogni PR confronta la nuova spec con quella di `main` e blocca i breaking change non dichiarati.

```bash
oasdiff breaking main:api.yaml api.yaml --fail-on ERR
```

- `oasdiff`, `openapi-diff` (openapitools)

**Regola:** un endpoint assente dalla spec, o una risposta che non rispetta lo schema, fa **fallire la pipeline**. Così la spec resta davvero la fonte della verità, invece di divergere in silenzio.

---

## 10. Struttura progetto SDD consigliata

```
progetto/
├── api.yaml                    ← Spec OpenAPI (fonte della verità)
├── README.md
├── docs/
│   ├── logica-prestiti.md      ← Requisiti funzionali specifici
│   └── adr/
│       └── 001-soglia-rari.md  ← Decisioni architetturali (opzionale)
├── src/
│   ├── main/java/...
│   │   ├── controller/         ← Implementazione endpoint
│   │   ├── service/            ← Logica di business
│   │   ├── repository/         ← Accesso dati
│   │   └── entity/             ← Entità JPA
│   └── test/java/              ← Test JUnit
```

---

## 11. Esempio completo di flusso

### Step 1 — Aggiorna la spec (se cambia il contratto)
```yaml
# api.yaml
Libro:
  properties:
    raro:
      type: boolean
      readOnly: true
      description: true se prezzo > 50€
```

### Step 2 — Genera/aggiorna lo stub
```java
public class Libro {
    private Boolean raro;  // generato automaticamente
}
```

### Step 3 — Documenta la logica (per te o per l'AI)
```markdown
<!-- docs/logica-rari.md -->
Regola: durante la creazione di un libro, se prezzo > 50€ 
il campo `raro` deve essere impostato a `true` automaticamente.
```

### Step 4 — Implementa
```java
public Libro create(Libro libro) {
    if (libro.getPrezzo().compareTo(new BigDecimal("50.00")) > 0) {
        libro.setRaro(true);
    }
    return repository.save(libro);
}
```

---

## 12. Checklist prima di iniziare a codificare

- [ ] La spec `api.yaml` è aggiornata con il nuovo campo/endpoint?
- [ ] Lo stub è stato generato o aggiornato?
- [ ] Ho chiaro dove finisce il contratto e inizia la logica?
- [ ] Ho scritto (o pensato) i requisiti funzionali da implementare?

---

## In sintesi

| Livello | Dove | Quando cambia |
|---------|------|---------------|
| **Contratto** | `api.yaml` | Quando cambia ciò che il client vede |
| **Logica** | Codice Java/Kotlin | Quando cambiano le regole di business |
| **Contesto** | `docs/*.md` | Quando serve spiegare decisioni complesse |

> **La spec guida l'interfaccia. La logica vive nel codice. L'AI aiuta a scrivere entrambi, ma nel giusto ordine.**
