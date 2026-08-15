<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr">
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="13" />
        <source>ImageWorker — Help</source>
        <translation>ImageWorker — Aide</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="25" />
        <source>Using ImageWorker</source>
        <translation>Utiliser ImageWorker</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="26" />
        <source>Command line</source>
        <translation>Ligne de commande</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="42" />
        <source>Could not load %1.</source>
        <translation>Impossible de charger %1.</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="14" />
        <source>ImageWorker</source>
        <translation>ImageWorker</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="54" />
        <source>Images:</source>
        <translation>Images :</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>Dossier contenant des fichiers png / jpg</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="287" />
        <source>Browse…</source>
        <translation>Parcourir…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="75" />
        <source>&amp;Index</source>
        <translation>&amp;Indexer</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="91" />
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="98" />
        <source>Index in:</source>
        <translation>Index dans :</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .imageworker inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>Dossier contenant la base de l'index, les aperçus et les descripteurs.
Par défaut .imageworker dans le dossier d'images ; choisissez-en un autre
pour ne pas toucher à un partage réseau ou en lecture seule, ou pour
placer l'index sur un disque plus rapide.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>Replacer l'index dans le dossier d'images.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>Par défaut</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let ImageWorker locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>Construire aussi les descripteurs neuronaux qui permettent de localiser
un asset dans une capture d'écran. Double à peu près la durée d'indexation
et ajoute environ 40 Ko par image ; sans eux, seules la détection de
doublons et la similarité globale fonctionnent.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>Données de recherche d'objet</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="168" />
        <source>Duplicates</source>
        <translation>Doublons</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="176" />
        <location filename="../src/gui/MainWindow.ui" line="345" />
        <source>Max distance:</source>
        <translation>Distance max. :</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="183" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>Rayon de Hamming sur l'empreinte perceptuelle 64 bits.
0 = empreintes identiques, 4 = tolérant au réencodage et au redimensionnement.
Au-delà de 8, les résultats sont à examiner, pas à supprimer : l'empreinte décrit
la composition, si bien qu'un personnage aux expressions variées finit dans un même groupe.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="199" />
        <source>Exact</source>
        <translation>Exacts</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="209" />
        <source>Near</source>
        <translation>Proches</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="219" />
        <source>&amp;Find duplicates</source>
        <translation>&amp;Chercher les doublons</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="258" />
        <source>Find by example</source>
        <translation>Recherche par l'exemple</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="280" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>Image de référence : choisissez un fichier, déposez-le ou collez avec Ctrl+V</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Paste</source>
        <translation>Coller</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="303" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <location filename="../src/gui/MainWindow.cpp" line="966" />
        <source>No reference image yet.</source>
        <translation>Pas encore d'image de référence.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="315" />
        <source>Method:</source>
        <translation>Méthode :</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="325" />
        <source>Top:</source>
        <translation>Meilleurs :</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="352" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>Distance combinée pHash + dHash, 0..128.
L'empreinte perceptuelle retrouve la même image, pas seulement un sujet voisin.
En dessous de 16 environ, les résultats sont la même image réencodée ou redimensionnée.
Au-dessus de 30, ils sont quasi aléatoires, quoi qu'en dise le score.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="368" />
        <source>&amp;Search</source>
        <translation>&amp;Rechercher</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="416" />
        <source>Path:</source>
        <translation>Chemin :</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="426" />
        <source>Select a result to see its full path</source>
        <translation>Sélectionnez un résultat pour voir son chemin complet</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="443" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>Copier le chemin complet de chaque résultat sélectionné, un par ligne.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="446" />
        <source>&amp;Copy</source>
        <translation>C&amp;opier</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="125" />
        <location filename="../src/gui/MainWindow.cpp" line="603" />
        <source>Choose a folder to begin.</source>
        <translation>Choisissez un dossier pour commencer.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="130" />
        <source>Paste screenshot</source>
        <translation>Coller une capture d'écran</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="230" />
        <location filename="../src/gui/MainWindow.cpp" line="424" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>Recherche sur toute l'image. Tracez un cadre pour chercher un seul objet.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="235" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>Recherche dans la zone sélectionnée %1 x %2 — plus rapide et plus précis que l'image entière.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="245" />
        <location filename="../src/gui/MainWindow.cpp" line="404" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>Coller une capture d'écran depuis le presse-papiers (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="406" />
        <source>Locate object in the image</source>
        <translation>Localiser un objet dans l'image</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="250" />
        <location filename="../src/gui/MainWindow.cpp" line="407" />
        <source>Whole-image similarity</source>
        <translation>Similarité de l'image entière</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>Caractéristiques locales neuronales : retrouvent un asset réellement présent
dans l'image, même petit, redimensionné ou noyé dans le décor, et montrent où.
Nécessite un index avec descripteurs et un vocabulaire entraîné.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="259" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>Empreinte perceptuelle : compare l'image dans son ensemble.
Efficace pour les copies redimensionnées ou réencodées de la même image,
inutile pour trouver un petit objet dans une capture d'écran.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="290" />
        <source>&amp;View</source>
        <translation>&amp;Affichage</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="291" />
        <source>&amp;Theme</source>
        <translation>&amp;Thème</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="292" />
        <source>&amp;Language</source>
        <translation>&amp;Langue</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="298" />
        <source>&amp;Help</source>
        <translation>Aid&amp;e</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="300" />
        <source>&amp;User guide</source>
        <translation>&amp;Guide d'utilisation</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="305" />
        <source>&amp;Command line and automation</source>
        <translation>&amp;Ligne de commande et automatisation</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="310" />
        <source>&amp;About</source>
        <translation>À &amp;propos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="312" />
        <source>About ImageWorker</source>
        <translation>À propos d'ImageWorker</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="313" />
        <source>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;imageworker-gui&lt;/b&gt; and the headless &lt;b&gt;imageworker&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;Trouve quels assets apparaissent dans une capture d'écran, et à quel endroit ; et regroupe les doublons d'un dossier.&lt;/p&gt;&lt;p&gt;Livré en deux exécutables issus d'une même bibliothèque : &lt;b&gt;imageworker-gui&lt;/b&gt; et &lt;b&gt;imageworker&lt;/b&gt; en ligne de commande.&lt;/p&gt;&lt;p&gt;&lt;b&gt;F1&lt;/b&gt; ouvre le manuel.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="354" />
        <source>Theme: %1</source>
        <translation>Thème : %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="373" />
        <source>System language</source>
        <translation>Langue du système</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="384" />
        <source>No translation for %1 in this build.</source>
        <translation>Aucune traduction pour %1 dans cette version.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="387" />
        <source>Language: %1</source>
        <translation>Langue : %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="465" />
        <source>Copied %1</source>
        <translation>%1 copié</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="495" />
        <source>+%n more</source>
        <translation><numerusform>+%n autre</numerusform><numerusform>+%n autres</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="521" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation><numerusform>%n chemin complet copié dans le presse-papiers.</numerusform><numerusform>%n chemins complets copiés dans le presse-papiers.</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="522" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation><numerusform>%n chemin relatif copié dans le presse-papiers.</numerusform><numerusform>%n chemins relatifs copiés dans le presse-papiers.</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="551" />
        <source>Choose where to keep the index</source>
        <translation>Choisir où conserver l'index</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="582" />
        <source>Choose a folder</source>
        <translation>Choisir un dossier</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="609" />
        <source>Choose where the index should be kept.</source>
        <translation>Choisissez où conserver l'index.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="614" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>Aucun index dans %1 — appuyez sur « Indexer » pour en créer un.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Cannot open index: %1</source>
        <translation>Impossible d'ouvrir l'index : %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="633" />
        <source>%1 indexed files, %2%3</source>
        <translation>%1 fichiers indexés, %2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="638" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — aucun descripteur, localisation d'objet indisponible</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="652" />
        <source>Choose an existing folder first.</source>
        <translation>Choisissez d'abord un dossier existant.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="665" />
        <source>Cannot create the index directory:
%1</source>
        <translation>Impossible de créer le dossier d'index :
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="682" />
        <source>Indexing</source>
        <translation>Indexation</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="706" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>Indexés %1, inchangés %2, en échec %3, retirés %4 — %5 s%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="712" />
        <location filename="../src/gui/MainWindow.cpp" line="914" />
        <source> (cancelled)</source>
        <translation> (annulé)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="718" />
        <source>Training the visual vocabulary</source>
        <translation>Entraînement du vocabulaire visuel</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>No descriptors to train on.</source>
        <translation>Aucun descripteur pour l'entraînement.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="791" />
        <source>Vocabulary failed: %1</source>
        <translation>Échec du vocabulaire : %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="801" />
        <source>Shortlist index failed: %1</source>
        <translation>Échec de l'index de présélection : %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="812" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>Prêt : %1 images cherchables par objet, vocabulaire de %2 mots.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="826" />
        <source>Cancelling…</source>
        <translation>Annulation…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="839" />
        <location filename="../src/gui/MainWindow.cpp" line="1050" />
        <location filename="../src/gui/MainWindow.cpp" line="1275" />
        <source>Build an index for this folder first.</source>
        <translation>Créez d'abord un index pour ce dossier.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="850" />
        <source>Matching</source>
        <translation>Comparaison</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="897" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 fichiers  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="900" />
        <source>exact</source>
        <translation>exact</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="901" />
        <source>near d≤%1</source>
        <translation>proche d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="906" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 groupes, %2 récupérables</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="910" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — %1 groupes d'empreintes trop volumineux ignorés</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="937" />
        <source>Choose a reference image</source>
        <translation>Choisir une image de référence</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="938" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>Images (*.png *.jpg *.jpeg);;Tous les fichiers (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="954" />
        <source>Cannot read %1: %2</source>
        <translation>Impossible de lire %1 : %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="984" />
        <source>The clipboard holds no image.</source>
        <translation>Le presse-papiers ne contient aucune image.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="988" />
        <source>(clipboard, %1 x %2)</source>
        <translation>(presse-papiers, %1 x %2)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="992" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>Capture %1 x %2 collée — tracez un cadre autour de l'objet.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1019" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>Cet index n'a encore ni descripteurs ni vocabulaire. Créez-les ainsi :
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1034" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>Cet index ne contient pas de données de recherche d'objet — appuyez sur « Indexer » avec « Données de recherche d'objet » coché. D'ici là, seule la similarité de l'image entière est disponible, et elle ne sait pas trouver un objet à l'intérieur d'une image.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1056" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</source>
        <translation>Cet index n'a pas encore de descripteurs de caractéristiques locales : impossible de localiser un objet dans une image.

Créez-les d'abord :

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1069" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>Collez d'abord une capture avec Ctrl+V, déposez une image ou choisissez un fichier.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1095" />
        <source>Locating</source>
        <translation>Localisation</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1154" />
        <source>Searching</source>
        <translation>Recherche</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1192" />
        <source>distance</source>
        <translation>distance</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1200" />
        <source>the same picture</source>
        <translation>la même image</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1202" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>vraisemblablement la même image, réencodée ou redimensionnée</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1204" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>rien de convaincant — elles sont très éloignées, quoi qu'en disent les scores</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1207" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>%1 résultats sur %2 fichiers indexés, distance minimale %3 : %4. Similarité de l'image entière, pas localisation d'objet.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1231" />
        <source>inliers</source>
        <translation>inliers</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1236" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>Rien trouvé. Essayez d'encadrer l'objet, ou vérifiez que l'asset est bien indexé.</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1252" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation><numerusform>%n asset localisé ; meilleur : %1 (%2 inliers, %3 % cohérents)</numerusform><numerusform>%n assets localisés ; meilleur : %1 (%2 inliers, %3 % cohérents)</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1288" />
        <source>Looking for copies of %1…</source>
        <translation>Recherche de copies de %1…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1320" />
        <source>Open</source>
        <translation>Ouvrir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1323" />
        <source>Reveal in file manager</source>
        <translation>Afficher dans le gestionnaire de fichiers</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1327" />
        <source>Find duplicates of this image</source>
        <translation>Chercher les doublons de cette image</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1329" />
        <source>Use as reference image</source>
        <translation>Utiliser comme image de référence</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1334" />
        <source>Copy full path</source>
        <translation>Copier le chemin complet</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1335" />
        <source>Copy path relative to the root</source>
        <translation>Copier le chemin relatif à la racine</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>Ctrl+V pour coller une capture d'écran,
ou déposez une image ici</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="146" />
        <source>distance %1</source>
        <translation>distance %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="87" />
        <source>scanning</source>
        <translation>analyse</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="88" />
        <source>indexing</source>
        <translation>indexation</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="89" />
        <source>descriptors</source>
        <translation>descripteurs</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>pruning</source>
        <translation>nettoyage</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>loading</source>
        <translation>chargement</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>matching</source>
        <translation>comparaison</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>collecting</source>
        <translation>collecte</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>verifying</source>
        <translation>vérification</translation>
    </message>
</context>
<context>
    <name>theme</name>
    <message>
        <location filename="../src/gui/Theme.cpp" line="228" />
        <source>System</source>
        <translation>Système</translation>
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
        <translation>Floraison</translation>
    </message>
</context>
</TS>