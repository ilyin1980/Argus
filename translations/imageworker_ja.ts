<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja">
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="13" />
        <source>ImageWorker — Help</source>
        <translation>ImageWorker — ヘルプ</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="25" />
        <source>Using ImageWorker</source>
        <translation>使い方</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="26" />
        <source>Command line</source>
        <translation>コマンドライン</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="42" />
        <source>Could not load %1.</source>
        <translation>%1 を読み込めませんでした。</translation>
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
        <translation>画像:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>png / jpg ファイルのあるフォルダー</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="287" />
        <source>Browse…</source>
        <translation>参照…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="75" />
        <source>&amp;Index</source>
        <translation>インデックス作成(&amp;I)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="91" />
        <source>Cancel</source>
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="98" />
        <source>Index in:</source>
        <translation>インデックスの場所:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .imageworker inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>インデックスのデータベース、プレビュー、記述子を置くフォルダーです。
既定では画像フォルダー内の .imageworker です。読み取り専用の場所や
ネットワーク共有に手を加えたくない場合、あるいは高速なディスクに
置きたい場合は別の場所を指定してください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>インデックスを画像フォルダー内に戻します。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>既定</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let ImageWorker locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>スクリーンショット内のアセットを見つけるためのニューラル記述子も
作成します。インデックス作成の時間はおよそ 2 倍になり、画像 1 枚あたり
約 40 KB 増えます。これがない場合は重複検出と画像全体の類似度だけが
使えます。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>オブジェクト検索用データ</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="168" />
        <source>Duplicates</source>
        <translation>重複</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="176" />
        <location filename="../src/gui/MainWindow.ui" line="345" />
        <source>Max distance:</source>
        <translation>最大距離:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="183" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>64 ビット知覚ハッシュ上のハミング半径です。
0 = 指紋が完全に一致、4 = 再エンコードや拡大縮小を許容。
8 を超えた結果は削除用ではなく確認用です。知覚ハッシュは構図を表すため、
表情違いの同じキャラクターが 1 つのグループにまとまります。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="199" />
        <source>Exact</source>
        <translation>完全一致</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="209" />
        <source>Near</source>
        <translation>近似</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="219" />
        <source>&amp;Find duplicates</source>
        <translation>重複を検索(&amp;F)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="258" />
        <source>Find by example</source>
        <translation>見本で検索</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="280" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>参照画像: ファイルを選ぶか、ドロップするか、Ctrl+V で貼り付けます</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Paste</source>
        <translation>貼り付け</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="303" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <location filename="../src/gui/MainWindow.cpp" line="966" />
        <source>No reference image yet.</source>
        <translation>参照画像がまだありません。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="315" />
        <source>Method:</source>
        <translation>方式:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="325" />
        <source>Top:</source>
        <translation>上位:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="352" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>pHash と dHash を合算した距離です（0..128）。
知覚ハッシュが見つけるのは似た題材ではなく、同じ画像そのものです。
おおよそ 16 以下なら、再エンコードや拡大縮小をした同じ画像です。
30 を超えると、スコアがどう見えても事実上ランダムです。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="368" />
        <source>&amp;Search</source>
        <translation>検索(&amp;S)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="416" />
        <source>Path:</source>
        <translation>パス:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="426" />
        <source>Select a result to see its full path</source>
        <translation>結果を選ぶとフルパスが表示されます</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="443" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>選択したすべての結果のフルパスを 1 行ずつコピーします。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="446" />
        <source>&amp;Copy</source>
        <translation>コピー(&amp;C)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="125" />
        <location filename="../src/gui/MainWindow.cpp" line="603" />
        <source>Choose a folder to begin.</source>
        <translation>まずフォルダーを選んでください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="130" />
        <source>Paste screenshot</source>
        <translation>スクリーンショットを貼り付け</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="230" />
        <location filename="../src/gui/MainWindow.cpp" line="424" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>画像全体を検索します。枠をドラッグすると 1 つのオブジェクトだけを検索できます。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="235" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>選択した %1 x %2 の領域を検索します。画面全体より速く、正確です。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="245" />
        <location filename="../src/gui/MainWindow.cpp" line="404" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>クリップボードからスクリーンショットを貼り付け (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="406" />
        <source>Locate object in the image</source>
        <translation>画像内のオブジェクトを特定</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="250" />
        <location filename="../src/gui/MainWindow.cpp" line="407" />
        <source>Whole-image similarity</source>
        <translation>画像全体の類似度</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>ニューラル局所特徴: 画像に実際に写っているアセットを、小さくても、
拡大縮小されていても、雑多な背景の中でも見つけ出し、位置を示します。
記述子と学習済みの視覚語彙を含むインデックスが必要です。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="259" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>知覚ハッシュ: 画像を全体として比較します。
同じ画像の拡大縮小版や再エンコード版を見つけるのに向きますが、
スクリーンショット内の小さなオブジェクトを探すのには使えません。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="290" />
        <source>&amp;View</source>
        <translation>表示(&amp;V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="291" />
        <source>&amp;Theme</source>
        <translation>テーマ(&amp;T)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="292" />
        <source>&amp;Language</source>
        <translation>言語(&amp;L)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="298" />
        <source>&amp;Help</source>
        <translation>ヘルプ(&amp;H)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="300" />
        <source>&amp;User guide</source>
        <translation>ユーザーガイド(&amp;U)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="305" />
        <source>&amp;Command line and automation</source>
        <translation>コマンドラインと自動化(&amp;C)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="310" />
        <source>&amp;About</source>
        <translation>バージョン情報(&amp;A)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="312" />
        <source>About ImageWorker</source>
        <translation>ImageWorker について</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="313" />
        <source>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;imageworker-gui&lt;/b&gt; and the headless &lt;b&gt;imageworker&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;スクリーンショットの中にどの画像アセットがどこに写っているかを見つけ、フォルダー内の重複をまとめます。&lt;/p&gt;&lt;p&gt;1 つのコアライブラリから作られた 2 つの実行ファイル、&lt;b&gt;imageworker-gui&lt;/b&gt; とコマンドライン版 &lt;b&gt;imageworker&lt;/b&gt; として提供されます。&lt;/p&gt;&lt;p&gt;&lt;b&gt;F1&lt;/b&gt; でマニュアルを開きます。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="354" />
        <source>Theme: %1</source>
        <translation>テーマ: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="373" />
        <source>System language</source>
        <translation>システムの言語</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="384" />
        <source>No translation for %1 in this build.</source>
        <translation>このビルドには %1 の翻訳がありません。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="387" />
        <source>Language: %1</source>
        <translation>言語: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="465" />
        <source>Copied %1</source>
        <translation>%1 をコピーしました</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="495" />
        <source>+%n more</source>
        <translation><numerusform>他 %n 件</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="521" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation><numerusform>フルパス %n 件をクリップボードにコピーしました。</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="522" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation><numerusform>相対パス %n 件をクリップボードにコピーしました。</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="551" />
        <source>Choose where to keep the index</source>
        <translation>インデックスの保存先を選択</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="582" />
        <source>Choose a folder</source>
        <translation>フォルダーを選択</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="609" />
        <source>Choose where the index should be kept.</source>
        <translation>インデックスの保存先を選んでください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="614" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>%1 にインデックスがありません。「インデックス作成」を押して作成してください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Cannot open index: %1</source>
        <translation>インデックスを開けません: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="633" />
        <source>%1 indexed files, %2%3</source>
        <translation>インデックス済み %1 ファイル、%2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="638" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — 記述子がないため、オブジェクトの特定は利用できません</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="652" />
        <source>Choose an existing folder first.</source>
        <translation>先に既存のフォルダーを選んでください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="665" />
        <source>Cannot create the index directory:
%1</source>
        <translation>インデックス用フォルダーを作成できません:
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="682" />
        <source>Indexing</source>
        <translation>インデックス作成</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="706" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>処理 %1、変更なし %2、失敗 %3、削除 %4 — %5 秒%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="712" />
        <location filename="../src/gui/MainWindow.cpp" line="914" />
        <source> (cancelled)</source>
        <translation>（キャンセル）</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="718" />
        <source>Training the visual vocabulary</source>
        <translation>視覚語彙を学習中</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>No descriptors to train on.</source>
        <translation>学習に使える記述子がありません。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="791" />
        <source>Vocabulary failed: %1</source>
        <translation>視覚語彙の作成に失敗: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="801" />
        <source>Shortlist index failed: %1</source>
        <translation>候補インデックスの作成に失敗: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="812" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>準備完了: %1 枚をオブジェクトで検索できます。語彙は %2 語です。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="826" />
        <source>Cancelling…</source>
        <translation>キャンセルしています…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="839" />
        <location filename="../src/gui/MainWindow.cpp" line="1050" />
        <location filename="../src/gui/MainWindow.cpp" line="1275" />
        <source>Build an index for this folder first.</source>
        <translation>先にこのフォルダーのインデックスを作成してください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="850" />
        <source>Matching</source>
        <translation>照合</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="897" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 ファイル  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="900" />
        <source>exact</source>
        <translation>完全一致</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="901" />
        <source>near d≤%1</source>
        <translation>近似 d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="906" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 グループ、%2 を解放できます</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="910" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — 大きすぎるハッシュバケット %1 個をスキップしました</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="937" />
        <source>Choose a reference image</source>
        <translation>参照画像を選択</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="938" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>画像 (*.png *.jpg *.jpeg);;すべてのファイル (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="954" />
        <source>Cannot read %1: %2</source>
        <translation>%1 を読み取れません: %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="984" />
        <source>The clipboard holds no image.</source>
        <translation>クリップボードに画像がありません。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="988" />
        <source>(clipboard, %1 x %2)</source>
        <translation>（クリップボード、%1 x %2）</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="992" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>%1 x %2 のスクリーンショットを貼り付けました。対象を枠で囲んでください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1019" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>このインデックスにはまだ記述子と視覚語彙がありません。次のコマンドで作成します:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1034" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>このインデックスにはオブジェクト検索用データがありません。「オブジェクト検索用データ」にチェックを入れて「インデックス作成」を押してください。それまでは画像全体の類似度しか使えず、画像の中のオブジェクトを見つけることはできません。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1056" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</source>
        <translation>このインデックスにはまだ局所特徴の記述子がないため、画像の中のオブジェクトを特定できません。

先に作成してください:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1069" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>先に Ctrl+V でスクリーンショットを貼り付けるか、画像をドロップするか、ファイルを選んでください。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1095" />
        <source>Locating</source>
        <translation>位置を特定中</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1154" />
        <source>Searching</source>
        <translation>検索中</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1192" />
        <source>distance</source>
        <translation>距離</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1200" />
        <source>the same picture</source>
        <translation>同じ画像です</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1202" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>おそらく同じ画像です（再エンコードまたは拡大縮小）</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1204" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>確かなものはありません。スコアがどうであれ、これらは大きく離れています</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1207" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>インデックス済み %2 ファイル中 %1 件。最小距離 %3: %4。これは画像全体の類似度であり、オブジェクトの特定ではありません。</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1231" />
        <source>inliers</source>
        <translation>インライア</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1236" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>何も見つかりませんでした。対象を枠で囲むか、そのアセットが実際にインデックスされているか確認してください。</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1252" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation><numerusform>アセットを %n 件検出。最良: %1（インライア %2、整合 %3%）</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1288" />
        <source>Looking for copies of %1…</source>
        <translation>%1 の複製を探しています…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1320" />
        <source>Open</source>
        <translation>開く</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1323" />
        <source>Reveal in file manager</source>
        <translation>ファイルマネージャーで表示</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1327" />
        <source>Find duplicates of this image</source>
        <translation>この画像の重複を検索</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1329" />
        <source>Use as reference image</source>
        <translation>参照画像として使う</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1334" />
        <source>Copy full path</source>
        <translation>フルパスをコピー</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1335" />
        <source>Copy path relative to the root</source>
        <translation>ルートからの相対パスをコピー</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>Ctrl+V でスクリーンショットを貼り付け、
またはここに画像をドロップ</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="146" />
        <source>distance %1</source>
        <translation>距離 %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="87" />
        <source>scanning</source>
        <translation>走査</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="88" />
        <source>indexing</source>
        <translation>インデックス作成</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="89" />
        <source>descriptors</source>
        <translation>記述子</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>pruning</source>
        <translation>整理</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>loading</source>
        <translation>読み込み</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>matching</source>
        <translation>照合</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>collecting</source>
        <translation>集計</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>verifying</source>
        <translation>検証</translation>
    </message>
</context>
<context>
    <name>theme</name>
    <message>
        <location filename="../src/gui/Theme.cpp" line="228" />
        <source>System</source>
        <translation>システム</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="229" />
        <source>Sci-fi</source>
        <translation>SF</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="230" />
        <source>Material</source>
        <translation>Material</translation>
    </message>
    <message>
        <location filename="../src/gui/Theme.cpp" line="231" />
        <source>Blossom</source>
        <translation>ブロッサム</translation>
    </message>
</context>
</TS>