<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="nl">
<context>
    <name>BranchDialog</name>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="21" />
        <source>Branches to index</source>
        <translation>Te indexeren branches</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="27" />
        <source>Files are read straight out of the repository, so nothing is checked out and the working tree is never touched. The branch you have checked out is not listed: it is the working tree, which the index already covers.</source>
        <translation>De bestanden worden rechtstreeks uit de repository gelezen, er wordt dus niets uitgecheckt en de werkmap blijft ongemoeid. De branch die u hebt uitgecheckt staat niet in de lijst: dat is de werkmap, die de index al dekt.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="39" />
        <source>Include remote-tracking branches</source>
        <translation>Ook remote-tracking branches tonen</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="50" />
        <source>Select none</source>
        <translation>Niets selecteren</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="87" />
        <source>This repository has no branches.</source>
        <translation>Deze repository heeft geen branches.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="88" />
        <source>Cannot list branches: %1</source>
        <translation>Kan de branches niet opvragen: %1</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/BranchDialog.cpp" line="100" />
        <source>%n branch(es) available.</source>
        <translation><numerusform>%n branch beschikbaar.</numerusform><numerusform>%n branches beschikbaar.</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="103" />
        <source>Indexed earlier but gone from the repository: %1. Pressing OK drops them from the index.</source>
        <translation>Eerder geïndexeerd maar verdwenen uit de repository: %1. Met OK verdwijnen ze uit de index.</translation>
    </message>
</context>
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="18" />
        <location filename="../src/gui/HelpDialog.cpp" line="85" />
        <source>Argus — Help</source>
        <translation>Argus — Help</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="57" />
        <source>Using Argus</source>
        <translation>Argus gebruiken</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="58" />
        <source>Command line</source>
        <translation>Opdrachtregel</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="75" />
        <source>Could not load %1.</source>
        <translation>%1 kon niet worden geladen.</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="14" />
        <source>Argus</source>
        <translation>Argus</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="54" />
        <source>Images:</source>
        <translation>Afbeeldingen:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>Map met png-/jpg-bestanden</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="301" />
        <source>Browse…</source>
        <translation>Bladeren…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="75" />
        <source>&amp;Index</source>
        <translation>&amp;Indexeren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="91" />
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="98" />
        <source>Index in:</source>
        <translation>Index in:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .argus inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>Map met de indexdatabase, voorbeelden en descriptoren.
Standaard .argus in de afbeeldingenmap; kies een andere om een
alleen-lezen of netwerkshare ongemoeid te laten, of om de index op een
snellere schijf te zetten.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>Zet de index terug in de afbeeldingenmap.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>Standaard</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let Argus locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>Bouw ook de neurale descriptoren waarmee Argus een asset in een
schermafbeelding kan terugvinden. Verdubbelt ruwweg de indexeertijd en
kost ongeveer 40 KB per afbeelding; zonder deze werken alleen duplicaat-
detectie en gelijkenis van de hele afbeelding.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>Gegevens voor objectzoeken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="151" />
        <source>Pick which git branches to index alongside the working tree.
Enabled only when the image folder is inside a git repository.</source>
        <translation>Kies welke git-branches naast de werkmap worden geïndexeerd.
Alleen beschikbaar wanneer de afbeeldingenmap in een git-repository ligt.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="155" />
        <source>Branches…</source>
        <translation>Branches…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="182" />
        <source>Duplicates</source>
        <translation>Duplicaten</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="190" />
        <location filename="../src/gui/MainWindow.ui" line="359" />
        <source>Max distance:</source>
        <translation>Max. afstand:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="197" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>Hamming-straal op de 64-bits perceptuele hash.
0 = identieke vingerafdrukken, 4 = bestand tegen hercodering en schalen.
Boven 8 zijn de treffers om te bekijken, niet om te verwijderen: de hash beschrijft
de compositie, dus één personage met meerdere gezichtsuitdrukkingen belandt in dezelfde groep.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="213" />
        <source>Exact</source>
        <translation>Exact</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="223" />
        <source>Near</source>
        <translation>Gelijkend</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="233" />
        <source>&amp;Find duplicates</source>
        <translation>Duplicaten &amp;zoeken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="272" />
        <source>Find by example</source>
        <translation>Zoeken op voorbeeld</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>Referentieafbeelding: kies een bestand, sleep het hierheen of plak met Ctrl+V</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="308" />
        <source>Paste</source>
        <translation>Plakken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="317" />
        <location filename="../src/gui/MainWindow.cpp" line="428" />
        <location filename="../src/gui/MainWindow.cpp" line="1029" />
        <source>No reference image yet.</source>
        <translation>Nog geen referentieafbeelding.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="329" />
        <source>Method:</source>
        <translation>Methode:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="339" />
        <source>Top:</source>
        <translation>Beste:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="366" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>Gecombineerde pHash- + dHash-afstand, 0..128.
De perceptuele hash vindt dezelfde afbeelding, niet zomaar een gelijkend onderwerp.
Onder ongeveer 16 zijn de treffers dezelfde afbeelding, hergecodeerd of geschaald.
Boven 30 zijn ze in feite willekeurig, wat de score ook suggereert.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="382" />
        <source>&amp;Search</source>
        <translation>&amp;Zoeken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="430" />
        <source>Path:</source>
        <translation>Pad:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="440" />
        <source>Select a result to see its full path</source>
        <translation>Selecteer een resultaat om het volledige pad te zien</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="457" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>Kopieer het volledige pad van elk geselecteerd resultaat, één per regel.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="460" />
        <source>&amp;Copy</source>
        <translation>&amp;Kopiëren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="128" />
        <location filename="../src/gui/MainWindow.cpp" line="645" />
        <source>Choose a folder to begin.</source>
        <translation>Kies een map om te beginnen.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="133" />
        <source>Paste screenshot</source>
        <translation>Schermafbeelding plakken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="234" />
        <location filename="../src/gui/MainWindow.cpp" line="430" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>De hele afbeelding wordt doorzocht. Sleep een kader om één object te zoeken.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="239" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>Het geselecteerde gebied %1 x %2 wordt doorzocht — sneller en nauwkeuriger dan het hele beeld.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>Een schermafbeelding uit het klembord plakken (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="412" />
        <source>Locate object in the image</source>
        <translation>Object in de afbeelding vinden</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="254" />
        <location filename="../src/gui/MainWindow.cpp" line="413" />
        <source>Whole-image similarity</source>
        <translation>Gelijkenis van de hele afbeelding</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="257" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>Neurale lokale kenmerken: vinden een asset die werkelijk in de afbeelding
staat — ook klein, geschaald of tussen rommel — en tonen waar.
Vereist een index met descriptoren en een getraind vocabulaire.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="263" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>Perceptuele hash: vergelijkt de afbeelding als geheel.
Goed voor geschaalde of hergecodeerde kopieën van dezelfde afbeelding,
nutteloos om een klein object in een schermafbeelding te vinden.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="294" />
        <source>&amp;View</source>
        <translation>Beel&amp;d</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="295" />
        <source>&amp;Theme</source>
        <translation>&amp;Thema</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="296" />
        <source>&amp;Language</source>
        <translation>&amp;Taal</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="302" />
        <source>&amp;Help</source>
        <translation>&amp;Help</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="304" />
        <source>&amp;User guide</source>
        <translation>&amp;Handleiding</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="309" />
        <source>&amp;Command line and automation</source>
        <translation>&amp;Opdrachtregel en automatisering</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="314" />
        <source>&amp;About</source>
        <translation>&amp;Over</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="316" />
        <source>About Argus</source>
        <translation>Over Argus</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="317" />
        <source>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;argus-gui&lt;/b&gt; and the headless &lt;b&gt;argus&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Vindt welke afbeeldings-assets in een schermafbeelding voorkomen en waar; en groepeert duplicaten in een map.&lt;/p&gt;&lt;p&gt;Wordt geleverd als twee programma's op één gedeelde bibliotheek: &lt;b&gt;argus-gui&lt;/b&gt; en het opdrachtregelprogramma &lt;b&gt;argus&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Druk op &lt;b&gt;F1&lt;/b&gt; voor de handleiding.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="358" />
        <source>Theme: %1</source>
        <translation>Thema: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="377" />
        <source>System language</source>
        <translation>Systeemtaal</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="388" />
        <source>No translation for %1 in this build.</source>
        <translation>Deze build bevat geen vertaling naar %1.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="391" />
        <source>Language: %1</source>
        <translation>Taal: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="471" />
        <source>Copied %1</source>
        <translation>%1 gekopieerd</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="501" />
        <source>+%n more</source>
        <translation>
            <numerusform>+%n meer</numerusform>
            <numerusform>+%n meer</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="527" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation>
            <numerusform>%n volledig pad naar het klembord gekopieerd.</numerusform>
            <numerusform>%n volledige paden naar het klembord gekopieerd.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="528" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation>
            <numerusform>%n relatief pad naar het klembord gekopieerd.</numerusform>
            <numerusform>%n relatieve paden naar het klembord gekopieerd.</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="557" />
        <source>Choose where to keep the index</source>
        <translation>Kies waar de index moet staan</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="588" />
        <source>Choose a folder</source>
        <translation>Kies een map</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="651" />
        <source>Choose where the index should be kept.</source>
        <translation>Kies waar de index moet staan.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="656" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>Geen index in %1 — druk op „Indexeren” om er een te maken.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="664" />
        <source>Cannot open index: %1</source>
        <translation>Index kan niet worden geopend: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="678" />
        <source>%1 indexed files, %2%3</source>
        <translation>%1 geïndexeerde bestanden, %2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="683" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — geen descriptoren, objectzoeken niet beschikbaar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="697" />
        <source>Choose an existing folder first.</source>
        <translation>Kies eerst een bestaande map.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="710" />
        <source>Cannot create the index directory:
%1</source>
        <translation>Indexmap kan niet worden aangemaakt:
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="732" />
        <source>Indexing</source>
        <translation>Indexeren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="769" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>Geïndexeerd %1, ongewijzigd %2, mislukt %3, verwijderd %4 — %5 s%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="764" />
        <location filename="../src/gui/MainWindow.cpp" line="775" />
        <location filename="../src/gui/MainWindow.cpp" line="977" />
        <source> (cancelled)</source>
        <translation> (geannuleerd)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Only the working tree will be indexed.</source>
        <translation>Alleen de werkmap wordt geïndexeerd.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="623" />
        <source>Branches to index: %1. Press Index to read them.</source>
        <translation>Te indexeren branches: %1. Druk op „Indexeren” om ze te lezen.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="757" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6; branches: %7 read, %8 unchanged</source>
        <translation>Geïndexeerd %1, ongewijzigd %2, mislukt %3, verwijderd %4 — %5 s%6; branches: %7 gelezen, %8 ongewijzigd</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>Training the visual vocabulary</source>
        <translation>Het visuele vocabulaire wordt getraind</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="844" />
        <source>No descriptors to train on.</source>
        <translation>Geen descriptoren om op te trainen.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="854" />
        <source>Vocabulary failed: %1</source>
        <translation>Vocabulaire mislukt: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="864" />
        <source>Shortlist index failed: %1</source>
        <translation>Voorselectie-index mislukt: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="875" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>Klaar: %1 afbeeldingen doorzoekbaar op object, vocabulaire van %2 woorden.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="889" />
        <source>Cancelling…</source>
        <translation>Bezig met annuleren…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="902" />
        <location filename="../src/gui/MainWindow.cpp" line="1113" />
        <location filename="../src/gui/MainWindow.cpp" line="1338" />
        <source>Build an index for this folder first.</source>
        <translation>Maak eerst een index voor deze map.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="913" />
        <source>Matching</source>
        <translation>Vergelijken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="960" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 bestanden  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="963" />
        <source>exact</source>
        <translation>exact</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="964" />
        <source>near d≤%1</source>
        <translation>gelijkend d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="969" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 groepen, %2 terug te winnen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="973" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — %1 te grote hash-groepen overgeslagen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1000" />
        <source>Choose a reference image</source>
        <translation>Kies een referentieafbeelding</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1001" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>Afbeeldingen (*.png *.jpg *.jpeg);;Alle bestanden (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1017" />
        <source>Cannot read %1: %2</source>
        <translation>%1 kan niet worden gelezen: %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1047" />
        <source>The clipboard holds no image.</source>
        <translation>Het klembord bevat geen afbeelding.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1051" />
        <source>(clipboard, %1 x %2)</source>
        <translation>(klembord, %1 x %2)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1055" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>Schermafbeelding %1 x %2 geplakt — sleep een kader om het object.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1082" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>Deze index heeft nog geen descriptoren en geen vocabulaire. Maak ze zo:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1097" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>Deze index bevat geen gegevens voor objectzoeken — druk op „Indexeren” met „Gegevens voor objectzoeken” aangevinkt. Tot dan is alleen gelijkenis van de hele afbeelding beschikbaar, en die kan geen object binnen een afbeelding vinden.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1119" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</source>
        <translation>Deze index heeft nog geen descriptoren van lokale kenmerken, dus een object kan niet in een afbeelding worden gevonden.

Maak ze eerst:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1132" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>Plak eerst een schermafbeelding met Ctrl+V, sleep een afbeelding hierheen of kies een bestand.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1158" />
        <source>Locating</source>
        <translation>Object zoeken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1217" />
        <source>Searching</source>
        <translation>Zoeken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1255" />
        <source>distance</source>
        <translation>afstand</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1263" />
        <source>the same picture</source>
        <translation>dezelfde afbeelding</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1265" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>waarschijnlijk dezelfde afbeelding, hergecodeerd of geschaald</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1267" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>niets overtuigends — deze liggen ver uit elkaar, wat de scores ook suggereren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1270" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>%1 treffers uit %2 geïndexeerde bestanden, kleinste afstand %3: %4. Gelijkenis van de hele afbeelding, geen objectlocatie.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1294" />
        <source>inliers</source>
        <translation>inliers</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1299" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>Niets gevonden. Kader het object in, of controleer of de asset wel geïndexeerd is.</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1315" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation>
            <numerusform>%n asset gevonden; beste: %1 (%2 inliers, %3% consistent)</numerusform>
            <numerusform>%n assets gevonden; beste: %1 (%2 inliers, %3% consistent)</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1351" />
        <source>Looking for copies of %1…</source>
        <translation>Zoeken naar kopieën van %1…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1367" />
        <source>%1 lives in branch %2, and this folder is no longer a git repository.</source>
        <translation>%1 staat in branch %2, en deze map is geen git-repository meer.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1376" />
        <source>Cannot read %1 from branch %2: %3</source>
        <translation>Kan %1 niet uit branch %2 lezen: %3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1394" />
        <source>Cannot write a temporary copy of %1.</source>
        <translation>Kan geen tijdelijke kopie van %1 schrijven.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1402" />
        <source>Opened %1 from branch %2.</source>
        <translation>%1 geopend uit branch %2.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1430" />
        <source>Open</source>
        <translation>Openen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1435" />
        <source>Reveal in file manager</source>
        <translation>Tonen in bestandsbeheer</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1441" />
        <source>Find duplicates of this image</source>
        <translation>Duplicaten van deze afbeelding zoeken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1443" />
        <source>Use as reference image</source>
        <translation>Als referentieafbeelding gebruiken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1455" />
        <source>Only for files in the working tree</source>
        <translation>Alleen voor bestanden in de werkmap</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1460" />
        <source>Copy full path</source>
        <translation>Volledig pad kopiëren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1461" />
        <source>Copy path relative to the root</source>
        <translation>Pad kopiëren ten opzichte van de hoofdmap</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>Ctrl+V plakt een schermafbeelding,
of sleep hier een afbeelding heen</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="159" />
        <source>distance %1</source>
        <translation>afstand %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>scanning</source>
        <translation>scannen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>indexing</source>
        <translation>indexeren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>descriptors</source>
        <translation>descriptoren</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>pruning</source>
        <translation>opruimen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>loading</source>
        <translation>laden</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="95" />
        <source>matching</source>
        <translation>vergelijken</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="96" />
        <source>collecting</source>
        <translation>verzamelen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="97" />
        <source>verifying</source>
        <translation>verifiëren</translation>
    </message>
</context>
<context>
    <name>theme</name>
    <message>
        <location filename="../src/gui/Theme.cpp" line="228" />
        <source>System</source>
        <translation>Systeem</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="229" />
        <source>Sci-fi</source>
        <translation>Sci-fi</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="230" />
        <source>Material</source>
        <translation>Material</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="231" />
        <source>Blossom</source>
        <translation>Bloesem</translation>
    </message>
</context>
</TS>