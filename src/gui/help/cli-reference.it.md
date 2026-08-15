# Riga di comando e automazione

Il pacchetto contiene **due eseguibili costruiti sulla stessa libreria di
base**:

| | |
|---|---|
| `imageworker-gui` | la finestra in cui stai leggendo questo testo |
| `imageworker` | senza interfaccia; tutto quello che fa la GUI, più opzioni di indicizzazione che la GUI non espone |

Condividono l'intera logica, quindi una cartella indicizzata dall'uno viene letta
dall'altro. Nessuno dei due avvolge l'altro: entrambi collegano lo stesso nucleo
statico.

---

## Contratto di output

Puoi farci affidamento negli script; è volutamente stabile.

- `--json` scrive **JSON delimitato da righe su stdout**, un oggetto per riga.
  Ogni risultato porta sia `rel` (relativo alla cartella indicizzata) sia `path`
  (assoluto).
- `--paths` scrive **soli percorsi assoluti**, uno per riga, per la pipe. In
  `dupes` una riga vuota separa i gruppi. Implica `--quiet`, e combinarlo con
  `--json` è un errore anziché una scelta silenziosa tra i due.
- Avanzamento, avvisi ed errori vanno sempre su **stderr**, mai su stdout.
- I risultati escono in un ordine deterministico.
- Codici di uscita: **0** trovato qualcosa, **1** non trovato nulla, **2** un
  errore.

---

## Comandi

```
imageworker index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
imageworker vocab  <dir> [--words 2048] [--sample 120000]
imageworker find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist N] [--top N]
imageworker dupes  <dir> [--distance 4] [--exact-only] [--near-only]
imageworker query  <dir> --image ref.png [--top 20]
imageworker match  --query q.png --asset a.png
imageworker stats  <dir>
imageworker doctor [--extract img.png]
imageworker formats
```

Ogni comando accetta `--db <path>` per mettere l'indice altrove rispetto a
`<dir>/.imageworker`.

### Preparare una cartella alla ricerca di oggetti

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` da solo dà il rilevamento dei duplicati. `--features` aggiunge i
descrittori neurali, e `vocab` addestra su di essi l'indice di preselezione;
servono entrambi prima che `find` funzioni. Rilancia `vocab` dopo aver aggiunto
un grosso lotto di asset, non dopo ogni singolo file.

### Cercare

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Passa `--roi x,y,w,h` quando sai dov'è l'oggetto. È l'opzione più efficace in
assoluto: su una regione inquadrata la ricerca è circa cinque volte più veloce e
trova all'incirca il doppio dei punti corrispondenti.

### Concatenare

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## Verificare l'installazione

```
imageworker doctor
```

Riporta le versioni di ONNX Runtime e OpenCV, i provider di esecuzione
disponibili, e se su questo driver una sessione DirectML si riesce davvero a
creare — cosa diversa dal vedere semplicemente il provider elencato.
`--extract <image>` fa girare l'estrattore su un file e riporta quanti punti
chiave ha trovato e quanto ci ha messo.

---

## Dove sta cosa

```
<cartella indice>/
  index.db              SQLite: file, hash, anteprime, offset dei descrittori
  features/desc.f16     descrittori, mezza precisione
  features/kpts.f32     punti chiave
  features/vocab.bin    vocabolario visivo
  features/bow.bin      indice di preselezione
```

Cancellare la cartella dell'indice azzera tutto e non tocca nient'altro. I
percorsi nel database sono salvati **relativi alla cartella indicizzata**, quindi
un indice resta valido quando lo stesso archivio viene montato altrove.

I modelli vengono cercati in `models/` accanto all'eseguibile, poi nelle cartelle
superiori — così funzionano senza configurazione sia un pacchetto installato sia
un albero di compilazione.

---

## Compilare

```
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF` compila senza ONNX Runtime e OpenCV; la ricerca
dei duplicati continua a funzionare, quella degli oggetti no.

Il passo di installazione mette insieme una cartella autosufficiente: ogni
dipendenza accanto agli eseguibili, niente letto dal `PATH`. Un singolo file
eseguibile non è possibile: ONNX Runtime con DirectML esiste solo come DLL, e
`DirectML.dll` è un componente ridistribuibile che non si può collegare
staticamente in alcun modo.
