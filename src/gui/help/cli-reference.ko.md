# 명령줄과 자동화

패키지에는 **같은 코어 라이브러리로 만든 실행 파일 두 개**가 들어 있습니다.

| | |
|---|---|
| `argus-gui` | 지금 이 글을 읽고 있는 창 |
| `argus` | 창 없이 동작. GUI가 하는 모든 일에 더해, GUI가 내놓지 않는 색인 옵션까지 |

둘은 로직을 전부 공유하므로 한쪽이 만든 색인을 다른 쪽이 읽습니다. 어느 쪽도 다른
쪽을 감싼 껍데기가 아니라, 둘 다 같은 정적 코어를 링크합니다.

---

## 출력 규약

스크립트를 짤 때 이 규약에 기대도 됩니다. 일부러 안정적으로 유지합니다.

- `--json`은 **줄로 구분된 JSON을 stdout에** 씁니다. 한 줄에 객체 하나입니다. 모든
  결과는 `rel`(색인한 폴더 기준 상대 경로)과 `path`(절대 경로)를 함께 지닙니다.
- `--paths`는 파이프에 쓰라고 **절대 경로만** 한 줄에 하나씩 씁니다. `dupes`에서는
  빈 줄이 묶음을 나눕니다. 이 옵션은 `--quiet`를 포함하며, `--json`과 함께 쓰면 둘 중
  하나를 조용히 고르는 대신 오류가 납니다.
- 진행 상황과 경고, 오류는 언제나 **stderr**로 갑니다. stdout으로는 절대 가지 않습니다.
- 결과는 결정적인 순서로 나옵니다.
- 종료 코드: **0** 무언가 찾음, **1** 아무것도 못 찾음, **2** 오류.

---

## 명령

```
argus index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
argus vocab  <dir> [--words 2048] [--sample 120000]
argus find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist N] [--top N]
argus dupes  <dir> [--distance 4] [--exact-only] [--near-only]
argus query  <dir> --image ref.png [--top 20]
argus match  --query q.png --asset a.png
argus stats  <dir>
argus doctor [--extract img.png]
argus formats
```

모든 명령은 `--db <path>`를 받아 색인을 `<dir>/.argus`가 아닌 다른 곳에 둘 수
있습니다.

### 객체 검색을 위해 폴더 준비하기

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index`만 해도 중복 검색은 됩니다. `--features`가 신경망 서술자를 더하고, `vocab`이
그 위에 후보 색인을 학습시킵니다. `find`가 동작하려면 둘 다 필요합니다. `vocab`은
파일 하나 추가할 때마다가 아니라 에셋을 크게 한 묶음 추가한 뒤에 다시 돌리세요.

### 검색

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

객체가 어디 있는지 안다면 `--roi x,y,w,h`를 넘기세요. 무엇보다 효과가 큰 옵션입니다.
지정한 영역에서는 검색이 약 다섯 배 빠르고, 일치하는 점은 대략 두 배 나옵니다.

### 다른 git 브랜치

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

이미지는 객체 저장소에서 읽으므로 체크아웃이 일어나지 않고 작업 트리도 그대로
있습니다. 지정한 집합이 곧 완전한 집합입니다. 전에 색인했지만 이번에 이름을 대지
않은 브랜치는 색인에서 빠집니다. `--branches all`은 모든 로컬 브랜치를,
`--remote-branches`는 원격 추적 브랜치까지 포함합니다.

다시 색인하는 비용은 적습니다. 끝이 움직이지 않은 브랜치는 트리를 하나도 나열하지
않고 건너뛰며, 브랜치 안에서는 blob 아이디가 판단 기준입니다. 내용 주소이므로
파일에 쓰는 크기와 수정 시각보다 강한 확인입니다.

브랜치에서 나온 행은 파일 시스템 경로를 결코 알려 주지 않습니다. 그런 경로가 없기
때문입니다. 보통 출력과 `--paths`는 git 자체의 `브랜치:경로` 문법을 쓰며 `git
show`가 그대로 받습니다. `--json`은 `path` 대신 `ref`, `blob`, `rev`를 담습니다.

Git LFS에서는 객체를 로컬 LFS 저장소에서 가져옵니다. 없는 것은 세어서 한 번만
알리고, 해결책으로 `git lfs fetch --all`을 제시합니다. 스스로 내려받는 일은
없습니다.

### 파이프

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## 설치 상태 확인

```
argus doctor
```

ONNX Runtime과 OpenCV의 버전, 사용 가능한 실행 공급자, 그리고 이 드라이버에서 실제로
DirectML 세션을 만들 수 있는지를 알려 줍니다. 공급자가 목록에 보이는 것과 실제로 만들
수 있는 것은 다릅니다. `--extract <image>`는 파일 하나에 추출기를 돌려 특징점을 몇 개
찾았고 얼마나 걸렸는지 알려 줍니다.

---

## 무엇이 어디에 있나

```
<색인 폴더>/
  index.db              SQLite: 파일, 해시, 미리보기, 서술자 오프셋
  features/desc.f16     서술자, 반정밀도
  features/kpts.f32     특징점
  features/vocab.bin    시각 어휘
  features/bow.bin      후보 색인
```

색인 폴더를 지우면 모든 것이 초기 상태로 돌아가고 그 밖에는 아무것도 건드리지 않습니다.
데이터베이스의 경로는 **색인한 폴더 기준 상대 경로**로 저장되므로, 같은 저장소를 다른
곳에 마운트해도 색인은 그대로 유효합니다.

모델은 실행 파일 옆의 `models/`에서 먼저 찾고, 그다음 상위 디렉터리들을 찾습니다. 덕분에
패키징한 폴더든 빌드 트리든 설정 없이 동작합니다.

---

## 빌드

```
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF`는 ONNX Runtime과 OpenCV 없이 빌드합니다. 중복 찾기는
계속 되지만 객체 검색은 되지 않습니다.

설치 단계는 자족적인 폴더를 꾸립니다. 모든 의존물이 실행 파일 옆에 놓이고 `PATH`에서는
아무것도 읽지 않습니다. 단일 파일 빌드는 불가능합니다. DirectML을 쓰는 ONNX Runtime은
DLL로만 제공되고, `DirectML.dll`은 애초에 링크해 넣을 수 없는 재배포 구성 요소이기
때문입니다.
