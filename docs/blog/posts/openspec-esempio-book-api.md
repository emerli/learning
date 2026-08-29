---
date: 2026-04-30
categories:
  - AI
---

# Esempio OpenSpec - Book Management REST API

OpenSpec è un framework SDD (Spec-Driven Development) leggero, iterativo e brownfield-first.
A differenza di Spec Kit, usa un approccio basato su **change proposal** con **spec delta** per tracciare le modifiche ai requisiti.

> **Repo sorgente**: https://gitlab.com/koji-ai-projects/book-api-spec-kit

---

## Installazione e Init

```bash
npm install -g @fission-ai/openspec@latest
cd mio-progetto
openspec init
```

Questo crea la struttura `openspec/` nel progetto e configura i comandi slash per l'agente AI scelto.

---

## Fase 1 — Propose (`/opsx:propose`)

Si descrive cosa si vuole costruire e OpenSpec genera l'intera cartella del change:

```
/opsx:propose "Un servizio REST per la gestione di un catalogo di libri. Supporta CRUD completo: creare, elencare (con paginazione e filtri), vedere dettaglio, aggiornare e cancellare libri. Ogni libro ha titolo, autore, ISBN, anno di pubblicazione e genere."
```

OpenSpec genera:

```
openspec/changes/add-book-api/
├── proposal.md        ← cosa e perché
├── specs/
│   └── book-api/
│       └── spec.md    ← requisiti e scenari (GIVEN/WHEN/THEN)
├── design.md          ← approccio tecnico
└── tasks.md           ← checklist implementazione
```

### proposal.md

```markdown
# Proposal: Book Management REST API

## Summary
Un servizio REST per gestire un catalogo di libri con CRUD completo,
paginazione e filtri.

## Motivation
Necessità di un backend per la gestione del catalogo librario che consenta
operazioni standard e consultazione efficiente.

## Scope
- API REST con endpoints CRUD per libri
- Paginazione e filtro per genere
- Validazione input (titolo/autore obbligatori, ISBN formato valido)
- Gestione duplicati ISBN (409 Conflict)

## Out of Scope
- Autenticazione e autorizzazione
- Gestione autori come entità separata
- Upload di copertine
```

### specs/book-api/spec.md

```markdown
# book-api Specification

## Purpose
Gestire il ciclo di vita dei libri nel catalogo: creazione, consultazione,
aggiornamento e cancellazione.

## Requirements

### Requirement: Create book
The system SHALL allow creation of a new book entry.

#### Scenario: Successful book creation
- GIVEN the catalog is available
- WHEN a user submits valid book data (title, author, ISBN, year, genre)
- THEN the system SHALL create the book and return it with a generated id
- AND respond with HTTP 201

#### Scenario: Duplicate ISBN
- GIVEN a book with ISBN "978-88-04-12345-6" already exists
- WHEN a user submits a book with the same ISBN
- THEN the system SHALL respond with HTTP 409
- AND include an error message indicating the ISBN conflict

#### Scenario: Missing required fields
- GIVEN the catalog is available
- WHEN a user submits a book without a title or author
- THEN the system SHALL respond with HTTP 400
- AND include validation error details

### Requirement: List books
The system SHALL provide a paginated list of books.

#### Scenario: Default pagination
- GIVEN the catalog contains 50 books
- WHEN a user requests GET /api/v1/books without parameters
- THEN the system SHALL return the first 20 books
- AND include pagination metadata (total, page, size)

#### Scenario: Filter by genre
- GIVEN the catalog contains books in multiple genres
- WHEN a user requests GET /api/v1/books?genre=Giallo
- THEN the system SHALL return only books with genre "Giallo"

### Requirement: Get book by id
The system SHALL return a single book by its identifier.

#### Scenario: Existing book
- GIVEN a book with id 1 exists
- WHEN a user requests GET /api/v1/books/1
- THEN the system SHALL return the book details with HTTP 200

#### Scenario: Non-existent book
- GIVEN no book with id 999 exists
- WHEN a user requests GET /api/v1/books/999
- THEN the system SHALL respond with HTTP 404

### Requirement: Update book
The system SHALL allow updating an existing book's information.

#### Scenario: Successful update
- GIVEN a book with id 1 exists
- WHEN a user submits updated book data
- THEN the system SHALL update and return the book with HTTP 200

#### Scenario: Update non-existent book
- GIVEN no book with id 999 exists
- WHEN a user submits update data
- THEN the system SHALL respond with HTTP 404

### Requirement: Delete book
The system SHALL allow removal of a book from the catalog.

#### Scenario: Successful deletion
- GIVEN a book with id 1 exists
- WHEN a user requests DELETE /api/v1/books/1
- THEN the system SHALL remove the book and respond with HTTP 204

#### Scenario: Delete non-existent book
- GIVEN no book with id 999 exists
- WHEN a user requests DELETE /api/v1/books/999
- THEN the system SHALL respond with HTTP 404
```

### design.md

```markdown
# Design: Book Management REST API

## Architecture
Layered architecture: Controller → Service → Repository → Database

## Tech Stack
- Java 21 + Spring Boot 3
- PostgreSQL 16 con Flyway
- JUnit 5 + MockMvc + Testcontainers

## Data Model
Book: id (Long, auto), title (String, required), author (String, required),
     isbn (String, unique, required), year (Integer), genre (String)

## API Endpoints
| Method   | Path               | Description          | Status Codes    |
|----------|--------------------|----------------------|-----------------|
| POST     | /api/v1/books      | Create book          | 201, 400, 409   |
| GET      | /api/v1/books      | List books (paged)   | 200             |
| GET      | /api/v1/books/{id} | Get book by id       | 200, 404        |
| PUT      | /api/v1/books/{id} | Update book          | 200, 400, 404   |
| DELETE   | /api/v1/books/{id} | Delete book          | 204, 404        |

## Error Response Format
{ "status": 404, "error": "Not Found", "message": "Book not found", "path": "/api/v1/books/999" }

## Project Structure
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
```

### tasks.md

```markdown
# Tasks: Book Management REST API

## Phase 1: Foundation
- [ ] 1.1 Create Book entity and Spring Data JPA repository
- [ ] 1.2 Create Flyway migration for books table

## Phase 2: Core Logic
- [ ] 2.1 Create DTOs (BookRequest, BookResponse, PagedResponse)
- [ ] 2.2 Implement BookService with CRUD logic + ISBN uniqueness validation
- [ ] 2.3 Implement global ErrorHandler (400, 404, 409)

## Phase 3: API Layer
- [ ] 3.1 Implement BookController with all REST endpoints
- [ ] 3.2 Configure application.yml (Hikari pool, pagination defaults)

## Phase 4: Testing
- [ ] 4.1 Write integration tests for each endpoint
- [ ] 4.2 Write validation tests (missing fields, duplicate ISBN)
```

---

## Fase 2 — Apply (`/opsx:apply`)

L'agente AI implementa tutti i task:

```
/opsx:apply
```

Esempio di implementazione per il task 3.1:

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

## Fase 3 — Archive (`/opsx:archive`)

Una volta completata l'implementazione:

```
/opsx:archive
```

OpenSpec archivia il change e **aggiorna le spec persistenti** — le spec rimangono come documentazione viva nel repo:

```
openspec/specs/
└── book-api/
    └── spec.md    ← rimane come riferimento per sviluppi futuri

openspec/changes/archive/
└── 2026-04-29-add-book-api/
    ├── proposal.md
    ├── design.md
    ├── tasks.md
    └── specs/
```

---

## Spec Delta — la differenza chiave

Se in futuro aggiungiamo un "Remember me" all'autenticazione, OpenSpec genera un **spec delta**:

```markdown
### Requirement: Session expiration
- The system SHALL expire sessions after a configured duration.
+ The system SHALL support configurable session expiration periods.

#### Scenario: Default session timeout
- GIVEN a user has authenticated
- WHEN 24 hours pass without activity
+ WHEN 24 hours pass without "Remember me"
- THEN invalidate the session token

+ #### Scenario: Extended session with remember me
+ - GIVEN user checks "Remember me" at login
+ - WHEN 30 days have passed
+ - THEN invalidate the session token
+ - AND clear the persistent cookie
```

Le righe con `-` vengono rimosse, quelle con `+` vengono aggiunte. Questo rende le **PR review molto più efficaci** — il reviewer vede l'impatto sui requisiti prima del codice.

---

## Confronto rapido con Spec Kit

| Aspetto | Spec Kit | OpenSpec |
|---------|----------|----------|
| Flusso | constitution → specify → plan → tasks → implement | propose → apply → archive |
| Fasi | Lineari con gate obbligatori | Fluide, iterative, senza gate |
| Tracking modifiche | Task completati in tasks.md | Spec delta (diff dei requisiti) |
| Spec persistenti | `.specify/features/` | `openspec/specs/` (rimangono come doc viva) |
| Principio | Process-driven, greenfield-first | Leggero, brownfield-first |
| Cli | `specify init/extension/workflow` | `openspec init/update/config` |
| Dashboard | No (solo CLI) | Sì (web UI) |
