<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="pt_BR">
<context>
    <name>BranchDialog</name>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="21" />
        <source>Branches to index</source>
        <translation>Branches a indexar</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="27" />
        <source>Files are read straight out of the repository, so nothing is checked out and the working tree is never touched. The branch you have checked out is not listed: it is the working tree, which the index already covers.</source>
        <translation>Os arquivos são lidos direto do repositório, então nada é feito checkout e a árvore de trabalho não é tocada. O branch em que você está não aparece na lista: ele é a árvore de trabalho, que o índice já cobre.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="39" />
        <source>Include remote-tracking branches</source>
        <translation>Incluir branches de rastreamento remoto</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="50" />
        <source>Select none</source>
        <translation>Desmarcar todos</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="87" />
        <source>This repository has no branches.</source>
        <translation>Este repositório não tem branches.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="88" />
        <source>Cannot list branches: %1</source>
        <translation>Não é possível listar os branches: %1</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/BranchDialog.cpp" line="100" />
        <source>%n branch(es) available.</source>
        <translation><numerusform>%n branch disponível.</numerusform><numerusform>%n branches disponíveis.</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="103" />
        <source>Indexed earlier but gone from the repository: %1. Pressing OK drops them from the index.</source>
        <translation>Indexados antes, mas sumiram do repositório: %1. Pressionar OK os remove do índice.</translation>
    </message>
</context>
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="18" />
        <location filename="../src/gui/HelpDialog.cpp" line="85" />
        <source>Argus — Help</source>
        <translation>Argus — Ajuda</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="57" />
        <source>Using Argus</source>
        <translation>Como usar o Argus</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="58" />
        <source>Command line</source>
        <translation>Linha de comando</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="75" />
        <source>Could not load %1.</source>
        <translation>Não foi possível carregar %1.</translation>
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
        <location filename="../src/gui/MainWindow.ui" line="301" />
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
Defaults to .argus inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>Pasta com o banco do índice, as miniaturas e os descritores.
Por padrão, .argus dentro da pasta de imagens; aponte para outra
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
        <source>Also build the neural descriptors that let Argus locate an
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
        <location filename="../src/gui/MainWindow.ui" line="151" />
        <source>Pick which git branches to index alongside the working tree.
Enabled only when the image folder is inside a git repository.</source>
        <translation>Escolha quais branches do git indexar junto da árvore de trabalho.
Disponível só quando a pasta de imagens está dentro de um repositório git.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="155" />
        <source>Branches…</source>
        <translation>Branches…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="182" />
        <source>Duplicates</source>
        <translation>Duplicatas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="190" />
        <location filename="../src/gui/MainWindow.ui" line="359" />
        <source>Max distance:</source>
        <translation>Distância máx.:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="197" />
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
        <location filename="../src/gui/MainWindow.ui" line="213" />
        <source>Exact</source>
        <translation>Exatas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="223" />
        <source>Near</source>
        <translation>Próximas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="233" />
        <source>&amp;Find duplicates</source>
        <translation>&amp;Buscar duplicatas</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="272" />
        <source>Find by example</source>
        <translation>Busca por exemplo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>Imagem de referência: escolha um arquivo, arraste-o ou cole com Ctrl+V</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="308" />
        <source>Paste</source>
        <translation>Colar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="317" />
        <location filename="../src/gui/MainWindow.cpp" line="428" />
        <location filename="../src/gui/MainWindow.cpp" line="1029" />
        <source>No reference image yet.</source>
        <translation>Ainda não há imagem de referência.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="329" />
        <source>Method:</source>
        <translation>Método:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="339" />
        <source>Top:</source>
        <translation>Melhores:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="366" />
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
        <location filename="../src/gui/MainWindow.ui" line="382" />
        <source>&amp;Search</source>
        <translation>&amp;Buscar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="430" />
        <source>Path:</source>
        <translation>Caminho:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="440" />
        <source>Select a result to see its full path</source>
        <translation>Selecione um resultado para ver o caminho completo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="457" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>Copiar o caminho completo de cada resultado selecionado, um por linha.</translation>
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
        <translation>Escolha uma pasta para começar.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="133" />
        <source>Paste screenshot</source>
        <translation>Colar captura de tela</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="234" />
        <location filename="../src/gui/MainWindow.cpp" line="430" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>Buscando na imagem inteira. Arraste um retângulo para procurar um único objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="239" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>Buscando na região selecionada de %1 x %2 — mais rápido e mais preciso do que o quadro inteiro.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>Colar uma captura de tela da área de transferência (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="412" />
        <source>Locate object in the image</source>
        <translation>Localizar objeto na imagem</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="254" />
        <location filename="../src/gui/MainWindow.cpp" line="413" />
        <source>Whole-image similarity</source>
        <translation>Similaridade da imagem inteira</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="257" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>Características locais neurais: encontram um asset realmente presente
na imagem, mesmo pequeno, redimensionado ou cercado de bagunça, e mostram onde.
Exige um índice com descritores e um vocabulário treinado.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="263" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>Hash perceptual: compara a imagem como um todo.
Bom para cópias redimensionadas ou recodificadas da mesma imagem,
inútil para encontrar um objeto pequeno dentro de uma captura de tela.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="294" />
        <source>&amp;View</source>
        <translation>&amp;Exibir</translation>
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
        <translation>&amp;Ajuda</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="304" />
        <source>&amp;User guide</source>
        <translation>&amp;Guia do usuário</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="309" />
        <source>&amp;Command line and automation</source>
        <translation>&amp;Linha de comando e automação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="314" />
        <source>&amp;About</source>
        <translation>&amp;Sobre</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="316" />
        <source>About Argus</source>
        <translation>Sobre o Argus</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="317" />
        <source>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;argus-gui&lt;/b&gt; and the headless &lt;b&gt;argus&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Descobre quais assets aparecem dentro de uma captura de tela, e em que lugar; e agrupa duplicatas de uma pasta.&lt;/p&gt;&lt;p&gt;Distribuído como dois executáveis feitos sobre uma mesma biblioteca: &lt;b&gt;argus-gui&lt;/b&gt; e o &lt;b&gt;argus&lt;/b&gt; de linha de comando.&lt;/p&gt;&lt;p&gt;Pressione &lt;b&gt;F1&lt;/b&gt; para abrir o manual.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="358" />
        <source>Theme: %1</source>
        <translation>Tema: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="377" />
        <source>System language</source>
        <translation>Idioma do sistema</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="388" />
        <source>No translation for %1 in this build.</source>
        <translation>Esta versão não traz tradução para %1.</translation>
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
            <numerusform>+%n a mais</numerusform>
            <numerusform>+%n a mais</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="527" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation>
            <numerusform>%n caminho completo copiado para a área de transferência.</numerusform>
            <numerusform>%n caminhos completos copiados para a área de transferência.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="528" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation>
            <numerusform>%n caminho relativo copiado para a área de transferência.</numerusform>
            <numerusform>%n caminhos relativos copiados para a área de transferência.</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="557" />
        <source>Choose where to keep the index</source>
        <translation>Escolher onde guardar o índice</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="588" />
        <source>Choose a folder</source>
        <translation>Escolher uma pasta</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="651" />
        <source>Choose where the index should be kept.</source>
        <translation>Escolha onde guardar o índice.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="656" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>Nenhum índice em %1 — pressione «Indexar» para criar um.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="664" />
        <source>Cannot open index: %1</source>
        <translation>Não é possível abrir o índice: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="678" />
        <source>%1 indexed files, %2%3</source>
        <translation>%1 arquivos indexados, %2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="683" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — sem descritores, localização de objetos indisponível</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="697" />
        <source>Choose an existing folder first.</source>
        <translation>Escolha primeiro uma pasta existente.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="710" />
        <source>Cannot create the index directory:
%1</source>
        <translation>Não é possível criar a pasta do índice:
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="732" />
        <source>Indexing</source>
        <translation>Indexação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="769" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>Indexados %1, inalterados %2, com falha %3, removidos %4 — %5 s%6</translation>
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
        <translation>Só a árvore de trabalho será indexada.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="623" />
        <source>Branches to index: %1. Press Index to read them.</source>
        <translation>Branches a indexar: %1. Pressione «Indexar» para lê-los.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="757" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6; branches: %7 read, %8 unchanged</source>
        <translation>Indexados %1, inalterados %2, com falha %3, removidos %4 — %5 s%6; branches: %7 lidos, %8 inalterados</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>Training the visual vocabulary</source>
        <translation>Treinando o vocabulário visual</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="844" />
        <source>No descriptors to train on.</source>
        <translation>Não há descritores para treinar.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="854" />
        <source>Vocabulary failed: %1</source>
        <translation>Falha no vocabulário: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="864" />
        <source>Shortlist index failed: %1</source>
        <translation>Falha no índice de pré-seleção: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="875" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>Pronto: %1 imagens pesquisáveis por objeto, vocabulário de %2 palavras.</translation>
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
        <translation>Crie primeiro um índice para esta pasta.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="913" />
        <source>Matching</source>
        <translation>Comparação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="960" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 arquivos  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="963" />
        <source>exact</source>
        <translation>exata</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="964" />
        <source>near d≤%1</source>
        <translation>próxima d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="969" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 grupos, %2 recuperáveis</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="973" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — %1 grupos de hash grandes demais foram ignorados</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1000" />
        <source>Choose a reference image</source>
        <translation>Escolher uma imagem de referência</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1001" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>Imagens (*.png *.jpg *.jpeg);;Todos os arquivos (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1017" />
        <source>Cannot read %1: %2</source>
        <translation>Não é possível ler %1: %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1047" />
        <source>The clipboard holds no image.</source>
        <translation>A área de transferência não contém imagem.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1051" />
        <source>(clipboard, %1 x %2)</source>
        <translation>(área de transferência, %1 x %2)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1055" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>Captura de %1 x %2 colada — desenhe um retângulo em volta do objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1082" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>Este índice ainda não tem descritores nem vocabulário. Crie-os assim:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1097" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>Este índice não tem dados de busca de objetos — pressione «Indexar» com «Dados de busca de objetos» marcado. Até lá só está disponível a similaridade da imagem inteira, que não encontra um objeto dentro de uma imagem.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1119" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</source>
        <translation>Este índice ainda não tem descritores de características locais, então não é possível localizar um objeto dentro de uma imagem.

Crie-os primeiro:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1132" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>Primeiro cole uma captura com Ctrl+V, arraste uma imagem ou escolha um arquivo.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1158" />
        <source>Locating</source>
        <translation>Localização</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1217" />
        <source>Searching</source>
        <translation>Busca</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1255" />
        <source>distance</source>
        <translation>distância</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1263" />
        <source>the same picture</source>
        <translation>a mesma imagem</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1265" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>provavelmente a mesma imagem, recodificada ou redimensionada</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1267" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>nada convincente — estão muito distantes, seja qual for a pontuação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1270" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>%1 resultados de %2 arquivos indexados, menor distância %3: %4. É similaridade da imagem inteira, não localização de objeto.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1294" />
        <source>inliers</source>
        <translation>inliers</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1299" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>Nada localizado. Tente delimitar o objeto ou verifique se o asset está mesmo indexado.</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1315" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation>
            <numerusform>%n asset localizado; melhor: %1 (%2 inliers, %3% consistentes)</numerusform>
            <numerusform>%n assets localizados; melhor: %1 (%2 inliers, %3% consistentes)</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1351" />
        <source>Looking for copies of %1…</source>
        <translation>Procurando cópias de %1…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1367" />
        <source>%1 lives in branch %2, and this folder is no longer a git repository.</source>
        <translation>%1 está no branch %2, e esta pasta não é mais um repositório git.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1376" />
        <source>Cannot read %1 from branch %2: %3</source>
        <translation>Não é possível ler %1 do branch %2: %3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1394" />
        <source>Cannot write a temporary copy of %1.</source>
        <translation>Não é possível gravar uma cópia temporária de %1.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1402" />
        <source>Opened %1 from branch %2.</source>
        <translation>%1 aberto a partir do branch %2.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1430" />
        <source>Open</source>
        <translation>Abrir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1435" />
        <source>Reveal in file manager</source>
        <translation>Mostrar no gerenciador de arquivos</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1441" />
        <source>Find duplicates of this image</source>
        <translation>Buscar duplicatas desta imagem</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1443" />
        <source>Use as reference image</source>
        <translation>Usar como imagem de referência</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1455" />
        <source>Only for files in the working tree</source>
        <translation>Apenas para arquivos da árvore de trabalho</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1460" />
        <source>Copy full path</source>
        <translation>Copiar caminho completo</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1461" />
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
        <location filename="../src/gui/ResultModel.cpp" line="159" />
        <source>distance %1</source>
        <translation>distância %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>scanning</source>
        <translation>varredura</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>indexing</source>
        <translation>indexação</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>descriptors</source>
        <translation>descritores</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>pruning</source>
        <translation>limpeza</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>loading</source>
        <translation>carregando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="95" />
        <source>matching</source>
        <translation>comparando</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="96" />
        <source>collecting</source>
        <translation>coletando</translation>
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