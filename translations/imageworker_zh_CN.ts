<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="13" />
        <source>ImageWorker — Help</source>
        <translation>ImageWorker — 帮助</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="25" />
        <source>Using ImageWorker</source>
        <translation>使用说明</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="26" />
        <source>Command line</source>
        <translation>命令行</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="42" />
        <source>Could not load %1.</source>
        <translation>无法加载 %1。</translation>
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
        <translation>图片：</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>存放 png / jpg 文件的目录</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="287" />
        <source>Browse…</source>
        <translation>浏览…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="75" />
        <source>&amp;Index</source>
        <translation>建立索引(&amp;I)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="91" />
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="98" />
        <source>Index in:</source>
        <translation>索引位置：</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .imageworker inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>存放索引数据库、预览图和描述子的目录。
默认是图片目录内的 .imageworker；如果不想改动只读目录或网络共享，
或者想把索引放到更快的磁盘上，可以另选一个位置。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>把索引放回图片目录内。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>默认</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let ImageWorker locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>同时生成神经网络描述子，用于在截图中定位素材。
索引时间大约翻倍，每张图片多占约 40 KB；没有它们时
只能使用重复图片检测和整图相似度。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>对象搜索数据</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="168" />
        <source>Duplicates</source>
        <translation>重复图片</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="176" />
        <location filename="../src/gui/MainWindow.ui" line="345" />
        <source>Max distance:</source>
        <translation>最大距离：</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="183" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>64 位感知哈希的汉明半径。
0 = 指纹完全相同，4 = 可容忍重新编码和缩放。
超过 8 时结果只适合人工复核，不适合直接删除：感知哈希描述的是构图，
因此同一角色的多种表情会被归为一组。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="199" />
        <source>Exact</source>
        <translation>完全相同</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="209" />
        <source>Near</source>
        <translation>相近</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="219" />
        <source>&amp;Find duplicates</source>
        <translation>查找重复(&amp;F)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="258" />
        <source>Find by example</source>
        <translation>按样例查找</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="280" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>参考图片：选择文件、拖入，或用 Ctrl+V 粘贴</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Paste</source>
        <translation>粘贴</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="303" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <location filename="../src/gui/MainWindow.cpp" line="966" />
        <source>No reference image yet.</source>
        <translation>尚未选择参考图片。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="315" />
        <source>Method:</source>
        <translation>方式：</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="325" />
        <source>Top:</source>
        <translation>最多：</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="352" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>pHash + dHash 的合并距离，0..128。
感知哈希找的是同一张图片，而不只是题材相近的图片。
大约 16 以下的结果是同一张图片，只是重新编码或缩放过。
超过 30 基本就是随机结果，无论分数看起来多高。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="368" />
        <source>&amp;Search</source>
        <translation>搜索(&amp;S)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="416" />
        <source>Path:</source>
        <translation>路径：</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="426" />
        <source>Select a result to see its full path</source>
        <translation>选择一个结果以查看其完整路径</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="443" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>复制所选每个结果的完整路径，每行一个。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="446" />
        <source>&amp;Copy</source>
        <translation>复制(&amp;C)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="125" />
        <location filename="../src/gui/MainWindow.cpp" line="603" />
        <source>Choose a folder to begin.</source>
        <translation>请选择一个目录以开始。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="130" />
        <source>Paste screenshot</source>
        <translation>粘贴截图</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="230" />
        <location filename="../src/gui/MainWindow.cpp" line="424" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>正在搜索整张图片。拖出一个框可只搜索其中一个对象。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="235" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>正在搜索所选的 %1 x %2 区域 —— 比整幅画面更快也更准确。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="245" />
        <location filename="../src/gui/MainWindow.cpp" line="404" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>从剪贴板粘贴截图（Ctrl+V）</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="406" />
        <source>Locate object in the image</source>
        <translation>在图片中定位对象</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="250" />
        <location filename="../src/gui/MainWindow.cpp" line="407" />
        <source>Whole-image similarity</source>
        <translation>整图相似度</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>神经网络局部特征：能找出真实出现在图片中的素材，
即使很小、被缩放或淹没在杂乱背景中，并指出具体位置。
需要包含描述子并已训练视觉词典的索引。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="259" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>感知哈希：把图片作为整体来比较。
适合找同一张图片被缩放或重新编码后的副本，
无法在截图中找出其中的小对象。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="290" />
        <source>&amp;View</source>
        <translation>视图(&amp;V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="291" />
        <source>&amp;Theme</source>
        <translation>主题(&amp;T)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="292" />
        <source>&amp;Language</source>
        <translation>语言(&amp;L)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="298" />
        <source>&amp;Help</source>
        <translation>帮助(&amp;H)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="300" />
        <source>&amp;User guide</source>
        <translation>使用手册(&amp;U)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="305" />
        <source>&amp;Command line and automation</source>
        <translation>命令行与自动化(&amp;C)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="310" />
        <source>&amp;About</source>
        <translation>关于(&amp;A)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="312" />
        <source>About ImageWorker</source>
        <translation>关于 ImageWorker</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="313" />
        <source>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;imageworker-gui&lt;/b&gt; and the headless &lt;b&gt;imageworker&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;找出截图中出现了哪些图片素材以及它们的位置；并把目录中的重复图片分组。&lt;/p&gt;&lt;p&gt;由同一个核心库构建出两个可执行文件：&lt;b&gt;imageworker-gui&lt;/b&gt; 和命令行版 &lt;b&gt;imageworker&lt;/b&gt;。&lt;/p&gt;&lt;p&gt;按 &lt;b&gt;F1&lt;/b&gt; 打开手册。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="354" />
        <source>Theme: %1</source>
        <translation>主题：%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="373" />
        <source>System language</source>
        <translation>跟随系统语言</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="384" />
        <source>No translation for %1 in this build.</source>
        <translation>此版本不包含 %1 的翻译。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="387" />
        <source>Language: %1</source>
        <translation>语言：%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="465" />
        <source>Copied %1</source>
        <translation>已复制 %1</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="495" />
        <source>+%n more</source>
        <translation><numerusform>另有 %n 个</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="521" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation><numerusform>已将 %n 条完整路径复制到剪贴板。</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="522" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation><numerusform>已将 %n 条相对路径复制到剪贴板。</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="551" />
        <source>Choose where to keep the index</source>
        <translation>选择索引的存放位置</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="582" />
        <source>Choose a folder</source>
        <translation>选择目录</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="609" />
        <source>Choose where the index should be kept.</source>
        <translation>请选择索引的存放位置。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="614" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>%1 中没有索引 —— 点击“建立索引”来创建。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Cannot open index: %1</source>
        <translation>无法打开索引：%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="633" />
        <source>%1 indexed files, %2%3</source>
        <translation>已索引 %1 个文件，%2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="638" />
        <source> — no descriptors, object location unavailable</source>
        <translation> —— 没有描述子，无法定位对象</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="652" />
        <source>Choose an existing folder first.</source>
        <translation>请先选择一个已存在的目录。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="665" />
        <source>Cannot create the index directory:
%1</source>
        <translation>无法创建索引目录：
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="682" />
        <source>Indexing</source>
        <translation>建立索引</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="706" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>已索引 %1，未变化 %2，失败 %3，已清除 %4 —— %5 秒%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="712" />
        <location filename="../src/gui/MainWindow.cpp" line="914" />
        <source> (cancelled)</source>
        <translation>（已取消）</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="718" />
        <source>Training the visual vocabulary</source>
        <translation>正在训练视觉词典</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>No descriptors to train on.</source>
        <translation>没有可用于训练的描述子。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="791" />
        <source>Vocabulary failed: %1</source>
        <translation>视觉词典失败：%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="801" />
        <source>Shortlist index failed: %1</source>
        <translation>候选索引失败：%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="812" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>已就绪：%1 张图片可按对象搜索，词典含 %2 个词。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="826" />
        <source>Cancelling…</source>
        <translation>正在取消…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="839" />
        <location filename="../src/gui/MainWindow.cpp" line="1050" />
        <location filename="../src/gui/MainWindow.cpp" line="1275" />
        <source>Build an index for this folder first.</source>
        <translation>请先为该目录建立索引。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="850" />
        <source>Matching</source>
        <translation>匹配中</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="897" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 个文件  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="900" />
        <source>exact</source>
        <translation>完全相同</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="901" />
        <source>near d≤%1</source>
        <translation>相近 d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="906" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 组，可释放 %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="910" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> —— 已跳过 %1 个过大的哈希桶</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="937" />
        <source>Choose a reference image</source>
        <translation>选择参考图片</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="938" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>图片 (*.png *.jpg *.jpeg);;所有文件 (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="954" />
        <source>Cannot read %1: %2</source>
        <translation>无法读取 %1：%2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="984" />
        <source>The clipboard holds no image.</source>
        <translation>剪贴板中没有图片。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="988" />
        <source>(clipboard, %1 x %2)</source>
        <translation>（剪贴板，%1 x %2）</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="992" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>已粘贴 %1 x %2 的截图 —— 在对象周围拖出一个框。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1019" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>该索引还没有描述子和视觉词典。请这样生成：
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1034" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>该索引没有对象搜索数据 —— 请勾选“对象搜索数据”后再点击“建立索引”。在此之前只能使用整图相似度，而它无法在图片内部找出某个对象。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1056" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</source>
        <translation>该索引还没有局部特征描述子，因此无法在图片内部定位对象。

请先生成它们：

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1069" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>请先用 Ctrl+V 粘贴截图、拖入图片，或选择一个文件。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1095" />
        <source>Locating</source>
        <translation>定位中</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1154" />
        <source>Searching</source>
        <translation>搜索中</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1192" />
        <source>distance</source>
        <translation>距离</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1200" />
        <source>the same picture</source>
        <translation>同一张图片</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1202" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>很可能是同一张图片，只是重新编码或缩放过</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1204" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>没有可信的结果 —— 无论分数如何，它们相差很远</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1207" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>在 %2 个已索引文件中找到 %1 个结果，最近距离 %3：%4。这是整图相似度，不是对象定位。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1231" />
        <source>inliers</source>
        <translation>内点</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1236" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>没有定位到任何内容。可以试着框出对象，或确认该素材确实已被索引。</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1252" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation><numerusform>定位到 %n 个素材；最佳：%1（%2 个内点，%3% 一致）</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1288" />
        <source>Looking for copies of %1…</source>
        <translation>正在查找 %1 的副本…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1320" />
        <source>Open</source>
        <translation>打开</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1323" />
        <source>Reveal in file manager</source>
        <translation>在文件管理器中显示</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1327" />
        <source>Find duplicates of this image</source>
        <translation>查找这张图片的重复项</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1329" />
        <source>Use as reference image</source>
        <translation>用作参考图片</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1334" />
        <source>Copy full path</source>
        <translation>复制完整路径</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1335" />
        <source>Copy path relative to the root</source>
        <translation>复制相对于根目录的路径</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>按 Ctrl+V 粘贴截图，
或把图片拖到这里</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="146" />
        <source>distance %1</source>
        <translation>距离 %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="87" />
        <source>scanning</source>
        <translation>扫描</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="88" />
        <source>indexing</source>
        <translation>索引</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="89" />
        <source>descriptors</source>
        <translation>描述子</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>pruning</source>
        <translation>清理</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>loading</source>
        <translation>载入</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>matching</source>
        <translation>匹配</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>collecting</source>
        <translation>汇总</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>verifying</source>
        <translation>校验</translation>
    </message>
</context>
<context>
    <name>theme</name>
    <message>
        <location filename="../src/gui/Theme.cpp" line="228" />
        <source>System</source>
        <translation>系统</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="229" />
        <source>Sci-fi</source>
        <translation>科幻</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="230" />
        <source>Material</source>
        <translation>Material</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="231" />
        <source>Blossom</source>
        <translation>花语</translation>
    </message>
</context>
</TS>