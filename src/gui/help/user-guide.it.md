# Come usare ImageWorker

ImageWorker risponde a due domande su una cartella di file `png` e `jpg`:

- **Dove compare questo asset nel mio screenshot?** — il compito principale.
- **Quali file di questa cartella sono duplicati?** — quello secondario.

---

## 1. Puntare a una cartella

**Immagini** è la cartella in cui cercare. **Indice in** è il posto dove
ImageWorker tiene i propri dati: il database, le anteprime e i descrittori.

Per impostazione predefinita l'indice finisce in una cartella nascosta
`.imageworker` dentro la cartella delle immagini. Cambia **Indice in** quando le
immagini stanno su una condivisione di rete o di sola lettura, quando preferisci
non aggiungere nulla al repository di qualcun altro, oppure quando l'indice sta
meglio su un disco più veloce. **Predefinito** rimette tutto a posto.

Premi **Indicizza**. Il primo passaggio legge ogni file; quelli successivi
rileggono solo i file la cui dimensione o data di modifica è cambiata, così
tenere aggiornato l'indice costa poco.

> Indicizzare 4600 immagini per circa 400 MB richiede all'incirca tre minuti,
> descrittori neurali compresi.

---

## 2. Trovare un asset dentro uno screenshot

Apri **Ricerca per esempio**, poi porta dentro lo screenshot:

- premi **Ctrl+V** per incollarlo dagli appunti — funziona da entrambe le
  schede;
- trascina un file immagine sulla finestra;
- oppure usa **Sfoglia…**.

**Traccia un riquadro attorno all'oggetto che cerchi.** Conta più di quanto
sembri: sull'immagine intera la ricerca spende gran parte dello sforzo sullo
sfondo. Su una libreria di 4600 asset un oggetto inquadrato richiede circa
**2,6 secondi contro 5,6** per l'immagine intera, e trova all'incirca **il
doppio dei punti corrispondenti**, perché ognuno di essi cade sull'oggetto e non
sulla scenografia. Clicca fuori dal riquadro per cancellarlo e cercare di nuovo
sull'immagine intera.

Premi **Cerca**. Il risultato migliore viene contornato sullo screenshot, e tutti
i risultati compaiono sotto, in ordine.

### Leggere i risultati

Ogni risultato mostra un **punteggio** e un **numero di inlier**. Gli inlier sono
i punti abbinati che concordano su un'unica collocazione coerente: è lì che sta
la prova. Il punteggio pesa quanto quegli abbinamenti fossero *coerenti*, e cala
quando sono pochi.

**Il risultato è un elenco ordinato, non una risposta sì o no.** Un altro disegno
dello stesso personaggio supera anch'esso la verifica, perché gran parte
dell'immagine corrisponde davvero. Quello giusto si colloca nettamente più in
alto: leggi quindi l'ordine invece di cercare un unico vincitore.

### Metodo

- **Individua un oggetto nell'immagine** — la ricerca neurale descritta sopra.
  Richiede un indice con descrittori; se la voce è in grigio, vedi sotto.
- **Somiglianza dell'immagine intera** — un hash percettivo che confronta le
  immagini nel loro insieme. Utile per ritrovare una copia ridimensionata o
  ricodificata *della stessa immagine*, inutile per trovare un piccolo oggetto
  dentro uno screenshot.

Se **Individua un oggetto** è disattivato, l'indice non ha ancora descrittori.
Creali una volta dalla riga di comando:

```
imageworker index "<cartella immagini>" --db "<cartella indice>\index.db" --features
imageworker vocab "<cartella immagini>" --db "<cartella indice>\index.db"
```

---

## 3. Trovare i duplicati

Apri **Duplicati**, imposta **Distanza max** e premi **Trova duplicati**.

- I gruppi **esatti** sono file identici byte per byte. Su questi si può agire
  tranquillamente.
- I gruppi **simili** si assomigliano agli occhi di un hash percettivo.
  **Esaminali, non cancellarli alla cieca.** L'hash descrive la composizione,
  perciò uno stesso personaggio disegnato con più espressioni finisce in un solo
  gruppo a qualsiasi distanza utilizzabile.

Distanza 0 significa impronte identiche; 4 tollera ricodifica e
ridimensionamento. Oltre 8 i gruppi servono solo per la revisione.

I gruppi sono elencati per quanto spazio sprecano, dal maggiore al minore. Dentro
un gruppo il file più grande e con la risoluzione più alta viene per primo: di
solito quello da tenere.

---

## 4. Tirare fuori i percorsi

Ogni risultato porta con sé il percorso completo:

- la barra in basso mostra il percorso della selezione corrente, e **Copia**
  copia tutti i percorsi selezionati, uno per riga;
- la piccola icona di copia accanto a un nome di file copia solo quello;
- il tasto destro offre il percorso completo, il percorso relativo alla cartella,
  oppure mostra il file nel gestore file.

---

## 5. Altri branch git

Se la cartella delle immagini si trova in un repository git, **Branch…** diventa
disponibile. Spunta i branch che ti servono e premi **Indicizza**: le loro
immagini vengono lette direttamente dal repository, quindi non viene fatto alcun
checkout e il tuo albero di lavoro resta intatto. Il branch su cui sei non
compare nell'elenco: è *lui* l'albero di lavoro.

L'elenco dei branch è il quadro completo di ciò che l'indice contiene. Togliere
la spunta e indicizzare di nuovo lo rimuove.

I risultati che vengono da un branch mostrano il nome del branch e si presentano
come `branch:percorso` invece che come percorso di file, perché quel file sul
disco non esiste. **Apri** estrae quella versione in un file temporaneo così puoi
guardarla; mostrarla nel gestore file e cercare con essa valgono solo per i file
dell'albero di lavoro.

Reindicizzare costa poco: un branch la cui punta non si è mossa viene saltato del
tutto, e dentro un branch vengono riletti solo i file il cui contenuto è
davvero cambiato.

> **Git LFS**: le immagini vengono risolte dal tuo archivio LFS locale. Se un
> branch non è mai stato scaricato, ImageWorker dice quanti oggetti mancano e che
> `git lfs fetch --all` li recupera: da solo non scarica nulla.

---

## 6. Temi e lingua

**Visualizza → Tema** passa tra Sistema, Sci-fi, Material e Fiorito mentre il
programma è in esecuzione. La scelta viene ricordata.

**Visualizza → Lingua** cambia l'interfaccia, anch'essa senza riavvio. L'elenco
contiene tutte le lingue che questa build porta con sé; **Lingua di sistema**
segue il sistema operativo. Anche questo manuale è tradotto, e ripiega
sull'inglese per una lingua il cui manuale non è ancora pronto.

---

## Cosa non sa fare

- L'hash percettivo trova **la stessa immagine**, mai semplicemente un soggetto
  simile.
- La ricerca neurale trova asset **realmente presenti** nell'immagine. Non è una
  ricerca per descrizione.
- Un asset che non produce alcun punto chiave — una campitura piatta, un'icona
  minuscola — non può essere individuato. Circa il 4% di una libreria di asset
  tipica ricade in questa categoria.
