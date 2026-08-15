# Wiersz poleceń i automatyzacja

Pakiet zawiera **dwa programy zbudowane na tej samej bibliotece rdzeniowej**:

| | |
|---|---|
| `imageworker-gui` | okno, w którym to czytasz |
| `imageworker` | bez interfejsu; wszystko to samo co GUI, plus opcje indeksowania, których GUI nie udostępnia |

Dzielą całą logikę, więc katalog zindeksowany przez jeden odczyta drugi. Żaden
nie jest nakładką na drugi — oba linkują ten sam statyczny rdzeń.

---

## Kontrakt wyjścia

Można na nim polegać w skryptach; jest celowo stabilny.

- `--json` wypisuje **JSON rozdzielany znakami nowej linii na stdout**, po jednym
  obiekcie w wierszu. Każdy wynik niesie zarówno `rel` (względem zindeksowanego
  katalogu), jak i `path` (bezwzględną).
- `--paths` wypisuje **same ścieżki bezwzględne**, po jednej w wierszu, do
  potoku. W `dupes` pusty wiersz rozdziela grupy. Opcja włącza `--quiet`, a
  łączenie jej z `--json` jest błędem, a nie cichym wyborem jednej z nich.
- Postęp, ostrzeżenia i błędy zawsze idą na **stderr**, nigdy na stdout.
- Wyniki wychodzą w deterministycznej kolejności.
- Kody wyjścia: **0** coś znaleziono, **1** nic nie znaleziono, **2** błąd.

---

## Polecenia

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

Każde polecenie przyjmuje `--db <path>`, aby umieścić indeks gdzie indziej niż w
`<dir>/.imageworker`.

### Przygotowanie katalogu do wyszukiwania obiektów

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

Samo `index` daje wykrywanie duplikatów. `--features` dodaje deskryptory
neuronowe, a `vocab` trenuje na nich indeks wstępnego wyboru; oba kroki są
potrzebne, zanim `find` zacznie działać. Uruchamiaj `vocab` ponownie po dodaniu
dużej partii zasobów, a nie po każdym pliku.

### Wyszukiwanie

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Podaj `--roi x,y,w,h`, gdy wiesz, gdzie jest obiekt. To najskuteczniejsza opcja
ze wszystkich: na zaznaczonym obszarze wyszukiwanie jest około pięć razy szybsze
i znajduje mniej więcej dwa razy więcej pasujących punktów.

### Inne gałęzie git

```
imageworker index D:/game/Assets --branches all
imageworker index D:/game/Assets --branches release/1.4,feature/new-ui
```

Obrazy są czytane z magazynu obiektów, więc nic nie jest wyewidencjonowywane, a
katalog roboczy pozostaje nietknięty. Podany zbiór jest zbiorem pełnym: gałąź
zindeksowana wcześniej i teraz niewymieniona wypada z indeksu. `--branches all`
bierze wszystkie gałęzie lokalne, a `--remote-branches` dokłada zdalne.

Ponowne indeksowanie jest tanie. Gałąź z nieruszonym czubkiem jest pomijana bez
wypisywania drzewa, a wewnątrz gałęzi rozstrzyga identyfikator blobu — adres
treści, czyli mocniejsze sprawdzenie niż rozmiar i czas modyfikacji używane dla
plików.

Wiersze pochodzące z gałęzi nigdy nie podają ścieżki w systemie plików, bo takiej
nie ma. Zwykłe wyjście i `--paths` wypisują własną składnię git `gałąź:ścieżka`,
którą przyjmuje `git show`; `--json` niesie `ref`, `blob` i `rev` zamiast `path`.

Przy Git LFS obiekty są rozwiązywane z lokalnego magazynu LFS. Brakujące są
liczone i zgłaszane raz, z `git lfs fetch --all` jako lekarstwem; nic nie jest
pobierane z własnej inicjatywy.

### Potoki

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## Sprawdzenie instalacji

```
imageworker doctor
```

Podaje wersje ONNX Runtime i OpenCV, dostępnych dostawców wykonania oraz to, czy
na tym sterowniku da się faktycznie utworzyć sesję DirectML — a to nie to samo,
co samo wymienienie dostawcy na liście. `--extract <image>` przepuszcza ekstraktor
przez jeden plik i podaje, ile punktów kluczowych znalazł i ile to trwało.

---

## Gdzie co leży

```
<katalog indeksu>/
  index.db              SQLite: pliki, skróty, podglądy, przesunięcia deskryptorów
  features/desc.f16     deskryptory, połowiczna precyzja
  features/kpts.f32     punkty kluczowe
  features/vocab.bin    słownik wizualny
  features/bow.bin      indeks wstępnego wyboru
```

Usunięcie katalogu indeksu resetuje wszystko i nie rusza niczego innego. Ścieżki
w bazie są zapisane **względem zindeksowanego katalogu**, więc indeks pozostaje
ważny, gdy ten sam nośnik zostanie zamontowany gdzie indziej.

Modeli szuka się w `models/` obok programu, a potem w katalogach nadrzędnych — tak
że i spakowany katalog, i drzewo kompilacji działają bez konfiguracji.

---

## Kompilacja

```
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF` kompiluje bez ONNX Runtime i OpenCV; wyszukiwanie
duplikatów nadal działa, wyszukiwanie obiektów nie.

Krok instalacji składa samowystarczalny katalog — każda zależność obok programów,
nic nie jest brane z `PATH`. Kompilacja do jednego pliku nie jest możliwa: ONNX
Runtime z DirectML występuje wyłącznie jako DLL, a `DirectML.dll` to komponent
redystrybucyjny, którego w ogóle nie da się wlinkować.
