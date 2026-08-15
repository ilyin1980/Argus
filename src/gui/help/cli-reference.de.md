# Befehlszeile und Automatisierung

Das Paket liefert **zwei Programme, die aus derselben Kernbibliothek gebaut
sind**:

| | |
|---|---|
| `argus-gui` | das Fenster, in dem Sie das hier lesen |
| `argus` | ohne Oberfläche; alles, was die GUI kann, plus Indexoptionen, die sie nicht anbietet |

Beide teilen sich die gesamte Logik, ein von einem indizierter Ordner wird also
vom anderen gelesen. Keines ist ein Aufsatz auf das andere — beide linken
denselben statischen Kern.

---

## Ausgabevertrag

Darauf können Sie sich in Skripten verlassen; er ist absichtlich stabil.

- `--json` schreibt **zeilengetrenntes JSON nach stdout**, ein Objekt pro Zeile.
  Jedes Ergebnis trägt sowohl `rel` (relativ zum indizierten Ordner) als auch
  `path` (absolut).
- `--paths` schreibt **nackte absolute Pfade**, einen pro Zeile, zum
  Weiterleiten. Bei `dupes` trennt eine Leerzeile die Gruppen. Es impliziert
  `--quiet`; die Kombination mit `--json` ist ein Fehler statt einer stillen
  Entscheidung für eines von beiden.
- Fortschritt, Warnungen und Fehler gehen immer nach **stderr**, nie nach stdout.
- Ergebnisse kommen in einer festgelegten Reihenfolge.
- Rückgabewerte: **0** etwas gefunden, **1** nichts gefunden, **2** ein Fehler.

---

## Befehle

```
argus index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
argus vocab  <dir> [--words 2048] [--sample 120000]
argus find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist N] [--top N]
argus dupes  <dir> [--distance 4] [--exact-only] [--near-only]
argus query  <dir> --image ref.png [--top 20]
argus match  --query q.png --asset a.png
argus stats  <dir>
argus doctor [--extract img.png]
argus formats
```

Jeder Befehl nimmt `--db <path>`, um den Index anderswo abzulegen als in
`<dir>/.argus`.

### Einen Ordner für die Objektsuche einrichten

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` allein liefert die Duplikatsuche. `--features` fügt die neuronalen
Deskriptoren hinzu, und `vocab` trainiert darüber den Vorauswahl-Index; beides
ist nötig, bevor `find` funktioniert. Lassen Sie `vocab` nach einem größeren
Schwung neuer Assets erneut laufen, nicht nach jeder einzelnen Datei.

### Suchen

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Übergeben Sie `--roi x,y,w,h`, wenn Sie wissen, wo das Objekt ist. Es ist die mit
Abstand wirksamste Option: auf einem eingerahmten Bereich ist die Suche etwa
fünfmal schneller und findet ungefähr doppelt so viele passende Punkte.

### Andere Git-Branches

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

Die Bilder werden aus dem Objektspeicher gelesen, es wird also nichts
ausgecheckt und der Arbeitsbaum bleibt unangetastet. Die genannte Menge ist die
vollständige Menge: ein früher indizierter und jetzt nicht genannter Branch fällt
aus dem Index. `--branches all` nimmt jeden lokalen Branch, `--remote-branches`
fügt die Remote-Tracking-Branches hinzu.

Erneutes Indizieren ist billig. Ein Branch mit unbewegter Spitze wird
übersprungen, ohne einen Baum aufzulisten, und innerhalb eines Branches
entscheidet die Blob-Id — eine Inhaltsadresse und damit eine stärkere Prüfung als
Größe und Änderungszeit bei Dateien.

Zeilen aus einem Branch melden nie einen Dateisystempfad, weil es keinen gibt.
Die normale Ausgabe und `--paths` schreiben Gits eigene `branch:pfad`-Syntax, die
`git show` akzeptiert; `--json` trägt statt `path` die Felder `ref`, `blob` und
`rev`.

Unter Git LFS werden die Objekte aus dem lokalen LFS-Speicher aufgelöst. Fehlende
werden gezählt und einmal gemeldet, mit `git lfs fetch --all` als Abhilfe; von
selbst wird nie etwas heruntergeladen.

### Weiterleiten

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## Die Installation prüfen

```
argus doctor
```

Meldet die Versionen von ONNX Runtime und OpenCV, die verfügbaren
Ausführungsanbieter, und ob sich auf diesem Treiber tatsächlich eine
DirectML-Sitzung erzeugen lässt — was nicht dasselbe ist, wie den Anbieter bloß
aufgelistet zu sehen. `--extract <image>` lässt den Extraktor über eine Datei
laufen und meldet, wie viele Merkmalspunkte gefunden wurden und wie lange es
gedauert hat.

---

## Wo was liegt

```
<Indexordner>/
  index.db              SQLite: Dateien, Hashes, Vorschauen, Deskriptor-Offsets
  features/desc.f16     Deskriptoren, halbe Genauigkeit
  features/kpts.f32     Merkmalspunkte
  features/vocab.bin    visuelles Vokabular
  features/bow.bin      Vorauswahl-Index
```

Den Indexordner zu löschen setzt alles zurück und rührt sonst nichts an. Pfade
in der Datenbank stehen **relativ zum indizierten Ordner**, ein Index bleibt also
gültig, wenn derselbe Speicher woanders eingehängt wird.

Modelle werden in `models/` neben dem Programm gesucht, danach in den
darüberliegenden Verzeichnissen — so funktionieren ein fertiges Paket und ein
Build-Verzeichnis gleichermaßen ohne Konfiguration.

---

## Bauen

```
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF` baut ohne ONNX Runtime und OpenCV; die
Duplikatsuche funktioniert weiter, die Objektsuche nicht.

Der Installationsschritt stellt einen in sich geschlossenen Ordner zusammen —
jede Abhängigkeit neben den Programmen, nichts aus `PATH`. Ein Build in eine
einzelne Datei ist nicht möglich: ONNX Runtime mit DirectML gibt es nur als DLL,
und `DirectML.dll` ist ein Redistributable, das sich überhaupt nicht einlinken
lässt.
