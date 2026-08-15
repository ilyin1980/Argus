<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ko">
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="13" />
        <source>ImageWorker — Help</source>
        <translation>ImageWorker — 도움말</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="25" />
        <source>Using ImageWorker</source>
        <translation>사용 방법</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="26" />
        <source>Command line</source>
        <translation>명령줄</translation>
    </message>
    <message>
        <location filename="../src/gui/HelpDialog.cpp" line="42" />
        <source>Could not load %1.</source>
        <translation>%1을(를) 불러올 수 없습니다.</translation>
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
        <translation>이미지:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="61" />
        <source>Directory with png / jpg files</source>
        <translation>png / jpg 파일이 있는 폴더</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="68" />
        <location filename="../src/gui/MainWindow.ui" line="115" />
        <location filename="../src/gui/MainWindow.ui" line="287" />
        <source>Browse…</source>
        <translation>찾아보기…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="75" />
        <source>&amp;Index</source>
        <translation>색인 만들기(&amp;I)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="91" />
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="98" />
        <source>Index in:</source>
        <translation>색인 위치:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="105" />
        <source>Directory holding the index database, previews and descriptors.
Defaults to .imageworker inside the image folder; point it elsewhere to
keep a read-only or network share untouched, or to put the index on a
faster disk.</source>
        <translation>색인 데이터베이스와 미리보기, 서술자를 두는 폴더입니다.
기본값은 이미지 폴더 안의 .imageworker입니다. 읽기 전용 위치나
네트워크 공유를 건드리지 않으려면, 또는 더 빠른 디스크에 색인을
두려면 다른 위치를 지정하세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="122" />
        <source>Put the index back inside the image folder.</source>
        <translation>색인을 이미지 폴더 안으로 되돌립니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="125" />
        <source>Default</source>
        <translation>기본값</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="132" />
        <source>Also build the neural descriptors that let ImageWorker locate an
asset inside a screenshot. Roughly doubles indexing time and adds
about 40 KB per image; without them only duplicate detection and
whole-image similarity work.</source>
        <translation>스크린샷 안에서 에셋을 찾아내는 데 쓰이는 신경망 서술자도 함께
만듭니다. 색인 시간이 대략 두 배가 되고 이미지당 약 40 KB가 늘어납니다.
이것이 없으면 중복 검색과 전체 이미지 유사도만 사용할 수 있습니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="138" />
        <source>Object search data</source>
        <translation>객체 검색 데이터</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="168" />
        <source>Duplicates</source>
        <translation>중복</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="176" />
        <location filename="../src/gui/MainWindow.ui" line="345" />
        <source>Max distance:</source>
        <translation>최대 거리:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="183" />
        <source>Hamming radius on the 64-bit perceptual hash.
0 = identical fingerprints, 4 = tolerant of re-encoding and rescaling.
Above 8 the results are for review, not for deletion: perceptual hashing
describes layout, so one character with several expressions groups together.</source>
        <translation>64비트 지각 해시의 해밍 반경입니다.
0 = 지문이 완전히 동일, 4 = 재인코딩과 크기 변경을 허용.
8을 넘으면 결과는 삭제용이 아니라 검토용입니다. 지각 해시는 구도를
나타내므로 표정만 다른 같은 캐릭터가 한 묶음이 됩니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="199" />
        <source>Exact</source>
        <translation>완전 일치</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="209" />
        <source>Near</source>
        <translation>유사</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="219" />
        <source>&amp;Find duplicates</source>
        <translation>중복 찾기(&amp;F)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="258" />
        <source>Find by example</source>
        <translation>예시로 찾기</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="280" />
        <source>Reference image, drop one, or paste with Ctrl+V</source>
        <translation>기준 이미지: 파일을 고르거나 끌어다 놓거나 Ctrl+V로 붙여넣으세요</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="294" />
        <source>Paste</source>
        <translation>붙여넣기</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="303" />
        <location filename="../src/gui/MainWindow.cpp" line="422" />
        <location filename="../src/gui/MainWindow.cpp" line="966" />
        <source>No reference image yet.</source>
        <translation>아직 기준 이미지가 없습니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="315" />
        <source>Method:</source>
        <translation>방식:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="325" />
        <source>Top:</source>
        <translation>상위:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="352" />
        <source>Combined pHash + dHash distance, 0..128.
Perceptual hashing finds the same picture, not merely a similar subject.
Below about 16 the hits are the same picture re-encoded or rescaled.
Above 30 they are effectively random, whatever the score suggests.</source>
        <translation>pHash와 dHash를 합한 거리입니다(0..128).
지각 해시는 비슷한 소재가 아니라 같은 그림을 찾습니다.
대략 16 이하면 재인코딩하거나 크기를 바꾼 같은 그림입니다.
30을 넘으면 점수가 어떻게 보이든 사실상 무작위입니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="368" />
        <source>&amp;Search</source>
        <translation>검색(&amp;S)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="416" />
        <source>Path:</source>
        <translation>경로:</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="426" />
        <source>Select a result to see its full path</source>
        <translation>결과를 선택하면 전체 경로가 표시됩니다</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="443" />
        <source>Copy the full path of every selected result, one per line.</source>
        <translation>선택한 모든 결과의 전체 경로를 한 줄에 하나씩 복사합니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.ui" line="446" />
        <source>&amp;Copy</source>
        <translation>복사(&amp;C)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="125" />
        <location filename="../src/gui/MainWindow.cpp" line="603" />
        <source>Choose a folder to begin.</source>
        <translation>먼저 폴더를 선택하세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="130" />
        <source>Paste screenshot</source>
        <translation>스크린샷 붙여넣기</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="230" />
        <location filename="../src/gui/MainWindow.cpp" line="424" />
        <source>Searching the whole image. Drag a box to search for one object.</source>
        <translation>이미지 전체를 검색합니다. 상자를 그리면 객체 하나만 검색합니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="235" />
        <source>Searching the selected %1 x %2 region — faster and more accurate than the whole frame.</source>
        <translation>선택한 %1 x %2 영역을 검색합니다. 전체 화면보다 빠르고 정확합니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="245" />
        <location filename="../src/gui/MainWindow.cpp" line="404" />
        <source>Paste a screenshot from the clipboard (Ctrl+V)</source>
        <translation>클립보드에서 스크린샷 붙여넣기 (Ctrl+V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="249" />
        <location filename="../src/gui/MainWindow.cpp" line="406" />
        <source>Locate object in the image</source>
        <translation>이미지에서 객체 찾기</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="250" />
        <location filename="../src/gui/MainWindow.cpp" line="407" />
        <source>Whole-image similarity</source>
        <translation>전체 이미지 유사도</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="253" />
        <location filename="../src/gui/MainWindow.cpp" line="410" />
        <source>Neural local features: finds an asset that is physically present in the
image, even small, rescaled or surrounded by clutter, and shows where.
Needs an index built with descriptors and a trained vocabulary.</source>
        <translation>신경망 지역 특징: 이미지에 실제로 들어 있는 에셋을 작아도, 크기가
바뀌었어도, 잡다한 배경에 묻혀 있어도 찾아내고 위치를 보여 줍니다.
서술자와 학습된 시각 어휘가 포함된 색인이 필요합니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="259" />
        <location filename="../src/gui/MainWindow.cpp" line="416" />
        <source>Perceptual hash: compares the picture as a whole.
Good for rescaled or re-encoded copies of the same picture,
useless for finding a small object inside a screenshot.</source>
        <translation>지각 해시: 그림을 전체로 비교합니다.
같은 그림의 크기 변경본이나 재인코딩본을 찾는 데 좋지만,
스크린샷 안의 작은 객체를 찾는 데는 쓸 수 없습니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="290" />
        <source>&amp;View</source>
        <translation>보기(&amp;V)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="291" />
        <source>&amp;Theme</source>
        <translation>테마(&amp;T)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="292" />
        <source>&amp;Language</source>
        <translation>언어(&amp;L)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="298" />
        <source>&amp;Help</source>
        <translation>도움말(&amp;H)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="300" />
        <source>&amp;User guide</source>
        <translation>사용 설명서(&amp;U)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="305" />
        <source>&amp;Command line and automation</source>
        <translation>명령줄과 자동화(&amp;C)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="310" />
        <source>&amp;About</source>
        <translation>정보(&amp;A)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="312" />
        <source>About ImageWorker</source>
        <translation>ImageWorker 정보</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="313" />
        <source>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;Finds which image assets appear inside a screenshot, and where; and groups duplicates in a folder.&lt;/p&gt;&lt;p&gt;Ships as two executables built from one core library: &lt;b&gt;imageworker-gui&lt;/b&gt; and the headless &lt;b&gt;imageworker&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;F1&lt;/b&gt; for the manual.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;ImageWorker %1&lt;/h3&gt;&lt;p&gt;스크린샷 안에 어떤 이미지 에셋이 어디에 나타나는지 찾아내고, 폴더 안의 중복을 묶어 줍니다.&lt;/p&gt;&lt;p&gt;하나의 코어 라이브러리로 만든 두 개의 실행 파일로 배포됩니다: &lt;b&gt;imageworker-gui&lt;/b&gt;와 명령줄용 &lt;b&gt;imageworker&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;&lt;b&gt;F1&lt;/b&gt;을 누르면 설명서가 열립니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="354" />
        <source>Theme: %1</source>
        <translation>테마: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="373" />
        <source>System language</source>
        <translation>시스템 언어</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="384" />
        <source>No translation for %1 in this build.</source>
        <translation>이 빌드에는 %1 번역이 없습니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="387" />
        <source>Language: %1</source>
        <translation>언어: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="465" />
        <source>Copied %1</source>
        <translation>%1을(를) 복사했습니다</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="495" />
        <source>+%n more</source>
        <translation><numerusform>외 %n개</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="521" />
        <source>Copied %n full path(s) to the clipboard.</source>
        <translation><numerusform>전체 경로 %n개를 클립보드에 복사했습니다.</numerusform></translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="522" />
        <source>Copied %n relative path(s) to the clipboard.</source>
        <translation><numerusform>상대 경로 %n개를 클립보드에 복사했습니다.</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="551" />
        <source>Choose where to keep the index</source>
        <translation>색인을 둘 위치 선택</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="582" />
        <source>Choose a folder</source>
        <translation>폴더 선택</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="609" />
        <source>Choose where the index should be kept.</source>
        <translation>색인을 둘 위치를 선택하세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="614" />
        <source>No index at %1 — press Index to build one.</source>
        <translation>%1에 색인이 없습니다. 「색인 만들기」를 눌러 만드세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="622" />
        <source>Cannot open index: %1</source>
        <translation>색인을 열 수 없습니다: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="633" />
        <source>%1 indexed files, %2%3</source>
        <translation>색인된 파일 %1개, %2%3</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="638" />
        <source> — no descriptors, object location unavailable</source>
        <translation> — 서술자가 없어 객체 위치 찾기를 쓸 수 없습니다</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="652" />
        <source>Choose an existing folder first.</source>
        <translation>먼저 존재하는 폴더를 선택하세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="665" />
        <source>Cannot create the index directory:
%1</source>
        <translation>색인 폴더를 만들 수 없습니다:
%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="682" />
        <source>Indexing</source>
        <translation>색인 만드는 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="706" />
        <source>Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6</source>
        <translation>색인 %1, 변경 없음 %2, 실패 %3, 제거 %4 — %5초%6</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="712" />
        <location filename="../src/gui/MainWindow.cpp" line="914" />
        <source> (cancelled)</source>
        <translation> (취소됨)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="718" />
        <source>Training the visual vocabulary</source>
        <translation>시각 어휘 학습 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="781" />
        <source>No descriptors to train on.</source>
        <translation>학습할 서술자가 없습니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="791" />
        <source>Vocabulary failed: %1</source>
        <translation>시각 어휘 생성 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="801" />
        <source>Shortlist index failed: %1</source>
        <translation>후보 색인 생성 실패: %1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="812" />
        <source>Ready: %1 images searchable by object, %2-word vocabulary.</source>
        <translation>준비됨: 이미지 %1개를 객체로 검색할 수 있습니다. 어휘는 %2개 단어입니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="826" />
        <source>Cancelling…</source>
        <translation>취소하는 중…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="839" />
        <location filename="../src/gui/MainWindow.cpp" line="1050" />
        <location filename="../src/gui/MainWindow.cpp" line="1275" />
        <source>Build an index for this folder first.</source>
        <translation>먼저 이 폴더의 색인을 만드세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="850" />
        <source>Matching</source>
        <translation>대조 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="897" />
        <source>#%1  %2  %3 files  %4</source>
        <translation>#%1  %2  파일 %3개  %4</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="900" />
        <source>exact</source>
        <translation>완전 일치</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="901" />
        <source>near d≤%1</source>
        <translation>유사 d≤%1</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="906" />
        <source>%1 groups, %2 reclaimable</source>
        <translation>%1개 묶음, %2 회수 가능</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="910" />
        <source> — %1 oversized hash buckets skipped</source>
        <translation> — 너무 큰 해시 버킷 %1개를 건너뛰었습니다</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="937" />
        <source>Choose a reference image</source>
        <translation>기준 이미지 선택</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="938" />
        <source>Images (*.png *.jpg *.jpeg);;All files (*)</source>
        <translation>이미지 (*.png *.jpg *.jpeg);;모든 파일 (*)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="954" />
        <source>Cannot read %1: %2</source>
        <translation>%1을(를) 읽을 수 없습니다: %2</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="984" />
        <source>The clipboard holds no image.</source>
        <translation>클립보드에 이미지가 없습니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="988" />
        <source>(clipboard, %1 x %2)</source>
        <translation>(클립보드, %1 x %2)</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="992" />
        <source>Pasted a %1 x %2 screenshot — drag a box around the object.</source>
        <translation>%1 x %2 스크린샷을 붙여넣었습니다. 객체 주위에 상자를 그리세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1019" />
        <source>This index has no descriptors or vocabulary yet. Build them with:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</source>
        <translation>이 색인에는 아직 서술자와 시각 어휘가 없습니다. 다음으로 만드세요:
  imageworker index &lt;dir&gt; --db &lt;db&gt; --features
  imageworker vocab &lt;dir&gt; --db &lt;db&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1034" />
        <source>This index has no object-search data — press Index with "Object search data" ticked. Until then only whole-image similarity is available, which cannot find an object inside a picture.</source>
        <translation>이 색인에는 객체 검색 데이터가 없습니다. 「객체 검색 데이터」를 켠 채로 「색인 만들기」를 누르세요. 그전까지는 전체 이미지 유사도만 쓸 수 있으며, 그것으로는 그림 안의 객체를 찾을 수 없습니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1056" />
        <source>This index has no local-feature descriptors yet, so an object cannot be located inside an image.

Build them first:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</source>
        <translation>이 색인에는 아직 지역 특징 서술자가 없어 이미지 안에서 객체를 찾을 수 없습니다.

먼저 만드세요:

  imageworker index "%1" --db "%2" --features
  imageworker vocab "%1" --db "%2"</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1069" />
        <source>Paste a screenshot with Ctrl+V, drop an image, or pick a file first.</source>
        <translation>먼저 Ctrl+V로 스크린샷을 붙여넣거나, 이미지를 끌어다 놓거나, 파일을 선택하세요.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1095" />
        <source>Locating</source>
        <translation>위치 찾는 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1154" />
        <source>Searching</source>
        <translation>검색 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1192" />
        <source>distance</source>
        <translation>거리</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1200" />
        <source>the same picture</source>
        <translation>같은 그림입니다</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1202" />
        <source>plausibly the same picture, re-encoded or rescaled</source>
        <translation>재인코딩하거나 크기를 바꾼 같은 그림으로 보입니다</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1204" />
        <source>nothing convincing — these are far apart, whatever the scores suggest</source>
        <translation>확실한 것이 없습니다. 점수가 어떻든 서로 멀리 떨어져 있습니다</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1207" />
        <source>%1 hits of %2 indexed files, closest distance %3: %4. Whole-image similarity, not object location.</source>
        <translation>색인된 %2개 파일 중 %1건, 최소 거리 %3: %4. 객체 위치 찾기가 아니라 전체 이미지 유사도입니다.</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1231" />
        <source>inliers</source>
        <translation>인라이어</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1236" />
        <source>Nothing located. Try boxing the object, or check that the asset is actually indexed.</source>
        <translation>아무것도 찾지 못했습니다. 객체를 상자로 지정하거나, 해당 에셋이 실제로 색인되어 있는지 확인하세요.</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/gui/MainWindow.cpp" line="1252" />
        <source>Located %n asset(s); best: %1 (%2 inliers, %3% consistent)</source>
        <translation><numerusform>에셋 %n개를 찾았습니다. 최적: %1 (인라이어 %2개, %3% 일치)</numerusform></translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1288" />
        <source>Looking for copies of %1…</source>
        <translation>%1의 사본을 찾는 중…</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1320" />
        <source>Open</source>
        <translation>열기</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1323" />
        <source>Reveal in file manager</source>
        <translation>파일 관리자에서 보기</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1327" />
        <source>Find duplicates of this image</source>
        <translation>이 이미지의 중복 찾기</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1329" />
        <source>Use as reference image</source>
        <translation>기준 이미지로 사용</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1334" />
        <source>Copy full path</source>
        <translation>전체 경로 복사</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="1335" />
        <source>Copy path relative to the root</source>
        <translation>루트 기준 상대 경로 복사</translation>
    </message>
</context>
<context>
    <name>QueryImageView</name>
    <message>
        <location filename="../src/gui/QueryImageView.cpp" line="135" />
        <source>Ctrl+V to paste a screenshot,
or drop an image here</source>
        <translation>Ctrl+V로 스크린샷을 붙여넣거나,
여기에 이미지를 끌어다 놓으세요</translation>
    </message>
</context>
<context>
    <name>ResultModel</name>
    <message>
        <location filename="../src/gui/ResultModel.cpp" line="146" />
        <source>distance %1</source>
        <translation>거리 %1</translation>
    </message>
</context>
<context>
    <name>stage</name>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="87" />
        <source>scanning</source>
        <translation>훑는 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="88" />
        <source>indexing</source>
        <translation>색인</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="89" />
        <source>descriptors</source>
        <translation>서술자</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="90" />
        <source>pruning</source>
        <translation>정리</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="91" />
        <source>loading</source>
        <translation>불러오는 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="92" />
        <source>matching</source>
        <translation>대조</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="93" />
        <source>collecting</source>
        <translation>모으는 중</translation>
    </message>
    <message>
        <location filename="../src/gui/MainWindow.cpp" line="94" />
        <source>verifying</source>
        <translation>검증</translation>
    </message>
</context>
<context>
    <name>theme</name>
    <message>
        <location filename="../src/gui/Theme.cpp" line="228" />
        <source>System</source>
        <translation>시스템</translation>
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
        <translation>블라썸</translation>
    </message>
</context>
</TS>