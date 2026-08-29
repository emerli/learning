---
title: "OpenCode — Agenti, Subagenti, MCP e Workflow"
date: 2026-08-29
categories:
  - AI
slug: ai-opencode
description: "Architettura degli agenti di OpenCode: primari e subagent, tool MCP, permessi, pattern di orchestrazione e best practice di workflow."
---

# OpenCode — Agenti, Subagenti, MCP e Workflow

## Introduzione

OpenCode ha un'architettura multi-agente: agenti **primari** con cui interagisci direttamente e **subagent** specializzati, invocati automaticamente o a mano. Capire quando e come usarli — e come configurarne tool e permessi — è ciò che fa la differenza sui task complessi.

---

## Concetti fondamentali

I pezzi in gioco sono quattro: gli agenti (primari e subagent), i tool che possono usare, i permessi che li vincolano e il modello che ragiona.

### Tipi di agenti

- **Primary agent** — l'agente principale con cui interagisci. Coordina il lavoro e può invocare subagent. Si cambia con `Tab` o il keybind configurato.
- **Subagent** — agente specializzato invocato dal primary agent tramite il tool `Task`, oppure manualmente con `@nome` nel prompt.

### Agenti built-in

| Agente | Tipo | Scopo | Permessi |
|---|---|---|---|
| **Build** | primary | Sviluppo attivo, scrittura codice | Pieno accesso a file e Bash |
| **Plan** | primary | Analisi e pianificazione | `ask` su scrittura/modifiche, per evitare cambiamenti accidentali |
| **General** | subagent | Task multi-step generici | Pieno accesso ai tool (tranne `todo`) |
| **Explore** | subagent | Ricerca ed esplorazione in sola lettura | Solo lettura |

### `@explore` — esplorazione in sola lettura

Ideale per cercare file, pattern o informazioni nel codebase **senza modificare nulla**.

Quando usarlo:

- Trovare dove è definita una funzione
- Analizzare la struttura del progetto
- Cercare riferimenti a un servizio o a un'API

```
@explore dove viene validato l'input dell'utente nel modulo di login?
@explore trova tutte le funzioni che chiamano l'API di pagamento
```

### `@general` — task multi-step complessi

Subagent general-purpose con pieno accesso agli strumenti (tranne `todo`). Esegue task multi-step in autonomia.

Quando usarlo:

- Analisi comparativa di più funzioni
- Refactoring su più file
- Task paralleli delegati dall'agente primario

```
@general analizza queste tre funzioni e proponi un'ottimizzazione comune
```

### Divisione dei ruoli

| Componente | Tipo | Responsabilità |
|---|---|---|
| Tool MCP | Codice deterministico | Raccoglie dati, esegue azioni concrete |
| Modello AI | Ragionamento | Decide quando usare i tool, interpreta i risultati |
| Subagent | Modello specializzato | Esegue task specifici con tool e permessi dedicati |
| Primary agent | Orchestratore | Coordina i subagent, mantiene il contesto globale |

---

## Tool

Ogni agente ha accesso a un insieme di tool: quelli nativi di OpenCode e quelli custom aggiunti via MCP.

### Tool built-in

OpenCode include tool nativi: `read`, `write`, `edit`, `bash`, `grep`, `glob`, `list`, `webfetch`, `websearch`, e il tool `Task` per invocare subagent.

### Tool MCP (Model Context Protocol)

MCP è il protocollo standard con cui un client (OpenCode, Claude Code, …) espone al modello tool e dati forniti da server esterni. Il modello non parla MCP: chiede al client di usare un tool, il client lo inoltra al server MCP.

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

**Esempio di tool custom — esporta lo schema di un DB:**

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

Gli strumenti custom possono essere scritti in qualsiasi linguaggio (Python, JavaScript, …) e poi abilitati per agenti specifici nella configurazione.

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

Il livello progetto (`.opencode/`) ha priorità su quello globale.

---

## Configurazione agenti

Un agente si definisce nel JSON di config oppure, più comodamente, come singolo file Markdown.

### Tramite `opencode.json`

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

### Tramite file Markdown

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

Sei uno specialista nell'analisi di codice .NET 4.7 e Mono legacy.
Il tuo compito è solo analizzare e descrivere, mai modificare.
```

Salva in `.opencode/agents/analista.md` (progetto) o `~/.config/opencode/agents/analista.md` (globale).

### Opzioni frontmatter

| Opzione | Valori | Descrizione |
|---|---|---|
| `mode` | `primary`, `subagent` | tipo di agente |
| `model` | `provider/model-id` | modello da usare |
| `description` | stringa | come il primary agent capisce quando invocarlo |
| `hidden` | `true` / `false` | nasconde dall'autocomplete `@` ma rimane invocabile via `Task` |
| `permission.task` | glob pattern | quali subagent può invocare |
| `temperature` | `0.0`–`1.0` | bassa (0.0–0.2) per analisi deterministiche, alta (0.6–1.0) per brainstorming |

### Altri parametri utili

- `default_agent` — seleziona automaticamente l'agente primario preferito all'avvio
- `max_iterations` — limita i passaggi massimi per un task complesso

---

## Routing: come il primary agent sceglie il subagent

**La `description` è il meccanismo di routing.**

Il primary agent ha nel contesto la lista di tutti i subagent con le loro descrizioni. Ragiona su quale corrisponde meglio al task — esattamente come decide quale tool MCP usare.

```
Più la descrizione è precisa e contestuale
→ migliore è il routing automatico
→ meno intervento manuale necessario
```

**Esempio di descrizioni efficaci:**

```markdown
# analista.md
description: Analizza codice C# legacy .NET 4.7 e Mono senza modificarlo.
             Usami PRIMA di qualsiasi migrazione per capire
             dipendenze, pattern usati e potenziali problemi.

# migratore.md
description: Esegue la migrazione di componenti da .NET 4.7 a .NET 10
             seguendo il pattern Strangler Fig. Usami DOPO l'analisi,
             mai direttamente senza contesto.

# reviewer.md
description: Verifica che il codice migrato compili e i test passino.
             Usami SEMPRE come ultimo step dopo una migrazione.
```

---

## Invocazione subagent

**Automatica** — il primary agent decide da solo tramite il tool `Task`.

**Manuale** — `@nome-subagent` nel prompt:

```
@analista analizza il servizio OrdiniService
```

**Navigazione sessioni figlie:**

- `<Leader>+Down` — entra nella prima sessione figlia
- Permette di seguire il lavoro di ogni subagent indipendentemente

---

## Permessi

Esempio di blocco `permission` (sintassi JSONC, i commenti sono ammessi):

```jsonc
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

I tre livelli sono:

- **`allow`** — l'agente usa lo strumento senza chiedere
- **`ask`** — l'agente chiede conferma prima di usarlo (default di `Plan` su scrittura/modifiche)
- **`deny`** — l'agente non può usarlo

Le regole sono valutate in ordine: **l'ultima che corrisponde vince**.

**Regole pratiche:**

- Inizia con permessi conservativi (`ask` su `edit` e `bash`) durante le prime iterazioni — visibilità su ogni step
- Rimuovi i vincoli quando hai fiducia nel template e nel flusso
- Non invocare mai gli strumenti direttamente dal prompt: descrivi il compito in linguaggio naturale e lascia che l'agente scelga lo strumento giusto

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

Il primary agent passa il contesto tra i subagent: `@database` produce lo schema che `@backend` consuma, `@backend` produce il contratto API che `@frontend` consuma.

### Pattern 3 — Iterazione su lista

```
@orchestratore: "crea CRUD per: Ordini, Clienti, Prodotti, Fatture, Spedizioni"

per ogni sezione:
    → @database crea migration
    → @backend  crea API
    → @frontend crea pagine
```

Il modello mantiene il contesto tra le iterazioni: apprende dai pattern della prima sezione e li applica alle successive.

---

## Workflow: Plan → Build → Verify

```
1. @plan       → analizza e genera un piano dettagliato
2. @build      → esegue fase per fase
3. @general    → verifica il risultato (test, build, lint)
```

Il piano diventa il **contesto condiviso** per tutti gli agenti coinvolti. Un errore architetturale nel piano, moltiplicato per N componenti, è costoso da correggere: **usa sempre `Plan` prima di un task complesso**.

### Gestione delle dipendenze

| Approccio | Uso |
|---|---|
| **Plan** | Genera una roadmap con fasi logiche e dipendenze |
| **Orchestratore** | Un agente che delega a subagent specializzati e schedula i passi in parallelo o in sequenza in base alle dipendenze |

---

## File `AGENTS.md` per il contesto di progetto

In progetti complessi, crea un file `AGENTS.md` nella root del repository per fornire contesto gerarchico (struttura, convenzioni, comandi di build/test). Gli agenti ne leggono solo le parti rilevanti, migliorando la precisione.

---

## Esempio di applicazione: migrazione di un servizio legacy .NET

### Tool MCP utili

```
esporta-schema-db    → legge SQL Server, restituisce lo schema
verifica-build       → lancia dotnet build, restituisce gli errori
analizza-dipendenze  → legge i .csproj, mappa le dipendenze
leggi-changelog      → legge i commit git per il contesto storico
```

### Subagent consigliati

```
@legacy-analyzer     → capisce il codice Mono/.NET 4.7 esistente
@net10-migrator      → scrive il nuovo codice .NET 10
@ef-core-specialist  → gestisce la parte dati con EF Core
@test-writer         → genera e verifica i test
```

### Workflow per ogni componente

```
1. @legacy-analyzer    analizza il componente        (read + db-tools)
2. @net10-migrator     migra il codice               (read + write + edit + bash)
3. @ef-core-specialist migra il layer dati           (read + write + bash + db-tools)
4. @test-writer        genera e verifica i test      (read + write + bash)
```

---

## In sintesi

Più contesto strutturato dai al sistema (tool MCP, `AGENTS.md`, rules), meno sorprese nell'output: la pianificazione dettagliata è l'investimento più redditizio.

| # | Azione |
|---|---|
| 1 | Inizia sempre con `Plan` per i task complessi |
| 2 | Usa `@explore` per capire il codebase prima di modificare |
| 3 | Delega i task paralleli a `@general` |
| 4 | Verifica il risultato con test/build prima di proseguire |
| 5 | Crea `AGENTS.md` per il contesto progettuale nei repo grandi |
| 6 | Temperatura bassa per il refactoring, alta per il design |
| 7 | Inizia con permessi conservativi, allenta col tempo |
| 8 | Scrivi `description` precise: sono il meccanismo di routing |

---

## Riferimenti

- [Documentazione OpenCode](https://opencode.ai/docs) — documentazione ufficiale
