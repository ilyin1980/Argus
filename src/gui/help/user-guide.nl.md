# ImageWorker gebruiken

ImageWorker beantwoordt twee vragen over een map met `png`- en `jpg`-bestanden:

- **Waar komt dit asset voor in mijn schermafbeelding?** — de hoofdtaak.
- **Welke bestanden in deze map zijn duplicaten?** — de bijtaak.

---

## 1. Richt het op een map

**Afbeeldingen** is de map waarin gezocht wordt. **Index in** is de plek waar
ImageWorker zijn eigen gegevens bewaart: de database, de voorbeelden en de
descriptoren.

Standaard komt de index in een verborgen map `.imageworker` binnen de
afbeeldingenmap. Wijzig **Index in** wanneer de afbeeldingen op een
alleen-lezen of netwerkshare staan, wanneer u liever niets toevoegt aan andermans
repository, of wanneer de index op een snellere schijf thuishoort. **Standaard**
zet het weer terug.

Druk op **Indexeren**. De eerste ronde leest elk bestand; latere rondes lezen
alleen bestanden opnieuw waarvan de grootte of wijzigingstijd veranderde, zodat
de index bijhouden weinig kost.

> 4600 afbeeldingen van samen ongeveer 400 MB indexeren duurt ruwweg drie
> minuten, inclusief de neurale descriptoren.

---

## 2. Een asset in een schermafbeelding vinden

Open **Zoeken op voorbeeld** en breng de schermafbeelding binnen:

- druk op **Ctrl+V** om hem uit het klembord te plakken — werkt vanaf beide
  tabbladen;
- sleep een afbeeldingsbestand op het venster;
- of gebruik **Bladeren…**.

**Sleep een kader om het object dat u zoekt.** Dat telt zwaarder dan het lijkt:
op een heel beeld gaat het meeste werk naar de achtergrond. Bij een bibliotheek
van 4600 assets kost een ingekaderd object ongeveer **2,6 seconden tegen 5,6**
voor het hele beeld, en levert het ruwweg **twee keer zoveel overeenkomende
punten**, omdat elk ervan op het object valt in plaats van op het decor. Klik
buiten het kader om het te wissen en weer het hele beeld te doorzoeken.

Druk op **Zoeken**. De beste treffer wordt op de schermafbeelding omlijnd, en
alle resultaten verschijnen eronder, op volgorde.

### De resultaten lezen

Elk resultaat toont een **score** en een **aantal inliers**. Inliers zijn de
gekoppelde punten die het eens zijn over één samenhangende plaatsing — dat is het
bewijs. De score weegt hoe *samenhangend* die koppelingen waren, en daalt als het
er weinig zijn.

**Het resultaat is een gerangschikte lijst, geen ja-of-nee-antwoord.** Een andere
tekening van hetzelfde personage doorstaat de controle ook, omdat het grootste
deel van het beeld werkelijk overeenkomt. Het juiste staat duidelijk hoger: lees
dus de volgorde in plaats van naar één winnaar te zoeken.

### Methode

- **Object in de afbeelding vinden** — het hierboven beschreven neurale zoeken.
  Vereist een index met descriptoren; is het item grijs, zie hieronder.
- **Gelijkenis van de hele afbeelding** — een perceptuele hash die afbeeldingen
  als geheel vergelijkt. Goed om een geschaalde of hergecodeerde kopie van
  *dezelfde afbeelding* te vinden, nutteloos om een klein object in een
  schermafbeelding te vinden.

Staat **Object vinden** uit, dan heeft de index nog geen descriptoren. Maak ze
eenmalig via de opdrachtregel:

```
imageworker index "<afbeeldingenmap>" --db "<indexmap>\index.db" --features
imageworker vocab "<afbeeldingenmap>" --db "<indexmap>\index.db"
```

---

## 3. Duplicaten vinden

Open **Duplicaten**, stel **Max. afstand** in en druk op **Duplicaten zoeken**.

- **Exacte** groepen zijn byte-identieke bestanden. Daarop kunt u gerust
  handelen.
- **Gelijkende** groepen lijken op elkaar voor een perceptuele hash. **Bekijk ze,
  verwijder ze niet blindelings.** De hash beschrijft de compositie, dus één
  personage getekend met meerdere gezichtsuitdrukkingen belandt bij elke bruikbare
  afstand in één groep.

Afstand 0 betekent identieke vingerafdrukken; 4 verdraagt hercodering en schalen.
Boven 8 zijn de groepen alleen nog om na te kijken.

Groepen staan op volgorde van verspilde ruimte, de grootste eerst. Binnen een
groep staat het grootste bestand met de hoogste resolutie bovenaan — meestal
degene die u wilt houden.

---

## 4. Paden eruit halen

Elk resultaat draagt zijn volledige pad:

- de balk onderaan toont het pad van de huidige selectie, en **Kopiëren** kopieert
  alle geselecteerde paden, één per regel;
- het kleine kopieerpictogram naast een bestandsnaam kopieert alleen dat ene;
- rechtsklikken geeft het volledige pad, het pad ten opzichte van de map, of het
  bestand tonen in de verkenner.

---

## 5. Andere git-branches

Ligt de afbeeldingenmap in een git-repository, dan wordt **Branches…**
beschikbaar. Vink de gewenste branches aan en druk op **Indexeren**: hun
afbeeldingen worden rechtstreeks uit de repository gelezen, er wordt dus niets
uitgecheckt en uw werkmap blijft ongemoeid. De branch die u hebt uitgecheckt
staat niet in de lijst — die *is* de werkmap.

De lijst met branches is het volledige beeld van wat de index bevat. Een vinkje
weghalen en opnieuw indexeren verwijdert die branch.

Resultaten uit een branch tonen de branchnaam en melden zich als `branch:pad` in
plaats van als een bestandspad, want zo'n bestand staat niet op schijf.
**Openen** pakt die versie uit naar een tijdelijk bestand zodat u ernaar kunt
kijken; tonen in de verkenner en ermee zoeken gelden alleen voor bestanden in de
werkmap.

Opnieuw indexeren kost weinig: een branch waarvan de tip niet is verschoven wordt
helemaal overgeslagen, en binnen een branch worden alleen bestanden herlezen
waarvan de inhoud echt is veranderd.

> **Git LFS**: de afbeeldingen komen uit uw lokale LFS-opslag. Is een branch nooit
> opgehaald, dan meldt ImageWorker hoeveel objecten ontbreken en dat
> `git lfs fetch --all` ze binnenhaalt — zelf downloadt het niets.

---

## 6. Thema's en taal

**Beeld → Thema** wisselt tijdens het draaien tussen Systeem, Sci-fi, Material en
Bloesem. De keuze wordt onthouden.

**Beeld → Taal** wisselt de interface, eveneens zonder herstart. De lijst bevat
elke taal die deze build meebrengt; **Systeemtaal** volgt het besturingssysteem.
Deze handleiding is ook vertaald, en valt terug op het Engels voor een taal
waarvan de handleiding nog niet klaar is.

---

## Wat het niet kan

- Een perceptuele hash vindt **dezelfde afbeelding**, nooit slechts een
  vergelijkbaar onderwerp.
- Het neurale zoeken vindt assets die **werkelijk aanwezig** zijn in het beeld.
  Het is geen zoeken op omschrijving.
- Een asset dat geen enkel kenmerkpunt oplevert — een egale kleur, een piepklein
  pictogram — kan niet gelokaliseerd worden. Ongeveer 4% van een gebruikelijke
  assetbibliotheek valt in die categorie.
