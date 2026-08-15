# ImageWorker verwenden

ImageWorker beantwortet zwei Fragen über einen Ordner mit `png`- und
`jpg`-Dateien:

- **Wo taucht dieses Asset in meinem Screenshot auf?** — die Hauptaufgabe.
- **Welche Dateien in diesem Ordner sind Duplikate?** — die Nebenaufgabe.

---

## 1. Auf einen Ordner richten

**Bilder** ist der zu durchsuchende Ordner. **Index in** ist der Ort, an dem
ImageWorker seine eigenen Daten ablegt: die Datenbank, die Vorschaubilder und
die Deskriptoren.

Standardmäßig landet der Index in einem versteckten Ordner `.imageworker`
innerhalb des Bilderordners. Ändern Sie **Index in**, wenn die Bilder auf einer
schreibgeschützten oder Netzwerkfreigabe liegen, wenn Sie einem fremden
Repository nichts hinzufügen möchten, oder wenn der Index auf eine schnellere
Platte gehört. **Standard** stellt den ursprünglichen Ort wieder her.

Drücken Sie **Indizieren**. Der erste Lauf liest jede Datei; spätere Läufe lesen
nur Dateien erneut, deren Größe oder Änderungszeit sich geändert hat, sodass ein
aktueller Index billig zu halten ist.

> Das Indizieren von 4600 Bildern mit etwa 400 MB dauert ungefähr drei Minuten,
> die neuronalen Deskriptoren eingeschlossen.

---

## 2. Ein Asset in einem Screenshot finden

Öffnen Sie **Suche nach Vorlage** und bringen Sie den Screenshot hinein:

- mit **Strg+V** aus der Zwischenablage einfügen — funktioniert von beiden
  Registerkarten aus;
- eine Bilddatei auf das Fenster ziehen;
- oder **Durchsuchen…** verwenden.

**Ziehen Sie einen Rahmen um das gesuchte Objekt.** Das ist wichtiger, als es
aussieht: auf einem ganzen Bild steckt der Großteil des Aufwands im Hintergrund.
Bei einer Bibliothek mit 4600 Assets braucht ein eingerahmtes Objekt etwa
**2,6 statt 5,6 Sekunden** und findet ungefähr **doppelt so viele passende
Punkte**, weil jeder davon auf dem Objekt liegt statt auf der Kulisse. Klicken
Sie außerhalb des Rahmens, um ihn zu löschen und wieder das ganze Bild zu
durchsuchen.

Drücken Sie **Suchen**. Der beste Treffer wird im Screenshot umrandet, und alle
Ergebnisse erscheinen darunter in einer Rangfolge.

### Die Ergebnisse lesen

Jedes Ergebnis zeigt eine **Bewertung** und eine **Inlier-Zahl**. Inlier sind
die zugeordneten Punkte, die sich auf eine einzige stimmige Platzierung einigen —
das ist der Beleg. Die Bewertung wiegt, wie *stimmig* diese Zuordnungen waren,
und fällt ab, wenn es nur wenige gibt.

**Das Ergebnis ist eine Rangliste, keine Ja-Nein-Antwort.** Eine andere
Zeichnung derselben Figur besteht die Prüfung ebenfalls, weil der größte Teil des
Bildes tatsächlich übereinstimmt. Das richtige Asset steht deutlich darüber:
lesen Sie also die Reihenfolge, statt einen einzigen Sieger zu suchen.

### Methode

- **Objekt im Bild finden** — die oben beschriebene neuronale Suche. Braucht
  einen Index mit Deskriptoren; ist der Eintrag ausgegraut, siehe unten.
- **Gesamtbildähnlichkeit** — ein Wahrnehmungshash, der Bilder als Ganzes
  vergleicht. Gut, um eine skalierte oder neu kodierte Kopie *desselben Bildes*
  zu finden, nutzlos für ein kleines Objekt in einem Screenshot.

Ist **Objekt im Bild finden** deaktiviert, hat der Index noch keine Deskriptoren.
Erzeugen Sie sie einmalig auf der Befehlszeile:

```
imageworker index "<Bilderordner>" --db "<Indexordner>\index.db" --features
imageworker vocab "<Bilderordner>" --db "<Indexordner>\index.db"
```

---

## 3. Duplikate finden

Öffnen Sie **Duplikate**, stellen Sie **Max. Abstand** ein und drücken Sie
**Duplikate suchen**.

- **Exakte** Gruppen sind bytegleiche Dateien. Mit denen können Sie bedenkenlos
  arbeiten.
- **Ähnliche** Gruppen sehen für einen Wahrnehmungshash gleich aus. **Sehen Sie
  sie durch, löschen Sie sie nicht blind.** Der Hash beschreibt die
  Bildaufteilung, deshalb landet eine Figur mit mehreren Gesichtsausdrücken bei
  jedem brauchbaren Abstand in einer Gruppe.

Abstand 0 heißt identische Fingerabdrücke; 4 verträgt Neukodierung und
Skalierung. Über 8 dienen die Gruppen nur der Durchsicht.

Gruppen sind nach verschwendetem Platz sortiert, die größte zuerst. Innerhalb
einer Gruppe steht die größte Datei mit der höchsten Auflösung oben — meist die,
die man behalten will.

---

## 4. Pfade herausbekommen

Jedes Ergebnis trägt seinen vollständigen Pfad:

- die Leiste unten zeigt den Pfad der aktuellen Auswahl, und **Kopieren** kopiert
  alle ausgewählten Pfade, einen pro Zeile;
- das kleine Kopiersymbol neben einem Dateinamen kopiert nur diesen einen;
- die rechte Maustaste bietet den vollständigen Pfad, den Pfad relativ zum
  Ordner, oder das Anzeigen der Datei im Explorer.

---

## 5. Designs und Sprache

**Ansicht → Design** wechselt im laufenden Betrieb zwischen System, Sci-Fi,
Material und Blüte. Die Wahl wird gemerkt.

**Ansicht → Sprache** wechselt die Oberfläche, ebenfalls ohne Neustart. Die
Liste enthält jede Sprache, die diese Fassung mitbringt; **Systemsprache** folgt
dem Betriebssystem. Dieses Handbuch ist ebenfalls übersetzt und fällt für eine
Sprache, deren Handbuch noch nicht fertig ist, auf Englisch zurück.

---

## Was es nicht kann

- Ein Wahrnehmungshash findet **dasselbe Bild**, niemals bloß ein ähnliches
  Motiv.
- Die neuronale Suche findet Assets, die **tatsächlich vorhanden** sind. Sie ist
  keine Suche nach Beschreibung.
- Ein Asset, das keine Merkmalspunkte hergibt — eine einfarbige Fläche, ein
  winziges Symbol — lässt sich nicht lokalisieren. Rund 4 % einer typischen
  Asset-Bibliothek fallen in diese Kategorie.
