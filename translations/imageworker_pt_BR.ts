<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="pt_BR">
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="13" />
        <source>ImageWorker — Help</source>
        <translation>ImageWorker — Ajuda</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="25" />
        <source>Using ImageWorker</source>
        <translation>Como usar o ImageWorker</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="26" />
        <source>Command line</source>
        <translation>Linha de comando</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="42" />
        <source>Could not load %1.</source>
        <translation>Não foi possível carregar %1.</translation>
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
        <translation>Imagens:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>Pasta com arquivos png / jpg</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="287" />
        <source>Browse…</source>
        <translation>Procurar…</translation>
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
        <translation>Índice em:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .imageworker inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>Pasta com o banco do índice, as miniaturas e os descritores.
Por padrão, .imageworker dentro da pasta de imagens; aponte para outra
para não mexer em um compartilhamento de rede ou somente leitura, ou
para colocar o índice em um disco mais rápido.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>Devolver o índice para dentro da pasta de imagens.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>Padrão</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let ImageWorker locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>Construir também os descritores neurais que permitem localizar um
asset dentro de uma captura de tela. Dobra aproximadamente o tempo
de indexação e acrescenta cerca de 40 KB por imagem; sem eles só
funcionam a busca de duplicatas e a similaridade da imagem inteira.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>Dados de busca de objetos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="168" />
        <source>Duplicates</source>
        <translation>Duplicatas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="176" />
        <location filename="../src/gui/MainWindow.ui" line="345" />
        <source>Max distance:</source>
        <translation>Distância máx.:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="183" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>Raio de Hamming sobre o hash perceptual de 64 bits.
0 = impressões idênticas, 4 = tolera recodificação e redimensionamento.
Acima de 8 os resultados são para revisão, não para exclusão: o hash descreve
a composição, então um personagem com várias expressões cai no mesmo grupo.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="199" />
        <source>Exact</source>
        <translation>Exatas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="209" />
        <source>Near</source>
        <translation>Próximas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="219" />
        <source>&amp;Find duplicates</source>
        <translation>&amp;Buscar duplicatas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="258" />
        <source>Find by example</source>
        <translation>Busca por exemplo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="280" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>Imagem de referência: escolha um arquivo, arraste-o ou cole com Ctrl+V</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Paste</source>
        <translation>Colar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="303" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <location filename="../src/gui/MainWindow.cpp" line="966" />
        <source>No reference image yet.</source>
        <translation>Ainda não há imagem de referência.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="315" />
        <source>Method:</source>
        <translation>Método:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="325" />
        <source>Top:</source>
        <translation>Melhores:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="352" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>Distância combinada pHash + dHash, 0..128.
O hash perceptual encontra a mesma imagem, não apenas um tema parecido.
Abaixo de cerca de 16 os resultados são a mesma imagem recodificada ou redimensionada.
Acima de 30 são praticamente aleatórios, seja qual for a pontuação.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="368" />
        <source>&amp;Search</source>
        <translation>&amp;Buscar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="416" />
        <source>Path:</source>
        <translation>Caminho:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="426" />
        <source>Select a result to see its full path</source>
        <translation>Selecione um resultado para ver o caminho completo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="443" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>Copiar o caminho completo de cada resultado selecionado, um por linha.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="446" />
        <source>&amp;Copy</source>
        <translation>&amp;Copiar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="125" />
        <location filename="../src/gui/MainWindow.cpp" line="603" />
        <source>Choose a folder to begin.</source>
        <translation>Escolha uma pasta para começar.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="130" />
        <source>Paste screenshot</source>
        <translation>Colar captura de tela</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="230" />
        <location filename="../src/gui/MainWindow.cpp" line="424" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>Buscando na imagem inteira. Arraste um retângulo para procurar um único objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="235" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>Buscando na região selecionada de %1 x %2 — mais rápido e mais preciso do que o quadro inteiro.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="245" />
        <location filename="../src/gui/MainWindow.cpp" line="404" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>Colar uma captura de tela da área de transferência (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="406" />
        <source>Locate object in the image</source>
        <translation>Localizar objeto na imagem</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="250" />
        <location filename="../src/gui/MainWindow.cpp" line="407" />
        <source>Whole-image similarity</source>
        <translation>Similaridade da imagem inteira</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>Características locais neurais: encontram um asset realmente presente
na imagem, mesmo pequeno, redimensionado ou cercado de bagunça, e mostram onde.
Exige um índice com descritores e um vocabulário treinado.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="259" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>Hash perceptual: compara a imagem como um todo.
Bom para cópias redimensionadas ou recodificadas da mesma imagem,
inútil para encontrar um objeto pequeno dentro de uma captura de tela.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="290" />
        <source>&amp;View</source>
        <translation>&amp;Exibir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="291" />
        <source>&amp;Theme</source>
        <translation>&amp;Tema</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="292" />
        <source>&amp;Language</source>
        <translation>&amp;Idioma</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="298" />
        <source>&amp;Help</source>
        <translation>&amp;Ajuda</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="300" />
        <source>&amp;User guide</source>
        <translation>&amp;Guia do usuário</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="305" />
        <source>&amp;Command line and automation</source>
        <translation>&amp;Linha de comando e automação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="310" />
        <source>&amp;About</source>
        <translation>&amp;Sobre</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="312" />
        <source>About ImageWorker</source>
        <translation>Sobre o ImageWorker</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="313" />
        <source>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;imageworker-gui&lt;/b&gt; and the headless &lt;b&gt;imageworker&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;Descobre quais assets aparecem dentro de uma captura de tela, e em que lugar; e agrupa duplicatas de uma pasta.&lt;/p&gt;&lt;p&gt;Distribuído como dois executáveis feitos sobre uma mesma biblioteca: &lt;b&gt;imageworker-gui&lt;/b&gt; e o &lt;b&gt;imageworker&lt;/b&gt; de linha de comando.&lt;/p&gt;&lt;p&gt;Pressione &lt;b&gt;F1&lt;/b&gt; para abrir o manual.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="354" />
        <source>Theme: %1</source>
        <translation>Tema: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="373" />
        <source>System language</source>
        <translation>Idioma do sistema</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="384" />
        <source>No translation for %1 in this build.</source>
        <translation>Esta versão não traz tradução para %1.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="387" />
        <source>Language: %1</source>
        <translation>Idioma: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="465" />
        <source>Copied %1</source>
        <translation>Copiado %1</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="495" />
        <source>+%n more</source>
        <translation><numerusform>+%n a mais</numerusform><numerusform>+%n a mais</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="521" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation><numerusform>%n caminho completo copiado para a área de transferência.</numerusform><numerusform>%n caminhos completos copiados para a área de transferência.</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="522" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation><numerusform>%n caminho relativo copiado para a área de transferência.</numerusform><numerusform>%n caminhos relativos copiados para a área de transferência.</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="551" />
        <source>Choose where to keep the index</source>
        <translation>Escolher onde guardar o índice</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="582" />
        <source>Choose a folder</source>
        <translation>Escolher uma pasta</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="609" />
        <source>Choose where the index should be kept.</source>
        <translation>Escolha onde guardar o índice.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="614" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>Nenhum índice em %1 — pressione «Indexar» para criar um.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Cannot open index: %1</source>
        <translation>Não é possível abrir o índice: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="633" />
        <source>%1 indexed files, %2%3</source>
        <translation>%1 arquivos indexados, %2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="638" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — sem descritores, localização de objetos indisponível</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="652" />
        <source>Choose an existing folder first.</source>
        <translation>Escolha primeiro uma pasta existente.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="665" />
        <source>Cannot create the index directory:
%1</source>
        <translation>Não é possível criar a pasta do índice:
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="682" />
        <source>Indexing</source>
        <translation>Indexação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="706" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>Indexados %1, inalterados %2, com falha %3, removidos %4 — %5 s%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="712" />
        <location filename="../src/gui/MainWindow.cpp" line="914" />
        <source> (cancelled)</source>
        <translation> (cancelado)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="718" />
        <source>Training the visual vocabulary</source>
        <translation>Treinando o vocabulário visual</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>No descriptors to train on.</source>
        <translation>Não há descritores para treinar.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="791" />
        <source>Vocabulary failed: %1</source>
        <translation>Falha no vocabulário: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="801" />
        <source>Shortlist index failed: %1</source>
        <translation>Falha no índice de pré-seleção: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="812" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>Pronto: %1 imagens pesquisáveis por objeto, vocabulário de %2 palavras.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="826" />
        <source>Cancelling…</source>
        <translation>Cancelando…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="839" />
        <location filename="../src/gui/MainWindow.cpp" line="1050" />
        <location filename="../src/gui/MainWindow.cpp" line="1275" />
        <source>Build an index for this folder first.</source>
        <translation>Crie primeiro um índice para esta pasta.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="850" />
        <source>Matching</source>
        <translation>Comparação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="897" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 arquivos  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="900" />
        <source>exact</source>
        <translation>exata</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="901" />
        <source>near d≤%1</source>
        <translation>próxima d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="906" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 grupos, %2 recuperáveis</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="910" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — %1 grupos de hash grandes demais foram ignorados</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="937" />
        <source>Choose a reference image</source>
        <translation>Escolher uma imagem de referência</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="938" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>Imagens (*.png *.jpg *.jpeg);;Todos os arquivos (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="954" />
        <source>Cannot read %1: %2</source>
        <translation>Não é possível ler %1: %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="984" />
        <source>The clipboard holds no image.</source>
        <translation>A área de transferência não contém imagem.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="988" />
        <source>(clipboard, %1 x %2)</source>
        <translation>(área de transferência, %1 x %2)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="992" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>Captura de %1 x %2 colada — desenhe um retângulo em volta do objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1019" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>Este índice ainda não tem descritores nem vocabulário. Crie-os assim:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1034" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>Este índice não tem dados de busca de objetos — pressione «Indexar» com «Dados de busca de objetos» marcado. Até lá só está disponível a similaridade da imagem inteira, que não encontra um objeto dentro de uma imagem.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1056" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</source>
        <translation>Este índice ainda não tem descritores de características locais, então não é possível localizar um objeto dentro de uma imagem.

Crie-os primeiro:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1069" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>Primeiro cole uma captura com Ctrl+V, arraste uma imagem ou escolha um arquivo.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1095" />
        <source>Locating</source>
        <translation>Localização</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1154" />
        <source>Searching</source>
        <translation>Busca</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1192" />
        <source>distance</source>
        <translation>distância</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1200" />
        <source>the same picture</source>
        <translation>a mesma imagem</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1202" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>provavelmente a mesma imagem, recodificada ou redimensionada</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1204" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>nada convincente — estão muito distantes, seja qual for a pontuação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1207" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>%1 resultados de %2 arquivos indexados, menor distância %3: %4. É similaridade da imagem inteira, não localização de objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1231" />
        <source>inliers</source>
        <translation>inliers</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1236" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>Nada localizado. Tente delimitar o objeto ou verifique se o asset está mesmo indexado.</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1252" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation><numerusform>%n asset localizado; melhor: %1 (%2 inliers, %3% consistentes)</numerusform><numerusform>%n assets localizados; melhor: %1 (%2 inliers, %3% consistentes)</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1288" />
        <source>Looking for copies of %1…</source>
        <translation>Procurando cópias de %1…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1320" />
        <source>Open</source>
        <translation>Abrir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1323" />
        <source>Reveal in file manager</source>
        <translation>Mostrar no gerenciador de arquivos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1327" />
        <source>Find duplicates of this image</source>
        <translation>Buscar duplicatas desta imagem</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1329" />
        <source>Use as reference image</source>
        <translation>Usar como imagem de referência</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1334" />
        <source>Copy full path</source>
        <translation>Copiar caminho completo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1335" />
        <source>Copy path relative to the root</source>
        <translation>Copiar caminho relativo à raiz</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>Ctrl+V para colar uma captura de tela,
ou arraste uma imagem para cá</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="146" />
        <source>distance %1</source>
        <translation>distância %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="87" />
        <source>scanning</source>
        <translation>varredura</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="88" />
        <source>indexing</source>
        <translation>indexação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="89" />
        <source>descriptors</source>
        <translation>descritores</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>pruning</source>
        <translation>limpeza</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>loading</source>
        <translation>carregando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>matching</source>
        <translation>comparando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>collecting</source>
        <translation>coletando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
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