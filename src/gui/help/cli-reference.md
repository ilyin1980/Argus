# Command line and automation

The package ships **two executables built from the same core library**:

| | |
|---|---|
| `imageworker-gui` | the window you are reading this in |
| `imageworker` | headless; everything the GUI does, plus indexing options the GUI does not expose |

They share all logic, so a folder indexed by one is read by the other. Neither
is a wrapper around the other — both link the same static core.

---

## Output contract

Rely on this when scripting; it is deliberately stable.

- `--json` writes **newline-delimited JSON to stdout**, one object per line.
  Every result carries both `rel` (relative to the indexed folder) and `path`
  (absolute).
- `--paths` writes **bare absolute paths**, one per line, for piping. In `dupes`
  a blank line separates groups. It implies `--quiet`, and combining it with
  `--json` is an error rather than a silent choice between them.
- Progress, warnings and errors always go to **stderr**, never stdout.
- Results come out in a deterministic order.
- Exit codes: **0** found something, **1** found nothing, **2** an error.

---

## Commands

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

Every command takes `--db <path>` to put the index somewhere other than
`<dir>/.imageworker`.

### Setting up a folder for object search

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` alone gives duplicate detection. `--features` adds the neural
descriptors, and `vocab` trains the shortlist index over them; both are needed
before `find` will work. Re-run `vocab` after adding a large batch of assets,
not after every file.

### Searching

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Pass `--roi x,y,w,h` when you know where the object is. It is the single most
effective option: on a boxed region the search is about five times faster and
finds roughly twice as many matching points.

### Piping

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## Checking the installation

```
imageworker doctor
```

Reports the ONNX Runtime and OpenCV versions, the available execution providers,
and whether a DirectML session can actually be created on this driver — which is
not the same as the provider merely being listed. `--extract <image>` runs the
extractor over one file and reports how many keypoints it found and how long it
took.

---

## Where things live

```
<index folder>/
  index.db              SQLite: files, hashes, previews, descriptor offsets
  features/desc.f16     descriptors, half precision
  features/kpts.f32     keypoints
  features/vocab.bin    visual vocabulary
  features/bow.bin      shortlist index
```

Deleting the index folder resets everything and touches nothing else. Paths in
the database are stored **relative to the indexed folder**, so an index stays
valid when the same storage is mounted elsewhere.

Models are looked for in `models/` beside the executable, then in the parent
directories above it — so both a packaged folder and a build tree work without
configuration.

---

## Building

```
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF` builds without ONNX Runtime and OpenCV; the
duplicate finder still works, object search does not.

The install step assembles a self-contained folder — every dependency beside the
executables, nothing read from `PATH`. A single-file build is not possible:
ONNX Runtime with DirectML ships as a DLL only, and `DirectML.dll` is a
redistributable that cannot be linked in at all.
