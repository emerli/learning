# Cosa ho imparato costruendo un agent loop su Ollama locale

Riassunto dell'esperienza del progetto di studio: un assistente di sviluppo / GitOps che gira su un modello locale
(Ollama, senza GPU) e chiama tool Python per clonare, compilare e creare immagini.
Il dettaglio operativo sta in `CLAUDE.md`; qui c'è la storia e le conclusioni.

Le prove sui modelli sono **poche (spesso una sola)**: i risultati sono indicazioni da ripetere 3-5 volte, non misure.

---

## 1. Il progetto in breve

- **Obiettivo:** capire come funziona un agent loop con tool calling su un modello piccolo e locale, e farne un caso utile.
- **Loop:** `ollama.chat(model, messages, tools=[funzioni])`. La libreria ricava lo schema dei tool dalla firma, dai type
  hints e dalla docstring. Il modello risponde con testo o con `tool_calls`; il codice esegue il tool, aggiunge il risultato
  a `messages` (`role: "tool"`) e richiama il modello. Si esce quando non ci sono più chiamate.
- **Memoria:** solo la lista `messages`. Il codice non pianifica nulla.
- **Principio:** le parti deterministiche restano nel codice (validazione, path, ordine dei passi, permessi). Il modello
  interpreta la richiesta e sceglie tool e parametri. *L'agent propone, un umano approva, la pipeline esegue.*
- **Agente o workflow?** Con una pipeline quasi tutta deterministica (~90%) il progetto è un **workflow con interfaccia in
  linguaggio naturale**. Il valore del modello sta nelle richieste a testo libero, nei casi ambigui, nell'interpretazione
  degli errori.

## 2. Le tappe

1. **Build Maven in container (Podman).** Podman rootless su Fedora: nomi immagine completi (`docker.io/library/...`) e
   `:z` sul mount per SELinux (`-v path:/src:z`), altrimenti `Permission denied` su `/src`. L'esito si legge dal codice
   di uscita, non da `stderr` (Maven scrive gli errori su stdout).
2. **Path calcolato dal nome del progetto**, mai passato dal modello. `is_clean` per i nomi, `project_path` per il path.
3. **Validazione di workspace e URL.** Perimetro nella home, niente cartelle nascoste, `--` prima dell'URL,
   `GIT_ALLOW_PROTOCOL=ssh:https`, comandi come lista (niente shell).
4. **Registro dei progetti.** Prima `projects.json` con una dataclass, poi un `config.json` unico (pydantic) con
   `workspace`, `registry`, `environments`, `projects`. Il modello vede nome e descrizione, mai gli URL.
5. **Config modificabile solo da dove serve.** Workspace e ambienti si cambiano **a mano nel file**: un tool che li scrive
   permetterebbe al modello di spostare il perimetro per aggirare un errore (o per prompt injection). I progetti si
   aggiungono con tool e conferma `[s/N]`.
6. **Release dell'immagine.** `release_image_tool` restituisce una ricetta con le chiamate esatte ("NON ancora eseguito.
   Chiama ora, in ordine: ..."); il modello la esegue: clone, build, immagine.
7. **Confronto fra modelli** sulla stessa richiesta (sezione 5).

## 3. Lezioni sul tool calling

**Cosa vede il modello**
- Nome del tool, docstring prima di `Args:` (per intero) e descrizioni dei parametri. `Returns:` non arriva.
- Il testo **restituito** da un tool lo legge solo dopo averlo chiamato: una regola nel `return` non orienta la scelta
  del tool.
- Il nome in `Args:` deve coincidere col parametro, altrimenti la descrizione è vuota. Il nome del tool è `func.__name__`.

**Dove le istruzioni funzionano**
- Nella frase del tool: "se non è stato clonato, clonalo prima di compilare" ha prodotto build → errore → clone → build.
- Nel messaggio utente e nel testo restituito dal tool, se imperative e con la chiamata esatta.
- Nella descrizione di un **parametro** vengono spesso ignorate, soprattutto dai modelli piccoli.

**Valori**
- Valori liberi nella firma (`workspace_folder`, `tag`, `version`) vengono **inventati** (`/`, `.`, `my_workspace`,
  `$(default)`, `dev-latest`, `v1.0.0-test`). Valori scritti letterali nel testo restituito vengono ricopiati, a volte
  male (`ki/devops-example` al posto di `koji/devops-example`).
- Regola pratica: ogni invenzione indica un parametro da togliere dalla firma o da validare.

**Condizioni**
- Condizioni sullo **stato esterno** ("se c'è il Dockerfile", "se è clonato"): il modello non può verificarle. Nel codice,
  con `saltato: ...` o un errore che dice la chiamata da fare.
- Condizioni su **valori nel contesto** (parametro scelto da lui, risultato di un tool): le valuta bene
  (ha mappato "sviluppo" a `dev` e preso il ramo giusto della ricetta).

**Errori**
- Un errore opaco (`path non valido`) fa ipotizzare cause inventate o cercare vie alternative (tool generico, URL
  inventati). L'errore deve dire la regola violata **e i valori validi**.
- "Se un tool ritorna un errore, interrompi" è un'istruzione, non un vincolo: il modello prosegue lo stesso. Per fermare
  davvero serve un'eccezione dedicata intercettata nel loop.
- Un errore che dice solo la regola ("deve stare dentro /home/emerli") fa inventare un valore che la rispetta
  (`~/devops-example`); per ottenere il valore giusto va scritto nell'errore, oppure il parametro deve sparire.

**Tool**
- Due tool con descrizioni sovrapposte ("compila" / "crea immagine") confondono i modelli piccoli.
- Un tool non inserito in `TOOLS` fa inventare un nome plausibile. Il tool generico (`run_command`) viene scelto se uno
  specifico fallisce: non esporre tool che aggirano i controlli.
- Il testo di successo di un helper arriva come verità: `"ok: Container docker creato"` ha prodotto il riepilogo
  "container creato". Dire cosa è stato fatto e cosa no, derivando ok/errore dal `returncode`.

**Riepilogo finale**
- Il modello ripete le parole della richiesta ("rilasciata", "nell'ambiente dev") anche se è stata creata solo un'immagine
  locale. Meglio costruirlo dal codice partendo dai risultati dei tool.

## 4. Sicurezza: il codice decide, non il prompt

- Path confinati con `realpath` + `commonpath`; workspace assoluto, dentro il perimetro, senza componenti nascosti.
- Allowlist invece di denylist; il registro dei progetti è la lista dei ammessi.
- Nessun tool che modifichi il perimetro (workspace, ambienti) esposto al modello.
- Conferma `[s/N]` solo per i comandi generici o per l'aggiunta di un progetto.
- Lo sporco delle prove (`devops-example/`, `my_workspace/` creati dentro il repo) è nato da una validazione troppo larga:
  accettava path relativi che cadono in home.
- I log di compilazione e i report dei test sono output di codice di terzi: possibili veicoli di prompt injection e di
  segreti. Passare al modello solo un estratto filtrato.

## 5. Confronto fra modelli

Richieste: "compila il progetto devops-example" e "crea una release per devops-example in dev nel workspace di default".
Tempi dagli header `Date` delle risposte; Maven nel container ~22 s a parte.

| Modello | Esito | Tempo | Note |
|---|---|---|---|
| `qwen3.5:4b` (thinking) | corretto | ~127 s, 15-30 s a giro | cerca i valori con i tool di scoperta, recupera il clone mancante, riepilogo fedele; a volte inventa il tag libero |
| `granite4:3b` | corretto con ricetta completa | ~64 s, 4-6 s a giro | con istruzioni complete non inventa; con prompt libero è intraprendente ma sbaglia i dettagli (`.`, `my_workspace`) e non cerca mai i valori da solo |
| `qwen3:1.7b` (thinking) | immagine con tag sbagliato | >90 s | chiamate dipendenti in parallelo, versione inventata, tag storpiato, riepilogo con fatti falsi |
| `qwen3.5:0.8b` | corretto con percorso sporco | n.d. | tool sbagliato al primo colpo, `workspace_folder='/'` respinto dalla validazione, poi recupera |
| `qwen2.5:3b` | fallito | ~34 s | inventa il workspace, mette il tag in `version`, non esegue la ricetta (la spiega) |
| `llama3.2:1b` | fallito | n.d. | copia lo schema del tool come argomenti |
| `deepseek-r1:1.5b` | fallito | n.d. | ignora i tool, prende il system prompt per la richiesta |

**Conclusioni**
- Il risultato qui è che se si decide per un agente un po piu deterministico  
  granite eccelle se invece si vuole qualcosa che usa meglio il thinking qwen 3.5 	  mi sembra migliore  
- Il fattore decisivo è l'**addestramento al tool calling**, non la taglia (0.8B qwen3.5 regge, 1B llama no). Il tag `tools`
  di Ollama dice solo che il template supporta le chiamate.
- **Due profili:** agente deterministico (ricetta nel codice, il modello sceglie il tool e copia i parametri) → Granite,
  veloce; agente che decide (richieste libere, valori da cercare, errori da interpretare) → Qwen 3.5 4B, lento ma attento.
- **Il tempo è ragionamento:** un giro con thinking costa 15-35 s, senza 4-6 s. Il riepilogo finale è il giro più lento
  (genera molto testo): un altro motivo per farlo scrivere al codice.
- **Banco di prova:** il modello più debole che regge (Granite) verifica la robustezza del codice; il 4B il comportamento
  libero. Ogni valore storto prodotto da un modello è un caso di test per la validazione.

## 6. Decisioni su versioning e rilascio

- La **versione è un input** (tag git), mai deciso dal modello. Il modello la trasmette se l'utente la dice; il codice
  verifica che il tag esista.
- Il tag va estratto **prima** di compilare (clone → checkout → build → immagine), altrimenti l'immagine dichiara una
  versione che non contiene.
- Trunk-based: la sorgente di una build è un tag (o HEAD per lo sviluppo), il branch non è un parametro.
- Il pom non si modifica nel repo; se serve, `versions:set` nella copia usa-e-getta. Nome immagine `koji/{progetto}:{versione}`,
  niente `latest` automatico. Creare i tag è un atto di rilascio umano.
- Per ambiente (`dev`, `test`): regole come **dati** in config (sorgente, tag dell'immagine), non come procedure in italiano.
  Il modello sceglie ambiente e versione da un insieme chiuso.
- "Già clonato" non significa "aggiornato": opzioni `git pull --ff-only` oppure rimuovere e riclonare un workspace
  usa-e-getta.


