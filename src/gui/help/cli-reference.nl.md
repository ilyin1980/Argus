# Opdrachtregel en automatisering

Het pakket levert **twee programma's die op dezelfde kernbibliotheek zijn
gebouwd**:

| | |
|---|---|
| `argus-gui` | het venster waarin u dit leest |
| `argus` | zonder venster; alles wat de GUI doet, plus indexeeropties die de GUI niet toont |

Ze delen alle logica, dus een map die door de één is geïndexeerd wordt door de
ander gelezen. Geen van beide is een omhulsel om de ander — beide linken dezelfde
statische kern.

---

## Uitvoerafspraak

Hierop kunt u bouwen bij het scripten; ze is bewust stabiel.

- `--json` schrijft **regelgescheiden JSON naar stdout**, één object per regel.
  Elk resultaat draagt zowel `rel` (ten opzichte van de geïndexeerde map) als
  `path` (absoluut).
- `--paths` schrijft **kale absolute paden**, één per regel, om door te sluizen.
  In `dupes` scheidt een lege regel de groepen. Het impliceert `--quiet`, en het
  met `--json` combineren is een fout in plaats van een stille keuze tussen de
  twee.
- Voortgang, waarschuwingen en fouten gaan altijd naar **stderr**, nooit naar
  stdout.
- Resultaten komen in een vaste volgorde.
- Afsluitcodes: **0** iets gevonden, **1** niets gevonden, **2** een fout.

---

## Opdrachten

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

Elke opdracht neemt `--db <path>` om de index ergens anders te zetten dan in
`<dir>/.argus`.

### Een map klaarmaken voor objectzoeken

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` alleen geeft duplicaatdetectie. `--features` voegt de neurale
descriptoren toe, en `vocab` traint daarover de voorselectie-index; beide zijn
nodig voordat `find` werkt. Draai `vocab` opnieuw na het toevoegen van een grote
partij assets, niet na elk bestand.

### Zoeken

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Geef `--roi x,y,w,h` mee wanneer u weet waar het object zit. Het is verreweg de
doeltreffendste optie: op een ingekaderd gebied is het zoeken ongeveer vijf keer
sneller en vindt het ruwweg twee keer zoveel overeenkomende punten.

### Andere git-branches

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

De afbeeldingen worden uit de objectopslag gelezen, er wordt dus niets uitgecheckt
en de werkmap blijft ongemoeid. De genoemde verzameling is de volledige
verzameling: een branch die eerder is geïndexeerd en nu niet wordt genoemd,
verdwijnt uit de index. `--branches all` neemt elke lokale branch, en
`--remote-branches` voegt de remote-tracking branches toe.

Opnieuw indexeren kost weinig. Een branch met een onveranderde tip wordt
overgeslagen zonder ook maar één boom op te vragen, en binnen een branch beslist
de blob-id: een inhoudsadres, en dus een sterkere controle dan de grootte en
wijzigingstijd die voor bestanden gelden.

Rijen uit een branch melden nooit een pad in het bestandssysteem, want dat is er
niet. De gewone uitvoer en `--paths` schrijven gits eigen `branch:pad`-syntaxis,
die `git show` aanvaardt; `--json` draagt `ref`, `blob` en `rev` in plaats van
`path`.

Onder Git LFS worden de objecten uit de lokale LFS-opslag opgelost. Ontbrekende
worden geteld en één keer gemeld, met `git lfs fetch --all` als oplossing; er
wordt nooit uit zichzelf iets gedownload.

### Doorsluizen

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## De installatie controleren

```
argus doctor
```

Meldt de versies van ONNX Runtime en OpenCV, de beschikbare uitvoeraanbieders, en
of er op dit stuurprogramma daadwerkelijk een DirectML-sessie te maken is — wat
iets anders is dan de aanbieder enkel in de lijst zien staan. `--extract <image>`
laat de extractor over één bestand lopen en meldt hoeveel kenmerkpunten hij vond
en hoe lang het duurde.

---

## Waar alles staat

```
<indexmap>/
  index.db              SQLite: bestanden, hashes, voorbeelden, descriptor-offsets
  features/desc.f16     descriptoren, halve precisie
  features/kpts.f32     kenmerkpunten
  features/vocab.bin    visueel vocabulaire
  features/bow.bin      voorselectie-index
```

De indexmap verwijderen zet alles terug en raakt verder niets aan. Paden in de
database staan **ten opzichte van de geïndexeerde map**, zodat een index geldig
blijft wanneer dezelfde opslag elders wordt aangekoppeld.

Modellen worden gezocht in `models/` naast het programma, daarna in de mappen
erboven — zo werken zowel een ingepakte map als een build-boom zonder verdere
instellingen.

---

## Bouwen

```
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF` bouwt zonder ONNX Runtime en OpenCV; het zoeken
naar duplicaten blijft werken, het objectzoeken niet.

De installatiestap zet een op zichzelf staande map in elkaar — elke afhankelijkheid
naast de programma's, niets uit `PATH`. Eén enkel bestand is onmogelijk: ONNX
Runtime met DirectML bestaat alleen als DLL, en `DirectML.dll` is een
redistributable die zich helemaal niet laat meelinken.
