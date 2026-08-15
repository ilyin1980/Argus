<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>BranchDialog</name>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="21" />
        <source>Branches to index</source>
        <translation>Dizinlenecek dallar</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="27" />
        <source>Files are read straight out of the repository, so nothing is checked out and the working tree is never touched. The branch you have checked out is not listed: it is the working tree, which the index already covers.</source>
        <translation>Dosyalar doğrudan depodan okunur; hiçbir şey checkout edilmez ve çalışma ağacına dokunulmaz. Üzerinde bulunduğunuz dal listede yoktur: o zaten çalışma ağacıdır ve dizin onu kapsar.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="39" />
        <source>Include remote-tracking branches</source>
        <translation>Uzak izleme dallarını da göster</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="50" />
        <source>Select none</source>
        <translation>Hiçbirini seçme</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="87" />
        <source>This repository has no branches.</source>
        <translation>Bu deponun hiç dalı yok.</translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="88" />
        <source>Cannot list branches: %1</source>
        <translation>Dallar listelenemiyor: %1</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/BranchDialog.cpp" line="100" />
        <source>%n branch(es) available.</source>
        <translation><numerusform>%n dal kullanılabilir.</numerusform><numerusform>%n dal kullanılabilir.</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/BranchDialog.cpp" line="103" />
        <source>Indexed earlier but gone from the repository: %1. Pressing OK drops them from the index.</source>
        <translation>Daha önce dizinlenmişti ama depoda yok: %1. Tamam'a basmak bunları dizinden çıkarır.</translation>
    </message>
</context>
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="18" />
        <location filename="../src/gui/HelpDialog.cpp" line="85" />
        <source>Argus — Help</source>
        <translation>Argus — Yardım</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="57" />
        <source>Using Argus</source>
        <translation>Argus nasıl kullanılır</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="58" />
        <source>Command line</source>
        <translation>Komut satırı</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="75" />
        <source>Could not load %1.</source>
        <translation>%1 yüklenemedi.</translation>
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
        <translation>Görseller:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>png / jpg dosyalarının bulunduğu klasör</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="301" />
        <source>Browse…</source>
        <translation>Gözat…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="75" />
        <source>&amp;Index</source>
        <translation>&amp;Dizinle</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="91" />
        <source>Cancel</source>
        <translation>İptal</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="98" />
        <source>Index in:</source>
        <translation>Dizin konumu:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .argus inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>Dizin veritabanını, önizlemeleri ve betimleyicileri tutan klasör.
Varsayılan olarak görsel klasörünün içindeki .argus; salt okunur
ya da ağ paylaşımına dokunmamak veya dizini daha hızlı bir diske koymak
için başka bir yer seçin.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>Dizini yeniden görsel klasörünün içine koy.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>Varsayılan</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let Argus locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>Argus'ın bir varlığı ekran görüntüsü içinde bulmasını sağlayan
sinirsel betimleyicileri de oluştur. Dizinleme süresini kabaca iki katına
çıkarır ve görsel başına yaklaşık 40 KB ekler; bunlar olmadan yalnızca
kopya bulma ve tüm görsel benzerliği çalışır.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>Nesne arama verisi</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="151" />
        <source>Pick which git branches to index alongside the working tree.
Enabled only when the image folder is inside a git repository.</source>
        <translation>Çalışma ağacının yanında hangi git dallarının dizinleneceğini seçin.
Yalnızca görsel klasörü bir git deposunun içindeyken kullanılabilir.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="155" />
        <source>Branches…</source>
        <translation>Dallar…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="182" />
        <source>Duplicates</source>
        <translation>Kopyalar</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="190" />
        <location filename="../src/gui/MainWindow.ui" line="359" />
        <source>Max distance:</source>
        <translation>Azami uzaklık:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="197" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>64 bitlik algısal özet üzerinde Hamming yarıçapı.
0 = aynı parmak izleri, 4 = yeniden kodlama ve ölçeklemeye toleranslı.
8'in üzerinde sonuçlar silmek için değil, gözden geçirmek içindir: algısal özet
yerleşimi betimler, bu yüzden farklı ifadelere sahip aynı karakter tek grupta toplanır.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="213" />
        <source>Exact</source>
        <translation>Birebir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="223" />
        <source>Near</source>
        <translation>Yakın</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="233" />
        <source>&amp;Find duplicates</source>
        <translation>&amp;Kopyaları bul</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="272" />
        <source>Find by example</source>
        <translation>Örnekle arama</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>Örnek görsel: dosya seçin, sürükleyin ya da Ctrl+V ile yapıştırın</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="308" />
        <source>Paste</source>
        <translation>Yapıştır</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="317" />
        <location filename="../src/gui/MainWindow.cpp" line="428" />
        <location filename="../src/gui/MainWindow.cpp" line="1029" />
        <source>No reference image yet.</source>
        <translation>Henüz örnek görsel yok.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="329" />
        <source>Method:</source>
        <translation>Yöntem:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="339" />
        <source>Top:</source>
        <translation>En iyi:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="366" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>Birleşik pHash + dHash uzaklığı, 0..128.
Algısal özet, benzer bir konuyu değil, aynı resmi bulur.
Yaklaşık 16'nın altında sonuçlar yeniden kodlanmış ya da ölçeklenmiş aynı resimdir.
30'un üzerinde puan ne derse desin sonuçlar fiilen rastgeledir.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="382" />
        <source>&amp;Search</source>
        <translation>&amp;Ara</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="430" />
        <source>Path:</source>
        <translation>Yol:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="440" />
        <source>Select a result to see its full path</source>
        <translation>Tam yolu görmek için bir sonuç seçin</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="457" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>Seçili her sonucun tam yolunu satır satır kopyala.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="460" />
        <source>&amp;Copy</source>
        <translation>K&amp;opyala</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="128" />
        <location filename="../src/gui/MainWindow.cpp" line="645" />
        <source>Choose a folder to begin.</source>
        <translation>Başlamak için bir klasör seçin.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="133" />
        <source>Paste screenshot</source>
        <translation>Ekran görüntüsü yapıştır</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="234" />
        <location filename="../src/gui/MainWindow.cpp" line="430" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>Görselin tamamında aranıyor. Tek bir nesne aramak için bir çerçeve çizin.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="239" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>Seçili %1 x %2 bölgede aranıyor — tüm kareye göre daha hızlı ve daha isabetli.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>Panodan ekran görüntüsü yapıştır (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="412" />
        <source>Locate object in the image</source>
        <translation>Görseldeki nesneyi bul</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="254" />
        <location filename="../src/gui/MainWindow.cpp" line="413" />
        <source>Whole-image similarity</source>
        <translation>Tüm görsel benzerliği</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="257" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>Sinirsel yerel öznitelikler: görselde gerçekten bulunan bir varlığı,
küçük, ölçeklenmiş ya da karmaşa içinde olsa bile bulur ve yerini gösterir.
Betimleyiciler ve eğitilmiş bir sözlük içeren bir dizin gerektirir.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="263" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>Algısal özet: resmi bütün olarak karşılaştırır.
Aynı resmin ölçeklenmiş ya da yeniden kodlanmış kopyaları için iyidir,
ekran görüntüsü içindeki küçük bir nesneyi bulmak için işe yaramaz.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="294" />
        <source>&amp;View</source>
        <translation>&amp;Görünüm</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="295" />
        <source>&amp;Theme</source>
        <translation>&amp;Tema</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="296" />
        <source>&amp;Language</source>
        <translation>&amp;Dil</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="302" />
        <source>&amp;Help</source>
        <translation>&amp;Yardım</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="304" />
        <source>&amp;User guide</source>
        <translation>&amp;Kullanım kılavuzu</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="309" />
        <source>&amp;Command line and automation</source>
        <translation>&amp;Komut satırı ve otomasyon</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="314" />
        <source>&amp;About</source>
        <translation>&amp;Hakkında</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="316" />
        <source>About Argus</source>
        <translation>Argus hakkında</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="317" />
        <source>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;argus-gui&lt;/b&gt; and the headless &lt;b&gt;argus&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Argus %1&lt;/h3&gt;&lt;p&gt;Bir ekran görüntüsünde hangi görsel varlıkların, nerede göründüğünü bulur; bir klasördeki kopyaları da gruplar.&lt;/p&gt;&lt;p&gt;Tek bir çekirdek kitaplıktan üretilen iki program olarak dağıtılır: &lt;b&gt;argus-gui&lt;/b&gt; ve komut satırı sürümü &lt;b&gt;argus&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Kılavuz için &lt;b&gt;F1&lt;/b&gt;'e basın.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="358" />
        <source>Theme: %1</source>
        <translation>Tema: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="377" />
        <source>System language</source>
        <translation>Sistem dili</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="388" />
        <source>No translation for %1 in this build.</source>
        <translation>Bu sürümde %1 için çeviri yok.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="391" />
        <source>Language: %1</source>
        <translation>Dil: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="471" />
        <source>Copied %1</source>
        <translation>%1 kopyalandı</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="501" />
        <source>+%n more</source>
        <translation>
            <numerusform>+%n tane daha</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="527" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation>
            <numerusform>%n tam yol panoya kopyalandı.</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="528" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation>
            <numerusform>%n göreli yol panoya kopyalandı.</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="557" />
        <source>Choose where to keep the index</source>
        <translation>Dizinin tutulacağı yeri seçin</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="588" />
        <source>Choose a folder</source>
        <translation>Bir klasör seçin</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="651" />
        <source>Choose where the index should be kept.</source>
        <translation>Dizinin nerede tutulacağını seçin.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="656" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>%1 konumunda dizin yok — oluşturmak için «Dizinle»ye basın.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="664" />
        <source>Cannot open index: %1</source>
        <translation>Dizin açılamıyor: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="678" />
        <source>%1 indexed files, %2%3</source>
        <translation>%1 dizinlenmiş dosya, %2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="683" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — betimleyici yok, nesne konumlandırma kullanılamıyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="697" />
        <source>Choose an existing folder first.</source>
        <translation>Önce var olan bir klasör seçin.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="710" />
        <source>Cannot create the index directory:
%1</source>
        <translation>Dizin klasörü oluşturulamıyor:
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="732" />
        <source>Indexing</source>
        <translation>Dizinleme</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="769" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>Dizinlenen %1, değişmeyen %2, başarısız %3, kaldırılan %4 — %5 sn%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="764" />
        <location filename="../src/gui/MainWindow.cpp" line="775" />
        <location filename="../src/gui/MainWindow.cpp" line="977" />
        <source> (cancelled)</source>
        <translation> (iptal edildi)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Only the working tree will be indexed.</source>
        <translation>Yalnızca çalışma ağacı dizinlenecek.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="623" />
        <source>Branches to index: %1. Press Index to read them.</source>
        <translation>Dizinlenecek dallar: %1. Okumak için «Dizinle»ye basın.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="757" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6; branches: %7 read, %8 unchanged</source>
        <translation>Dizinlenen %1, değişmeyen %2, başarısız %3, kaldırılan %4 — %5 sn%6; dallar: %7 okundu, %8 değişmedi</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>Training the visual vocabulary</source>
        <translation>Görsel sözlük eğitiliyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="844" />
        <source>No descriptors to train on.</source>
        <translation>Eğitilecek betimleyici yok.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="854" />
        <source>Vocabulary failed: %1</source>
        <translation>Sözlük başarısız: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="864" />
        <source>Shortlist index failed: %1</source>
        <translation>Ön eleme dizini başarısız: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="875" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>Hazır: %1 görsel nesneye göre aranabilir, %2 sözcüklük sözlük.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="889" />
        <source>Cancelling…</source>
        <translation>İptal ediliyor…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="902" />
        <location filename="../src/gui/MainWindow.cpp" line="1113" />
        <location filename="../src/gui/MainWindow.cpp" line="1338" />
        <source>Build an index for this folder first.</source>
        <translation>Önce bu klasör için bir dizin oluşturun.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="913" />
        <source>Matching</source>
        <translation>Eşleştirme</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="960" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  %3 dosya  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="963" />
        <source>exact</source>
        <translation>birebir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="964" />
        <source>near d≤%1</source>
        <translation>yakın d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="969" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1 grup, %2 geri kazanılabilir</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="973" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — %1 aşırı büyük özet kovası atlandı</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1000" />
        <source>Choose a reference image</source>
        <translation>Bir örnek görsel seçin</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1001" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>Görseller (*.png *.jpg *.jpeg);;Tüm dosyalar (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1017" />
        <source>Cannot read %1: %2</source>
        <translation>%1 okunamıyor: %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1047" />
        <source>The clipboard holds no image.</source>
        <translation>Panoda görsel yok.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1051" />
        <source>(clipboard, %1 x %2)</source>
        <translation>(pano, %1 x %2)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1055" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>%1 x %2 ekran görüntüsü yapıştırıldı — nesnenin çevresine bir çerçeve çizin.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1082" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>Bu dizinde henüz betimleyici ve sözlük yok. Şöyle oluşturun:
  argus index &lt;dir&gt; --db &lt;db&gt; --features
  argus vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1097" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>Bu dizinde nesne arama verisi yok — «Nesne arama verisi» işaretliyken «Dizinle»ye basın. O zamana dek yalnızca tüm görsel benzerliği kullanılabilir; o da bir resmin içindeki nesneyi bulamaz.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1119" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</source>
        <translation>Bu dizinde henüz yerel öznitelik betimleyicileri yok, bu yüzden bir görselin içindeki nesne bulunamaz.

Önce bunları oluşturun:

  argus index "%1" --db "%2" --features
  argus vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1132" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>Önce Ctrl+V ile bir ekran görüntüsü yapıştırın, bir görsel sürükleyin ya da bir dosya seçin.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1158" />
        <source>Locating</source>
        <translation>Nesne aranıyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1217" />
        <source>Searching</source>
        <translation>Aranıyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1255" />
        <source>distance</source>
        <translation>uzaklık</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1263" />
        <source>the same picture</source>
        <translation>aynı resim</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1265" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>büyük olasılıkla aynı resim, yeniden kodlanmış ya da ölçeklenmiş</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1267" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>inandırıcı bir şey yok — puanlar ne derse desin bunlar birbirinden çok uzak</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1270" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>%2 dizinlenmiş dosyadan %1 sonuç, en küçük uzaklık %3: %4. Bu, nesne konumlandırma değil, tüm görsel benzerliğidir.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1294" />
        <source>inliers</source>
        <translation>iç nokta</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1299" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>Hiçbir şey bulunamadı. Nesneyi çerçeveleyin ya da varlığın gerçekten dizinlenmiş olduğunu doğrulayın.</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1315" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation>
            <numerusform>%n varlık bulundu; en iyisi: %1 (%2 iç nokta, %%3 tutarlı)</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1351" />
        <source>Looking for copies of %1…</source>
        <translation>%1 kopyaları aranıyor…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1367" />
        <source>%1 lives in branch %2, and this folder is no longer a git repository.</source>
        <translation>%1, %2 dalında bulunuyor ve bu klasör artık bir git deposu değil.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1376" />
        <source>Cannot read %1 from branch %2: %3</source>
        <translation>%1, %2 dalından okunamıyor: %3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1394" />
        <source>Cannot write a temporary copy of %1.</source>
        <translation>%1 için geçici bir kopya yazılamıyor.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1402" />
        <source>Opened %1 from branch %2.</source>
        <translation>%1, %2 dalından açıldı.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1430" />
        <source>Open</source>
        <translation>Aç</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1435" />
        <source>Reveal in file manager</source>
        <translation>Dosya yöneticisinde göster</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1441" />
        <source>Find duplicates of this image</source>
        <translation>Bu görselin kopyalarını bul</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1443" />
        <source>Use as reference image</source>
        <translation>Örnek görsel olarak kullan</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1455" />
        <source>Only for files in the working tree</source>
        <translation>Yalnızca çalışma ağacındaki dosyalar için</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1460" />
        <source>Copy full path</source>
        <translation>Tam yolu kopyala</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1461" />
        <source>Copy path relative to the root</source>
        <translation>Kök klasöre göreli yolu kopyala</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>Ekran görüntüsü yapıştırmak için Ctrl+V,
ya da buraya bir görsel sürükleyin</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="159" />
        <source>distance %1</source>
        <translation>uzaklık %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>scanning</source>
        <translation>taranıyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>indexing</source>
        <translation>dizinleniyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>descriptors</source>
        <translation>betimleyiciler</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>pruning</source>
        <translation>temizleniyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>loading</source>
        <translation>yükleniyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="95" />
        <source>matching</source>
        <translation>eşleştiriliyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="96" />
        <source>collecting</source>
        <translation>toplanıyor</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="97" />
        <source>verifying</source>
        <translation>doğrulanıyor</translation>
    </message>
</context>
<context>
    <name>theme</name>
    <message>
        <location filename="../src/gui/Theme.cpp" line="228" />
        <source>System</source>
        <translation>Sistem</translation>
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
        <translation>Çiçek</translation>
    </message>
</context>
</TS>