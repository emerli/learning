---
date: 2026-04-30
categories:
  - Agile
---

# Ripasso Scrum - Guida strutturata

## 3 Pilastri
**Trasparenza** | **Ispezione** | **Adattamento**

## 5 Valori
**Impegno** | **Focus** | **Apertura** | **Rispetto** | **Coraggio**

---

## Ruoli (3)

| Ruolo | Responsabilità |
|-------|---------------|
| **Product Owner** | Maximizza il valore, gestisce il Product Backlog, definisce priorità |
| **Scrum Master** | Protegge il framework Scrum, rimuove ostacoli, coaching |
| **Developers** | Auto-organizzati, creano l'Increment, pianificano il lavoro tecnico |

## Eventi (5)

| Evento | Timebox (Sprint 2 sett.) | Scopo |
|--------|--------------------------|-------|
| **Sprint** | 2 settimane | Container di tutti gli altri eventi |
| **Sprint Planning** | max 4h | Risponde: Perché? Cosa? Come? → Sprint Goal + Sprint Backlog |
| **Daily Scrum** | 15 min | Allineamento giornaliero verso lo Sprint Goal |
| **Sprint Review** | max 2h | Demo + feedback degli stakeholder |
| **Sprint Retrospective** | max 90 min | Miglioramento del processo/team |

## Artefatti (3) + Impegni

| Artefatto | Impegno | Proprietà |
|-----------|---------|-----------|
| **Product Backlog** | Product Goal | Lista ordinata di tutto ciò che serve nel prodotto |
| **Sprint Backlog** | Sprint Goal | Obiettivo dello sprint + item selezionati + piano di lavoro |
| **Increment** | Definition of Done | Somma di tutti i completati; deve essere utilizzabile |

---

## Concetti chiave da ricordare

- **Definition of Done (DoD)** - criteri condivisi per dire che un PBItem è "fatto"
- **Sprint Goal** - obiettivo unico per lo sprint, negoziato, non solo somma di task
- **Product Goal** - visione a lungo termine del prodotto
- **No cambiamenti** durante lo Sprint che mettano a rischio lo Sprint Goal
- **Sprint cancellabile** solo dal Product Owner
- **Nessun uomo in meno** di 10 o più di 50: team piccolo, 1 PO, 1 SM, resto Developers

---

## AI e Scrum

### AI come Developer
Trattare l'AI come membro produttivo del team. L'umano fa da PO/SM, l'AI esegue. Assegni task, ricevi PR, fai code review.

### Dati di input per l'AI (nel prompt)
Questi vanno nel contesto/prompt di sistema per allineare l'AI agli standard del team:

| Input | Ruolo nel prompt | Effetto |
|-------|-------------------|---------|
| **Sprint Goal** | Filtra cosa l'AI deve/può proporre | L'AI lavora verso l'obiettivo corretto |
| **DoD** | Gate di qualità: un item è "done" solo se rispetta la DoD | L'AI consegna codice coerente con gli standard del team |
| **Product Goal** | Visione a lungo termine | L'AI propone soluzioni allineate al prodotto |

Senza questi input, l'AI lavora nel vuoto. Con questi, produce output coerenti con gli standard del team.

### Dove l'AI è efficace

- **Product Backlog** - generare user stories, criteri di accettazione, stimare complessità
- **Sprint Planning** - scomporre PBItems in task più fini
- **Daily Scrum** - riassumere stato e blocchi
- **Retrospective** - analizzare pattern ricorrenti, suggerire miglioramenti

### Dove fare attenzione

- Il **Product Owner** rimane umano: l'AI non conosce il contesto business profondo
- Lo **Sprint Goal** va negoziato tra umani, l'AI può solo supportare
- La **DoD** va definita dal team, non generata dall'AI

### Workflow Scrum + AI

```
1. Product Goal (umano)
   ↓
2. AI genera Product Backlog Items (storie + criteri di accettazione)
   ↓
3. PO review + prioritizza il backlog (umano)
   ↓
4. Sprint Planning: AI scompone i PBItems in task (Sprint Backlog)
   ↓
5. AI implementa i task (code)
   ↓
6. PO verifica DoD sull'incremento (umano)
   ↓
7. Sprint Review + Retro (umano, AI supporta)
```

Principio: l'AI **propone**, l'umano **decide**. Il PO valida prima di entrare nello sprint.

### Modello Agenti

```
┌─────────────────────────────────────────┐
│  Agente SM  │  Agente DEV  │  Umano PO  │
│  (orchestra)│  (implementa)│  (decide)   │
└─────────────────────────────────────────┘
```

- **SM Agent** - facilita, ricorda le regole Scrum, tracka ostacoli
- **DEV Agent** - scrive codice, scompone task, verifica tecnica
- **PO (umano)** - decide priorità, approva merge, verifica valore business

### DoD come regola automatizzabile

Se la DoD è espressa come regole verificabili, l'AI può auto-verificare la maggior parte:

```
DoD: "implementato + test green + merge"

AI verifica automaticamente:
  ✅ Codice scritto        → verificabile
  ✅ Test green             → verificabile (CI)
  ⏳ Merge                  → richiede approvazione umana
```

Flusso stato task:

```
DEV Agent → implementa + esegue test
                ↓
SM/DEV Agent → auto-verifica DoD automatizzabile
                ↓
         ├─── fallita ──→ torna al DEV
         └─── superata ──→ notifica PO per il gate umano (merge)
                              ↓
                        PO approva → "chiuso"
```

Il PO interviene solo dove serve la sua decisione, non per verificare cose che l'AI può verificare da sola.
