# コマンドラインと自動化

パッケージには**同じコアライブラリから作られた 2 つの実行ファイル**が入っています。

| | |
|---|---|
| `imageworker-gui` | この文章を読んでいるウィンドウ |
| `imageworker` | 画面なし。GUI にできることすべてに加え、GUI が出していないインデックスの選択肢 |

両者はロジックをすべて共有するので、一方が作ったインデックスをもう一方が読めます。
どちらも他方の薄い皮ではなく、同じ静的コアをリンクしています。

---

## 出力の約束

スクリプトを書くときはこれを当てにできます。意図的に安定させてあります。

- `--json` は **stdout に改行区切りの JSON** を書きます。1 行に 1 オブジェクトです。
  各結果は `rel`（インデックス対象フォルダーからの相対）と `path`（絶対）の両方を
  持ちます。
- `--paths` はパイプ用に**絶対パスだけ**を 1 行ずつ書きます。`dupes` では空行が
  グループの区切りです。このオプションは `--quiet` を含み、`--json` との併用は
  どちらかを黙って選ぶのではなくエラーになります。
- 進捗・警告・エラーは常に **stderr** へ出ます。stdout には決して出ません。
- 結果の順序は決定的です。
- 終了コードは **0** 何か見つかった、**1** 何も見つからなかった、**2** エラーです。

---

## コマンド

```
imageworker index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
imageworker vocab  <dir> [--words 2048] [--sample 120000]
imageworker find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist N] [--top N]
imageworker dupes  <dir> [--distance 4] [--exact-only] [--near-only]
imageworker query  <dir> --image ref.png [--top 20]
imageworker match  --query q.png --asset a.png
imageworker stats  <dir>
imageworker doctor [--extract img.png]
imageworker formats
```

どのコマンドも `--db <path>` を取り、インデックスを `<dir>/.imageworker` 以外の
場所に置けます。

### オブジェクト検索用にフォルダーを用意する

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` だけでも重複検出はできます。`--features` がニューラル記述子を加え、`vocab`
がその上に候補選抜のインデックスを学習します。`find` が動くにはこの両方が必要です。
`vocab` はファイルを 1 つ足すたびではなく、アセットをまとめて大量に足したあとに
再実行してください。

### 検索

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

オブジェクトの位置が分かっているときは `--roi x,y,w,h` を渡してください。数ある
オプションの中でいちばん効きます。囲んだ領域では検索がおよそ 5 倍速くなり、一致する
点はおよそ 2 倍見つかります。

### パイプ

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## インストールの確認

```
imageworker doctor
```

ONNX Runtime と OpenCV のバージョン、利用できる実行プロバイダー、そしてこのドライバー
で DirectML のセッションを実際に作れるかどうかを報告します。プロバイダーが一覧に載って
いることと、実際に作れることは別です。`--extract <image>` は 1 つのファイルに抽出器を
かけ、特徴点がいくつ見つかり、どれだけ時間がかかったかを報告します。

---

## 何がどこにあるか

```
<インデックスフォルダー>/
  index.db              SQLite: ファイル、ハッシュ、プレビュー、記述子のオフセット
  features/desc.f16     記述子、半精度
  features/kpts.f32     特徴点
  features/vocab.bin    視覚語彙
  features/bow.bin      候補選抜インデックス
```

インデックスフォルダーを消せばすべてが初期状態に戻り、ほかには何も触れません。
データベース内のパスは**インデックス対象フォルダーからの相対**で保存されるため、同じ
ストレージを別の場所にマウントしてもインデックスは有効なままです。

モデルは実行ファイルの隣の `models/` を探し、次にその上位のディレクトリを探します。
おかげでパッケージ化したフォルダーでもビルドツリーでも、設定なしで動きます。

---

## ビルド

```
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF` は ONNX Runtime と OpenCV なしでビルドします。
重複検出は動き続けますが、オブジェクト検索は動きません。

インストール手順は自己完結したフォルダーを組み立てます。依存物はすべて実行ファイルの
隣にあり、`PATH` からは何も読みません。単一ファイルのビルドはできません。DirectML 版の
ONNX Runtime は DLL の形でしか提供されず、`DirectML.dll` はそもそもリンクできない
再頒布可能コンポーネントだからです。
