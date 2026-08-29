---
title: "OpenCode — Agenti, Subagenti e Strumenti MCP"
date: 2026-04-24
description: "Guida completa all'architettura degli agenti di OpenCode, tool MCP e pattern di utilizzo"
categories:
  - AI
---

# OpenCode — Agenti, Subagenti e Strumenti MCP

## Concetti fondamentali

### Tipi di agenti

OpenCode ha due tipi di agenti:

- **Primary agent** — l'agente principale con cui interagisci direttamente. Coordina il lavoro e può invocare subagent. Si cambia con `Tab` o il keybind configurato.
- **Subagent** — agente specializzato invocato dal primary agent tramite il tool `Task`, oppure manualmente con `@nome` nel prompt.

Gli agenti built-in sono:
- Primary: `Build` (tutti i tool abilitati), `Plan` (tool limitati, solo analisi)
- Subagent: `General`, `Explore`

---

## Tool

### Tool built-in

OpenCode include tool nativi: `read`, `write`, `edit`, `bash`, `grep`, `glob`, `list`, `webfetch`, `websearch`, e il tool `Task` per invocare subagent.

### Tool MCP (Model Context Protocol)

MCP è il protocollo standard che fa da ponte tra tool custom e modello AI.

```
Tool (Python/Node/qualsiasi linguaggio)
    ↓ esposto tramite
MCP Server (protocollo standard)
    ↓ consumato da
OpenCode
    ↓ disponibile al
Modello AI (Sonnet, GLM, ecc.)
```

**Caratteristiche chiave:**
- I tool sono deterministici — codice normale, risultato prevedibile
- Il modello decide autonomamente quando e come usarli
- Lo stesso tool MCP funziona su qualsiasi client MCP (OpenCode, Claude Code, Cursor)
- I tool aggiungono token al contesto — usare solo quelli necessari

**Configurazione in `opencode.json`:**

```json
{
  "mcp": {
    "db-tools": {
      "type": "local",
      "command": ["python", "tools/db_server.py"],
      "enabled": true,
      "environment": {
        "DB_CONNECTION": "${DB_CONNECTION_STRING}"
      }
    }
  }
}
```

**Esempio tool custom — esporta schema DB:**

```python
from mcp.server import Server
import pyodbc, json

server = Server("db-tools")

@server.tool()
async def esporta_schema(connection_string: str, schema: str = "dbo") -> str:
    conn = pyodbc.connect(connection_string)
    cursor = conn.cursor()
    cursor.execute("""
        SELECT TABLE_NAME, COLUMN_NAME, DATA_TYPE, IS_NULLABLE
        FROM INFORMATION_SCHEMA.COLUMNS
        WHERE TABLE_SCHEMA = ?
        ORDER BY TABLE_NAME, ORDINAL_POSITION
    """, schema)
    risultato = {}
    for row in cursor.fetchall():
        tabella = row.TABLE_NAME
        if tabella not in risultato:
            risultato[tabella] = []
        risultato[tabella].append({
            "colonna": row.COLUMN_NAME,
            "tipo": row.DATA_TYPE,
            "nullable": row.IS_NULLABLE
        })
    return json.dumps(risultato, indent=2)
```

---

## Struttura delle directory

```
~/.config/opencode/
├── opencode.json       # config principale + MCP
├── agents/             # subagent custom globali
├── rules/              # regole globali
├── skills/             # skills globali
└── plugins/            # plugin custom

.opencode/              # livello progetto (alta priorità)
├── agents/             # subagent specifici del progetto
├── rules/
└── skills/
```

---

## Configurazione agenti

### Tramite opencode.json

```json
{
  "agents": {
    "mio-agente": {
      "description": "Descrizione di cosa fa e quando usarlo",
      "mode": "subagent",
      "model": "anthropic/claude-sonnet",
      "permission": {
        "read": "allow",
        "write": "allow",
        "edit": "allow",
        "bash": "ask"
      }
    }
  }
}
```

### Tramite file markdown

Crea un file `.md` in `agents/` — il nome del file diventa il nome dell'agente.

```markdown
---
description: Analizza codice C# legacy senza modificarlo.
             Usami PRIMA di qualsiasi migrazione per capire
             dipendenze, pattern e potenziali problemi.
mode: subagent
model: anthropic/claude-sonnet
permission:
  read: allow
  write: deny
  edit: deny
  bash: deny
---

Sei uno specialista nell analisi di codice .NET 4.7 e Mono legacy.
Il tuo compito è solo analizzare e descrivere, mai modificare.
```

### Opzioni frontmatter

| Opzione | Valori | Descrizione |
|---|---|---|
| `mode` | `primary`, `subagent` | tipo di agente |
| `model` | `provider/model-id` | modello da usare |
| `description` | stringa | come il primary agent capisce quando invocarlo |
| `hidden` | `true/false` | nasconde dall'autocomplete `@` ma rimane invocabile via Task |
| `permission.task` | glob pattern | quali subagent può invocare |

---

## Come il primary agent sceglie il subagent

**La `description` è il meccanismo di routing.**

Il primary agent ha nel contesto la lista di tutti i subagent con le loro descrizioni. Ragiona su quale corrisponde meglio al task — esattamente come decide quale tool MCP usare.

```
Più la descrizione è precisa e contestuale
→ migliore è il routing automatico
→ meno intervento manuale necessario
```

**Esempio descrizioni efficaci:**

```markdown
# analista.md
description: Analizza codice C# legacy .NET 4.7 e Mono senza modificarlo.
             Usami PRIMA di qualsiasi migrazione per capire
             dipendenze, pattern usati e potenziali problemi.

# migratore.md
description: Esegue la migrazione di componenti da .NET 4.7 a .NET 10
             seguendo il pattern Strangler Fig. Usami DOPO l analisi,
             mai direttamente senza contesto.

# reviewer.md
description: Verifica che il codice migrato compili e i test passino.
             Usami SEMPRE come ultimo step dopo una migrazione.
```

---

## Invocazione subagent

**Automatica** — il primary agent decide autonomamente tramite il tool `Task`

**Manuale** — `@nome-subagent` nel prompt:
```
@analista analizza il servizio OrdiniService
```

**Navigazione sessioni figlie:**
- `<Leader>+Down` — entra nella prima sessione figlia
- Permette di seguire il lavoro di ogni subagent indipendentemente

---

## Permessi

```json
"permission": {
  "read": "allow",
  "write": "allow",
  "edit": "ask",       // chiede conferma
  "bash": "deny",
  "mcp_db-tools": "allow",
  "task": {
    "*": "deny",
    "orchestratore-*": "allow"   // glob pattern
  }
}
```

Le regole sono valutate in ordine — l'ultima che corrisponde vince.

---

## Pattern architetturali

### Pattern 1 — Pipeline sequenziale

```
@orchestratore
    → @analista    (capisce il codice esistente)
    → @migratore   (esegue la migrazione)
    → @reviewer    (verifica il risultato)
```

### Pattern 2 — Lavoro parallelo fullstack

```
@orchestratore
    ├── @database   → migration, schema, repository
    ├── @backend    → API, business logic, DTOs
    └── @frontend   → componenti UI, pagine
```

Il primary agent passa il contesto tra i subagent — @database produce lo schema che @backend consuma, @backend produce il contratto API che @frontend consuma.

### Pattern 3 — Iterazione su lista

```
@orchestratore: "crea CRUD per: Ordini, Clienti, Prodotti, Fatture, Spedizioni"

per ogni sezione:
    → @database crea migration
    → @backend  crea API
    → @frontend crea pagine
```

Il modello mantiene il contesto tra le iterazioni — apprende dai pattern della prima sezione e li applica alle successive.

---

## Divisione dei ruoli

| Componente | Tipo | Responsabilità |
|---|---|---|
| Tool MCP | Codice deterministico | Raccoglie dati, esegue azioni concrete |
| Modello AI | Ragionamento | Decide quando usare i tool, interpreta i risultati |
| Subagent | Modello specializzato | Esegue task specifici con tool e permessi dedicati |
| Primary agent | Orchestratore | Coordina i subagent, mantiene il contesto globale |

---

## Applicazione a ToscanaFei

### Tool MCP utili

```
esporta-schema-db    → legge SQL Server, restituisce schema
verifica-build       → lancia dotnet build, restituisce errori
analizza-dipendenze  → legge i .csproj, mappa le dipendenze
leggi-changelog      → legge commit git per contesto storico
```

### Subagent consigliati

```
@legacy-analyzer     → capisce il codice Mono/.NET 4.7 esistente
@net10-migrator      → scrive il nuovo codice .NET 10
@ef-core-specialist  → gestisce la parte dati con EF Core
@test-writer         → genera e verifica i test
```

### Workflow tipo per ogni componente

```
1. @legacy-analyzer   analizza il componente (read + db-tools)
2. @net10-migrator    migra il codice (read + write + edit + bash)
3. @ef-core-specialist migra il layer dati (read + write + bash + db-tools)
4. @test-writer       genera e verifica i test (read + write + bash)
```

---

## Note pratiche

**Pattern plan → build**
Usa sempre @plan per generare e rivedere il piano prima di eseguire.
Un errore architetturale nel piano moltiplicato per N componenti è costoso.

**Permessi conservativi all inizio**
Configura `bash: ask` e `edit: ask` durante le prime iterazioni — visibilità su ogni step.
Rimuovi i vincoli quando hai fiducia nel template.

**Descrizioni precise**
La qualità del routing automatico dipende dalla qualità delle descrizioni.
Descrizione vaga → routing sbagliato → risultati imprevedibili.

**Contesto come valore**
Più contesto strutturato dai al sistema (tramite tool MCP, CLAUDE.md, rules)
meno sorprese nell output. La pianificazione dettagliata è l investimento più redditizio.