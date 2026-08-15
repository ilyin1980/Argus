# Jak używać ImageWorkera

ImageWorker odpowiada na dwa pytania o katalog z plikami `png` i `jpg`:

- **Gdzie ten zasób występuje na moim zrzucie ekranu?** — zadanie główne.
- **Które pliki w tym katalogu są duplikatami?** — zadanie poboczne.

---

## 1. Wskaż katalog

**Obrazy** to katalog, w którym szukamy. **Indeks w** to miejsce, gdzie
ImageWorker trzyma własne dane: bazę, podglądy i deskryptory.

Domyślnie indeks trafia do ukrytego katalogu `.imageworker` wewnątrz katalogu z
obrazami. Zmień **Indeks w**, gdy obrazy leżą na zasobie sieciowym albo tylko do
odczytu, gdy wolisz niczego nie dokładać do cudzego repozytorium, albo gdy
indeksowi lepiej na szybszym dysku. **Domyślnie** przywraca poprzednie miejsce.

Naciśnij **Indeksuj**. Pierwszy przebieg czyta każdy plik; kolejne czytają
ponownie tylko te, którym zmienił się rozmiar lub czas modyfikacji, więc
utrzymanie aktualnego indeksu jest tanie.

> Zindeksowanie 4600 obrazów o objętości około 400 MB zajmuje mniej więcej trzy
> minuty, razem z deskryptorami neuronowymi.

---

## 2. Znajdź zasób na zrzucie ekranu

Otwórz **Szukaj według wzoru** i wprowadź zrzut:

- naciśnij **Ctrl+V**, aby wkleić go ze schowka — działa z obu zakładek;
- przeciągnij plik obrazu na okno;
- albo użyj przycisku **Przeglądaj…**.

**Zaznacz ramką obiekt, którego szukasz.** To ważniejsze, niż wygląda: na pełnej
klatce wyszukiwanie zużywa większość wysiłku na tło. Przy bibliotece 4600 zasobów
zaznaczony obiekt zajmuje około **2,6 sekundy wobec 5,6** dla całej klatki i
znajduje mniej więcej **dwa razy więcej pasujących punktów**, bo każdy z nich
trafia w obiekt, a nie w dekoracje. Kliknij poza ramką, aby ją usunąć i znów
szukać na całej klatce.

Naciśnij **Szukaj**. Najlepsze trafienie zostanie obrysowane na zrzucie, a
wszystkie wyniki pojawią się poniżej, uszeregowane.

### Jak czytać wyniki

Każdy wynik pokazuje **wynik punktowy** i **liczbę inlierów**. Inliery to
dopasowane punkty, które zgadzają się co do jednego spójnego umiejscowienia — i
to jest dowód. Punktacja waży, jak *spójne* były te dopasowania, i spada, gdy
jest ich mało.

**Wynik to uszeregowana lista, a nie odpowiedź tak/nie.** Inny rysunek tej samej
postaci również przejdzie weryfikację, bo większość obrazu naprawdę się zgadza.
Właściwy stoi wyraźnie wyżej, więc czytaj kolejność, zamiast szukać jednego
zwycięzcy.

### Metoda

- **Znajdź obiekt na obrazie** — opisane wyżej wyszukiwanie neuronowe. Wymaga
  indeksu z deskryptorami; jeśli pozycja jest wyszarzona, patrz niżej.
- **Podobieństwo całego obrazu** — skrót percepcyjny porównujący obrazy jako
  całość. Dobry do znalezienia przeskalowanej lub przekodowanej kopii *tego
  samego obrazu*, bezużyteczny do szukania małego obiektu na zrzucie ekranu.

Jeśli **Znajdź obiekt** jest wyłączone, indeks nie ma jeszcze deskryptorów.
Zbuduj je raz z wiersza poleceń:

```
imageworker index "<katalog z obrazami>" --db "<katalog indeksu>\index.db" --features
imageworker vocab "<katalog z obrazami>" --db "<katalog indeksu>\index.db"
```

---

## 3. Znajdź duplikaty

Otwórz **Duplikaty**, ustaw **Maks. odległość** i naciśnij **Znajdź duplikaty**.

- Grupy **dokładne** to pliki identyczne bajt po bajcie. Na nich można działać
  śmiało.
- Grupy **zbliżone** wyglądają tak samo dla skrótu percepcyjnego. **Przejrzyj je,
  nie usuwaj na ślepo.** Skrót opisuje kompozycję, więc jedna postać narysowana z
  kilkoma minami trafia do jednej grupy przy każdej użytecznej odległości.

Odległość 0 oznacza identyczne odciski; 4 toleruje przekodowanie i
przeskalowanie. Powyżej 8 grupy nadają się już tylko do przejrzenia.

Grupy są uszeregowane według marnowanego miejsca, od największych. Wewnątrz grupy
pierwszy jest plik największy i o najwyższej rozdzielczości — zwykle ten, który
warto zachować.

---

## 4. Jak wyciągnąć ścieżki

Każdy wynik niesie pełną ścieżkę:

- pasek na dole pokazuje ścieżkę bieżącego zaznaczenia, a **Kopiuj** kopiuje
  wszystkie zaznaczone ścieżki, po jednej w wierszu;
- mała ikonka kopiowania obok nazwy pliku kopiuje tylko tę jedną;
- prawy przycisk myszy daje pełną ścieżkę, ścieżkę względem katalogu albo pokazuje
  plik w menedżerze plików.

---

## 5. Motywy i język

**Widok → Motyw** przełącza między Systemowym, Sci-fi, Material i Kwiatowym w
trakcie działania programu. Wybór jest zapamiętywany.

**Widok → Język** przełącza interfejs, również bez restartu. Lista zawiera
wszystkie języki, które niesie ta wersja; **Język systemu** podąża za systemem
operacyjnym. Ten podręcznik też jest przetłumaczony, a dla języka, którego
podręcznik nie jest jeszcze gotowy, wyświetla się po angielsku.

---

## Czego nie potrafi

- Skrót percepcyjny znajduje **ten sam obraz**, nigdy po prostu podobny temat.
- Wyszukiwanie neuronowe znajduje zasoby **rzeczywiście obecne** na obrazie. To
  nie jest wyszukiwanie po opisie.
- Zasobu, który nie dał żadnego punktu kluczowego — jednolita plama koloru,
  malutka ikona — nie da się zlokalizować. Około 4% typowej biblioteki zasobów
  należy do tej kategorii.
