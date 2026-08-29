---
date: 2026-04-30
categories:
  - AI
draft: true
---

# Esempio Spec Kit - Book Management REST API

Spec Kit è un toolkit open source di GitHub per lo **Spec-Driven Development (SDD)** — un approccio dove le specifiche diventano "eseguibili" e guidano direttamente l'implementazione, anziché essere scartate dopo la codifica.

> **Repo sorgente**: https://gitlab.com/koji-ai-projects/book-api-openspec

---

## Fase 1 — Constitution (`/speckit.constitution`)

File: `.specify/memory/constitution.md`

```markdown
# Principi del Progetto

- API RESTful con JSON in input/output
- CRUD completo per ogni entità
- Versioning API: /api/v1/
- Validazione input lato server
- Proper HTTP status codes (201, 204, 400, 404, 409)
- Test coverage >= 80%
- Nessuna logica di business nei controller
```

## Fase 2 — Specify (`/speckit.specify`)

File: `.specify/features/books/spec.md`

```markdown
# Book Management API

## Overview
Un servizio REST per gestire un catalogo di libri.
Supporta creazione, lettura, aggiornamento e cancellazione (CRUD).
Ogni libro ha: titolo, autore, ISBN, anno di pubblicazione, genere.

## User Stories

### US-1: Creare un libro
Come bibliotecario, voglio aggiungere un nuovo libro al catalogo
per tenere traccia delle nuove acquisizioni.

- POST /api/v1/books
- Body: { "title": "Il Nome della Rosa", "author": "Umberto Eco",
           "isbn": "978-88-04-12345-6", "year": 1980, "genre": "Giallo" }
- Ritorna 201 con il libro creato (incluso id)
- Ritorna 409 se ISBN già esistente

### US-2: Elencare i libri
Come utente, voglio vedere tutti i libri per sfogliare il catalogo.

- GET /api/v1/books
- Supporta paginazione: ?page=1&size=20
- Supporta filtro per genere: ?genre=Giallo
- Ritorna 200 con array di libri + metadati paginazione

### US-3: Dettaglio libro
Come utente, voglio vedere i dettagli di un libro specifico.

- GET /api/v1/books/{id}
- Ritorna 200 con il libro
- Ritorna 404 se non trovato

### US-4: Aggiornare un libro
Come bibliotecario, voglio modificare i dati di un libro
per correggere errori o aggiungere informazioni.

- PUT /api/v1/books/{id}
- Body completo del libro aggiornato
- Ritorna 200 con il libro aggiornato
- Ritorna 404 se non trovato

### US-5: Cancellare un libro
Come bibliotecario, voglio rimuovere un libro dal catalogo.

- DELETE /api/v1/books/{id}
- Ritorna 204 senza body
- Ritorna 404 se non trovato

## Acceptance Criteria
- Input validato: titolo e autore obbligatori, ISBN formato valido
- Paginazione default 20 elementi, max 100
- Risposte coerenti: struttura uniforme per errori
```

## Fase 3 — Plan (`/speckit.plan`)

File: `.specify/features/books/plan.md`

```markdown
# Piano Tecnico - Book Management API

## Stack
- Backend: Java 21 + Spring Boot 3
- DB: PostgreSQL 16 con Flyway per le migrazioni
- Testing: JUnit 5 + MockMvc + Testcontainers

## Architettura
Layered:
Controller → Service → Repository → Database

## Struttura
src/main/java/com/example/books/
├── controller/BookController.java
├── service/BookService.java
├── repository/BookRepository.java
├── model/Book.java
├── dto/BookRequest.java
├── dto/BookResponse.java
├── dto/PagedResponse.java
├── exception/ErrorHandler.java
└── exception/NotFoundException.java

src/main/resources/
├── db/migration/V1__create_books_table.sql
└── application.yml
```

## Fase 4 — Tasks (`/speckit.tasks`)

File: `.specify/features/books/tasks.md`

```markdown
# Tasks - Book Management API

- [ ] T1: Creare entità Book e repository Spring Data JPA
- [ ] T2: Creare migration Flyway per tabella books
- [ ] T3: Creare DTO (BookRequest, BookResponse, PagedResponse)
- [ ] T4: Implementare BookService con logica CRUD + validazione ISBN
- [ ] T5: Implementare BookController con tutti gli endpoint REST
- [ ] T6: Implementare ErrorHandler globale per 400/404/409
- [ ] T7: Scrivere test di integrazione per ogni endpoint
- [ ] T8: Configurare application.yml con pool Hikari e paginazione
```

## Fase 5 — Implement (`/speckit.implement`)

L'agente AI esegue ogni task seguendo gli artifact generati. Esempio per T5:

```java
@RestController
@RequestMapping("/api/v1/books")
public class BookController {

    private final BookService bookService;

    public BookController(BookService bookService) {
        this.bookService = bookService;
    }

    @PostMapping
    public ResponseEntity<BookResponse> create(@Valid @RequestBody BookRequest request) {
        var book = bookService.create(request);
        return ResponseEntity.status(201).body(book);
    }

    @GetMapping
    public ResponseEntity<PagedResponse<BookResponse>> list(
            @RequestParam(defaultValue = "1") int page,
            @RequestParam(defaultValue = "20") int size,
            @RequestParam(required = false) String genre) {
        return ResponseEntity.ok(bookService.findAll(page, size, genre));
    }

    @GetMapping("/{id}")
    public ResponseEntity<BookResponse> get(@PathVariable Long id) {
        return ResponseEntity.ok(bookService.findById(id));
    }

    @PutMapping("/{id}")
    public ResponseEntity<BookResponse> update(
            @PathVariable Long id, @Valid @RequestBody BookRequest request) {
        return ResponseEntity.ok(bookService.update(id, request));
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> delete(@PathVariable Long id) {
        bookService.delete(id);
        return ResponseEntity.noContent().build();
    }
}
```

---

## Nota su OpenAPI

Spec Kit non genera un `openapi.yaml` nativamente. Il contrato API vive nel `spec.md` in linguaggio naturale.
Se serve un OpenAPI esplicito:

1. **Richiederlo nello spec** — aggiungere che il deliverable include un file OpenAPI
2. **Derivarlo dal framework** — springdoc-openapi, Swashbuckle, ecc.
3. **Estensione community** — crearne una che generi openapi.yaml dallo spec.md

Il punto filosofico: in SDD lo `spec.md` è il contratto — OpenAPI è un formato macchina,
lo spec è il formato umano. L'AI lo legge e produce codice che rispetta quegli endpoint.

---

## Relazione tra CLI e Agente AI

Spec Kit **non è un tool che l'agente AI chiama** (tipo function calling). È un **framework di prompt strutturati** che l'agente consuma passivamente.

Il flusso è:

1. L'utente esegue `specify init --integration copilot` → genera file, template e comandi slash nella directory dell'agente
2. L'utente invoca `/speckit.specify` dentro Copilot/Claude/Gemini
3. L'agente **legge il prompt file** corrispondente e lo segue come istruzione
4. L'output dell'agente diventa un artifact (`.specify/features/books/spec.md`, ecc.)

La relazione è **spec-kit → agente**, non il contrario. Spec Kit dice all'agente cosa fare tramite i prompt files.

### Cosa fa la CLI `specify`

Gli slash commands (`/speckit.*`) sono ciò che l'agente AI esegue. La CLI è il **motore di orchestrazione** — senza CLI hai solo prompt files sparsi, con la CLI hai un sistema ripetibile e automatizzabile.

#### 1. Inizializzazione progetto

```bash
specify init mio-progetto --integration copilot   # crea struttura .specify/ + file agente
specify init --here --integration claude          # init in directory esistente
specify check                                       # verifica tools installati
specify version                                     # info versione
```

Crea `.specify/` con template, script, e i file di integrazione per l'agente scelto (`.claude/commands/`, `.github/copilot/`, ecc.).

#### 2. Gestione estensioni e preset

```bash
specify extension search          # cerca estensioni community
specify extension add <name>      # installa un'estensione
specify extension remove <name>   # rimuove

specify preset search             # cerca preset
specify preset add <name>         # installa un preset
specify preset remove <name>      # rimuove
```

Le estensioni aggiungono nuove funzionalità (integrazione Jira, code review, QA). I preset personalizzano i template esistenti (terminologia, standard organizzativi). La CLI gestisce priorità, conflitti e rollback.

#### 3. Workflows automatizzati

```bash
specify workflow run speckit \
  -i spec="Build a kanban board" \
  -i scope=full
```

Catene multi-step con:
- **Comandi** → esegue uno slash command (es. `speckit.plan`)
- **Gate** → pausa per approvazione umana
- **Shell** → esegue comandi di sistema
- **Condizioni, loop, fan-out/fan-in**

Il workflow built-in `speckit` esegue: `specify → gate(approvazione) → plan → gate(approvazione) → tasks → implement`

Se si interrompe (es. al gate), si riprende con:
```bash
specify workflow resume <run_id>
```

Stato persistente in `.specify/workflows/runs/<run_id>/`.

### In sintesi

| Ruolo | Chi agisce |
|-------|-----------|
| Generare lo scaffolding | **Utente** via `specify init` |
| Aggiungere estensioni/preset | **Utente** via `specify extension/preset add` |
| Automatizzare il ciclo SDD | **Utente** via `specify workflow run` |
| Eseguire le fasi (specify, plan, tasks, implement) | **Agente AI** leggendo i prompt files |
| Leggere e seguire le istruzioni | **Agente AI** — non chiama spec-kit, lo consuma |
