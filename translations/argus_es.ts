<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es">
<context>
    <name>BranchDialog</name>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="21" />
        <source>Branches to index</source>
        <translation>Ramas que indexar</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="27" />
        <source>Files are read straight out of the repository, so nothing is checked out and the working tree is never touched. The branch you have checked out is not listed: it is the working tree, which the index already covers.</source>
        <translation>Los archivos se leen directamente del repositorio, así que no se extrae nada y el árbol de trabajo no se toca. La rama que tiene activa no aparece en la lista: es el árbol de trabajo, que el índice ya cubre.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="39" />
        <source>Include remote-tracking branches</source>
        <translation>Incluir ramas de seguimiento remotas</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="50" />
        <source>Select none</source>
        <translation>No seleccionar ninguna</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="87" />
        <source>This repository has no branches.</source>
        <translation>Este repositorio no tiene ramas.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="88" />
        <source>Cannot list branches: %1</source>
        <translation>No se pueden listar las ramas: %1</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/BranchDialog.cpp" line="100" />
        <source>%n branch(es) available.</source>
        <translation><numerusform>%n rama disponible.</numerusform><numerusform>%n ramas disponibles.</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="103" />
        <source>Indexed earlier but gone from the repository: %1. Pressing OK drops them from the index.</source>
        <translation>Indexadas antes pero ya no están en el repositorio: %1. Al pulsar «Aceptar» se eliminan del índice.</translation>
    </message>
</context>
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="18" />
        <location filename="../src/gui/HelpDialog.cpp" line="85" />
        <source>Argus — Help</source>
        <translation>Argus — Ayuda</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="57" />
        <source>Using Argus</source>
        <translation>Cómo usar Argus</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="58" />
        <source>Command line</source>
        <translation>Línea de comandos</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="75" />
        <source>Could not load %1.</source>
        <translation>No se pudo cargar %1.</translation>
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
        <translation>Imágenes:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>Carpeta con archivos png / jpg</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="301" />
        <source>Browse…</source>
        <translation>Examinar…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="75" />
        <source>&amp;Index</source>
        <translation>&amp;Indexar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="91" />
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="98" />
        <source>Index in:</source>
        <translation>Índice en:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .argus inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>Carpeta con la base de datos del índice, las miniaturas y los descriptores.
De forma predeterminada, .argus dentro de la carpeta de imágenes;
elija otra para no tocar un recurso de red o de solo lectura, o para
poner el índice en un disco más rápido.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>Devolver el índice a la carpeta de imágenes.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>Predeterminado</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let Argus locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>Construir además los descriptores neuronales que permiten localizar
un recurso dentro de una captura de pantalla. Duplica aproximadamente
el tiempo de indexación y añade unos 40 KB por imagen; sin ellos solo
funcionan la detección de duplicados y la similitud de la imagen completa.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>Datos de búsqueda de objetos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="151" />
        <source>Pick which git branches to index alongside the working tree.
Enabled only when the image folder is inside a git repository.</source>
        <translation>Elija qué ramas de git indexar junto al árbol de trabajo.
Solo disponible cuando la carpeta de imágenes está dentro de un repositorio git.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="155" />
        <source>Branches…</source>
        <translation>Ramas…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="182" />
        <source>Duplicates</source>
        <translation>Duplicados</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="190" />
        <location filename="../src/gui/MainWindow.ui" line="359" />
        <source>Max distance:</source>
        <translation>Distancia máx.:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="197" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>Radio de Hamming sobre el hash perceptual de 64 bits.
0 = huellas idénticas, 4 = tolera recodificación y reescalado.
Por encima de 8 los resultados son para revisar, no para borrar: el hash describe
la composición, así que un personaje con varias expresiones acaba en el mismo grupo.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="213" />
        <source>Exact</source>
        <translation>Exactos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="223" />
        <source>Near</source>
        <translation>Cercanos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="233" />
        <source>&amp;Find duplicates</source>
        <translation>&amp;Buscar duplicados</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="272" />
        <source>Find by example</source>
        <translation>Búsqueda por ejemplo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>Imagen de referencia: elija un archivo, arrástrelo o péguelo con Ctrl+V</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="308" />
        <source>Paste</source>
        <translation>Pegar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="317" />
        <location filename="../src/gui/MainWindow.cpp" line="428" />
        <location filename="../src/gui/MainWindow.cpp" line="1029" />
        <source>No reference image yet.</source>
        <translation>Aún no hay imagen de referencia.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="329" />
        <source>Method:</source>
        <translation>Método:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="339" />
        <source>Top:</source>
        <translation>Mejores:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="366" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>Distancia combinada pHash + dHash, 0..128.
El hash perceptual encuentra la misma imagen, no solo un motivo parecido.
Por debajo de 16 aproximadamente, los resultados son la misma imagen recodificada o reescalada.
Por encima de 30 son prácticamente aleatorios, diga lo que diga la puntuación.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="382" />
        <source>&amp;Search</source>
        <translation>&amp;Buscar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="430" />
        <source>Path:</source>
        <translation>Ruta:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="440" />
        <source>Select a result to see its full path</source>
        <translation>Seleccione un resultado para ver su ruta completa</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="457" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>Copiar la ruta completa de cada resultado seleccionado, una por línea.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="460" />
        <source>&amp;Copy</source>
        <translation>&amp;Copiar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="128" />
        <location filename="../src/gui/MainWindow.cpp" line="645" />
        <source>Choose a folder to begin.</source>
        <translation>Elija una carpeta para empezar.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="133" />
        <source>Paste screenshot</source>
        <translation>Pegar captura de pantalla</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="234" />
        <location filename="../src/gui/MainWindow.cpp" line="430" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>Se busca en toda la imagen. Dibuje un recuadro para buscar un solo objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="239" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>Se busca en la región seleccionada de %1 x %2: más rápido y más preciso que la imagen entera.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>Pegar una captura de pantalla desde el portapapeles (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="412" />
        <source>Locate object in the image</source>
        <translation>Localizar un objeto en la imagen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="254" />
        <location filename="../src/gui/MainWindow.cpp" line="413" />
        <source>Whole-image similarity</source>
        <translation>Similitud de la imagen completa</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="257" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>Características locales neuronales: encuentran un recurso realmente presente
en la imagen, aunque sea pequeño, reescalado o rodeado de ruido, y muestran dónde.
Requiere un índice con descriptores y un vocabulario entrenado.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="263" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>Hash perceptual: compara la imagen en su conjunto.
Bueno para copias reescaladas o recodificadas de la misma imagen,
inútil para encontrar un objeto pequeño dentro de una captura.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="294" />
        <source>&amp;View</source>
        <translation>&amp;Ver</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="295" />
        <source>&amp;Theme</source>
        <translation>&amp;Tema</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="296" />
        <source>&amp;Language</source>
        <translation>&amp;Idioma</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="302" />
        <source>&amp;Help</source>
        <translation>A&amp;yuda</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="304" />
        <source>&amp;User guide</source>
        <translation>&amp;Guía de usuario</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="309" />
        <source>&amp;Command line and automation</source>
        <translation>&amp;Línea de comandos y automatización</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="314" />
        <source>&amp;About</source>
        <translation>&amp;Acerca de</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="316" />
        <source>About Argus</source>
        <translation>Acerca de Argus</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="317" />
        <source>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;argus-gui&lt;/b&gt; and the headless &lt;b&gt;argus&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Encuentra qué recursos gráficos aparecen dentro de una captura de pantalla, y dónde; y agrupa los duplicados de una carpeta.&lt;/p&gt;&lt;p&gt;Se distribuye como dos ejecutables construidos sobre una misma biblioteca: &lt;b&gt;argus-gui&lt;/b&gt; y el &lt;b&gt;argus&lt;/b&gt; de consola.&lt;/p&gt;&lt;p&gt;Pulse &lt;b&gt;F1&lt;/b&gt; para abrir el manual.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="358" />
        <source>Theme: %1</source>
        <translation>Tema: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="377" />
        <source>System language</source>
        <translation>Idioma del sistema</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="388" />
        <source>No translation for %1 in this build.</source>
        <translation>Esta versión no incluye traducción a %1.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="391" />
        <source>Language: %1</source>
        <translation>Idioma: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="471" />
        <source>Copied %1</source>
        <translation>Copiado %1</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="501" />
        <source>+%n more</source>
        <translation>
            <numerusform>+%n más</numerusform>
            <numerusform>+%n más</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="527" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation>
            <numerusform>Se copió %n ruta completa al portapapeles.</numerusform>
            <numerusform>Se copiaron %n rutas completas al portapapeles.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="528" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation>
            <numerusform>Se copió %n ruta relativa al portapapeles.</numerusform>
            <numerusform>Se copiaron %n rutas relativas al portapapeles.</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="557" />
        <source>Choose where to keep the index</source>
        <translation>Elegir dónde guardar el índice</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="588" />
        <source>Choose a folder</source>
        <translation>Elegir una carpeta</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="651" />
        <source>Choose where the index should be kept.</source>
        <translation>Elija dónde guardar el índice.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="656" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>No hay índice en %1: pulse «Indexar» para crear uno.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="664" />
        <source>Cannot open index: %1</source>
        <translation>No se puede abrir el índice: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="678" />
        <source>%1 indexed files, %2%3</source>
        <translation>%1 archivos indexados, %2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="683" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — sin descriptores, la localización de objetos no está disponible</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="697" />
        <source>Choose an existing folder first.</source>
        <translation>Elija primero una carpeta existente.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="710" />
        <source>Cannot create the index directory:
%1</source>
        <translation>No se puede crear la carpeta del índice:
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="732" />
        <source>Indexing</source>
        <translation>Indexación</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="769" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>Indexados %1, sin cambios %2, fallidos %3, eliminados %4 — %5 s%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="764" />
        <location filename="../src/gui/MainWindow.cpp" line="775" />
        <location filename="../src/gui/MainWindow.cpp" line="977" />
        <source> (cancelled)</source>
        <translation> (cancelado)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Only the working tree will be indexed.</source>
        <translation>Solo se indexará el árbol de trabajo.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="623" />
        <source>Branches to index: %1. Press Index to read them.</source>
        <translation>Ramas que indexar: %1. Pulse «Indexar» para leerlas.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="757" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6; branches: %7 read, %8 unchanged</source>
        <translation>Indexados %1, sin cambios %2, fallidos %3, eliminados %4 — %5 s%6; ramas: %7 leídas, %8 sin cambios</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>Training the visual vocabulary</source>
        <translation>Entrenando el vocabulario visual</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="844" />
        <source>No descriptors to train on.</source>
        <translation>No hay descriptores con los que entrenar.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="854" />
        <source>Vocabulary failed: %1</source>
        <translation>Error en el vocabulario: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="864" />
        <source>Shortlist index failed: %1</source>
        <translation>Error en el índice de preselección: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="875" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>Listo: %1 imágenes buscables por objeto, vocabulario de %2 palabras.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="889" />
        <source>Cancelling…</source>
        <translation>Cancelando…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="902" />
        <location filename="../src/gui/MainWindow.cpp" line="1113" />
        <location filename="../src/gui/MainWindow.cpp" line="1338" />
        <source>Build an index for this folder first.</source>
        <translation>Cree primero un índice para esta carpeta.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="913" />
        <source>Matching</source>
        <translation>Comparación</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="960" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 archivos  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="963" />
        <source>exact</source>
        <translation>exacto</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="964" />
        <source>near d≤%1</source>
        <translation>cercano d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="969" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 grupos, %2 recuperables</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="973" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — se omitieron %1 grupos de hash demasiado grandes</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1000" />
        <source>Choose a reference image</source>
        <translation>Elegir una imagen de referencia</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1001" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>Imágenes (*.png *.jpg *.jpeg);;Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1017" />
        <source>Cannot read %1: %2</source>
        <translation>No se puede leer %1: %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1047" />
        <source>The clipboard holds no image.</source>
        <translation>El portapapeles no contiene ninguna imagen.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1051" />
        <source>(clipboard, %1 x %2)</source>
        <translation>(portapapeles, %1 x %2)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1055" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>Captura de %1 x %2 pegada: dibuje un recuadro alrededor del objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1082" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>Este índice todavía no tiene descriptores ni vocabulario. Créelos así:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1097" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>Este índice no tiene datos de búsqueda de objetos: pulse «Indexar» con «Datos de búsqueda de objetos» marcado. Hasta entonces solo está disponible la similitud de la imagen completa, que no puede encontrar un objeto dentro de una imagen.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1119" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</source>
        <translation>Este índice todavía no tiene descriptores de características locales, así que no se puede localizar un objeto dentro de una imagen.

Créelos primero:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1132" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>Primero pegue una captura con Ctrl+V, arrastre una imagen o elija un archivo.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1158" />
        <source>Locating</source>
        <translation>Localización</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1217" />
        <source>Searching</source>
        <translation>Búsqueda</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1255" />
        <source>distance</source>
        <translation>distancia</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1263" />
        <source>the same picture</source>
        <translation>la misma imagen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1265" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>probablemente la misma imagen, recodificada o reescalada</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1267" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>nada convincente: están muy lejos entre sí, diga lo que diga la puntuación</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1270" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>%1 coincidencias de %2 archivos indexados, distancia mínima %3: %4. Es similitud de la imagen completa, no localización de objetos.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1294" />
        <source>inliers</source>
        <translation>inliers</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1299" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>No se localizó nada. Pruebe a encuadrar el objeto o compruebe que el recurso esté realmente indexado.</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1315" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation>
            <numerusform>Localizado %n recurso; el mejor: %1 (%2 inliers, %3 % coherentes)</numerusform>
            <numerusform>Localizados %n recursos; el mejor: %1 (%2 inliers, %3 % coherentes)</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1351" />
        <source>Looking for copies of %1…</source>
        <translation>Buscando copias de %1…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1367" />
        <source>%1 lives in branch %2, and this folder is no longer a git repository.</source>
        <translation>%1 está en la rama %2, y esta carpeta ya no es un repositorio git.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1376" />
        <source>Cannot read %1 from branch %2: %3</source>
        <translation>No se puede leer %1 de la rama %2: %3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1394" />
        <source>Cannot write a temporary copy of %1.</source>
        <translation>No se puede escribir una copia temporal de %1.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1402" />
        <source>Opened %1 from branch %2.</source>
        <translation>Se abrió %1 de la rama %2.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1430" />
        <source>Open</source>
        <translation>Abrir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1435" />
        <source>Reveal in file manager</source>
        <translation>Mostrar en el gestor de archivos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1441" />
        <source>Find duplicates of this image</source>
        <translation>Buscar duplicados de esta imagen</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1443" />
        <source>Use as reference image</source>
        <translation>Usar como imagen de referencia</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1455" />
        <source>Only for files in the working tree</source>
        <translation>Solo para archivos del árbol de trabajo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1460" />
        <source>Copy full path</source>
        <translation>Copiar ruta completa</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1461" />
        <source>Copy path relative to the root</source>
        <translation>Copiar ruta relativa a la raíz</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>Ctrl+V para pegar una captura de pantalla,
o arrastre una imagen aquí</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="159" />
        <source>distance %1</source>
        <translation>distancia %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>scanning</source>
        <translation>explorando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>indexing</source>
        <translation>indexando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>descriptors</source>
        <translation>descriptores</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>pruning</source>
        <translation>limpiando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>loading</source>
        <translation>cargando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="95" />
        <source>matching</source>
        <translation>comparando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="96" />
        <source>collecting</source>
        <translation>recopilando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="97" />
        <source>verifying</source>
        <translation>verificando</translation>
    </message>
</context>
<context>
    <name>theme</name>
    <message>
        <location filename="../src/gui/Theme.cpp" line="228" />
        <source>System</source>
        <translation>Sistema</translation>
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
        <translation>Floral</translation>
    </message>
</context>
</TS>