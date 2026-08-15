# ImageWorker

Finds images. Two related jobs, one core library, driven by both a Qt GUI and a
headless CLI so that a person and an automation script get identical results.

**Locate an asset inside a screenshot** — the main job. Grab a screenshot from a
running game, hand it over as a file or straight from the clipboard, and get back
the source `png`/`jpg` files that appear in it, each with the rectangle showing
where. Survives rescaling, background clutter, colour tinting and compression.

**Find duplicates in a folder** — the secondary job. Byte-identical copies and
near-identical variants, grouped and ranked by reclaimable space.

## Status

| Stage | State |
|---|---|
| Scanning, hashing, SQLite index, duplicate grouping | done |
| Qt GUI: thumbnail grid, clipboard paste, drag-to-select region | done |
| Neural local features (DISK) at index time | done |
| Bag-of-words shortlist + LightGlue matching + RANSAC verification | done |
| GUI wiring for the search, masked template matching for flat UI art | to do |
| HTTP worker mode for a remote Linux box | deferred |

Measured on a 4665-image Unity asset library: indexing 196 s, one search 12.5 s
(the search is the current bottleneck — see *Known limits*).

## Building

One script per platform. Both locate the compiler and Qt themselves, and
**download any missing dependency** before configuring.

```bash
tools\build.bat
```

```bash
tools/build.sh
```

| argument | effect |
|---|---|
| *(none)* | release build with the neural backend |
| `debug` | debug build |
| `package` | build, then assemble the self-contained `dist/` |
| `clean` | rebuild from scratch |
| `no-inference` | skip ONNX Runtime and OpenCV; duplicate finding still works |

Override discovery with `QTDIR` / `QT_PREFIX`, `ORT_ROOT` or `OPENCV_DIR`.

To fetch dependencies without building, run `tools\fetch-deps.ps1` or
`tools/fetch-deps.sh` directly.

On Linux and macOS, Qt and OpenCV come from the system package manager: the
script checks for them and prints the single command that installs them, rather
than claiming root for itself. Everything else — ONNX Runtime, the CUDA runtime
and cuDNN where a card is present, the model weights — lands in `third_party/`,
`~/imageworker-deps` and `models/`, none of which are in the repository.

After a Unix build, `source ~/imageworker-deps/env.sh` before running the
binaries, so the runtime libraries are found.

### By hand

Qt 6.4+ (Core, Gui, Sql, Widgets) and CMake 3.21. The neural backend is optional
and off by default.

```bash
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON && cmake --build --preset msvc-release
```

That needs ONNX Runtime and OpenCV under `third_party/`, and the models under
`models/` — see *Third-party layout* below. The MSVC preset expects a Visual
Studio environment (`VC\Auxiliary\Build\vcvars64.bat`); `mingw-release` needs no
such setup.

In Qt Creator: **File → Open File or Project →** `CMakeLists.txt`.

## CLI

```bash
imageworker index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
imageworker vocab  <dir> [--words 2048] [--sample 120000]
imageworker find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist 200] [--top 10]
imageworker dupes  <dir> [--distance 4] [--exact-only] [--near-only]
imageworker query  <dir> --image ref.png [--top 20]
imageworker match  --query q.png --asset a.png
imageworker stats  <dir>
imageworker doctor [--model m.onnx] [--extract img.png]
imageworker formats
```

Every command takes `--db <path>` to put the index somewhere other than
`<dir>/.imageworker`.

Output contract, which scripts and coding agents can rely on:

- `--json` writes **newline-delimited JSON to stdout**, one object per line;
  every result carries both `rel` (relative to the indexed root) and `path`
  (absolute);
- `--paths` writes bare absolute paths instead, one per line, for piping — in
  `dupes` a blank line separates groups. It implies `--quiet` so nothing else
  reaches stdout, and combining it with `--json` is an error;
- progress, warnings and errors always go to **stderr**, never stdout;
- results come out in a deterministic order;
- exit codes: `0` found something, `1` found nothing, `2` error.

```bash
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --paths | clip
```

### Typical session

```bash
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
```

```bash
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

```bash
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Indexing is incremental: a file is re-read only when its size or mtime changed.
`vocab` needs re-running after a large batch of new assets, not after every one.

## GUI

```bash
imageworker-gui [dir] [--index <dir>]
```

**Images:** picks the folder to search; **Index in:** picks where the database,
previews and descriptors are kept — point it away from the image folder to leave
a read-only share or someone else's repository untouched. `--index` sets the
same thing from the command line.

Press **Index**, then either **Find duplicates**, or switch to *Find by example*,
paste a screenshot with **Ctrl+V** (works from either tab), and drag a box around
the object you are looking for.

Every result shows its file name and dimensions, with a small copy glyph beside
the name that puts the **full path** on the clipboard. The bar along the bottom
shows the full path of the current selection and copies every selected path at
once, one per line.

## Packaging

```bash
cmake --install build/msvc-release
```

Assembles a self-contained folder in `dist/`: both executables, the Qt runtime
and plugins, ONNX Runtime, DirectML, OpenCV, the MSVC runtime libraries and the
models. Nothing needs installing on the target machine and nothing is read from
`PATH`.

```bash
cpack --config build/msvc-release/CPackConfig.cmake
```

Produces `ImageWorker-<version>-<system>-<arch>.zip` — about 126 MB compressed,
227 MB unpacked.

`-DIMAGEWORKER_PACKAGE_MODELS=OFF` leaves the 70 MB of ONNX weights out, for
builds that ship them separately.

### Not statically linked, and why

ONNX Runtime with DirectML is distributed as a DLL only — its import library is
3 KB — and `DirectML.dll` is a Microsoft redistributable that cannot be linked
into an executable at all. A single-file build is therefore impossible for the
neural path. What the package guarantees instead is that every dependency sits
beside the executables, so the folder runs on a machine with no Qt, no OpenCV
and no runtime installed. That is verified by running the packaged binaries with
`PATH` reduced to `C:\Windows\system32`.

### macOS and Linux

The CMake rules are cross-platform and `macdeployqt` is picked up the same way
`windeployqt` is. Two things still need doing before either produces a working
package:

- **No DirectML outside Windows.** `FeatureExtractor::create` and
  `FeatureMatcher::create` need a CoreML branch on macOS and a CUDA branch on
  Linux, both falling back to the CPU provider.
- **The fp16 matcher does not run on CPU** — ONNX Runtime has no CPU kernel for
  its packed-QKV attention. A CPU fallback must load
  `disk_lightglue_fused_cpu.onnx`, which is why that model is in the package.

## Third-party layout

Nothing is installed system-wide.

```
third_party/onnxruntime/{include,lib,bin}   ONNX Runtime 1.24.4 + DirectML 1.15.4
third_party/opencv/                          OpenCV 4.14.0, prebuilt
models/disk.onnx                             DISK local-feature extractor
models/disk_lightglue_fused_fp16.onnx        LightGlue matcher
```

All Apache-2.0 or MIT. SuperPoint is deliberately absent: its weights carry a
non-commercial licence.

`imageworker doctor` reports what the backend can actually do, including whether
DirectML works on the installed driver, and prints each model's tensor signature.

## How the search works

1. **Extract** local features from the query once. Local, not global: on a
   screenshot the object of interest is a few percent of the frame, so any
   whole-image fingerprint or embedding compares mostly clutter.
2. **Shortlist** with a bag-of-words index over the visual vocabulary —
   microseconds for thousands of images. This is a recall filter; its scores do
   not rank usefully and are not used for ranking.
3. **Match and verify** only the shortlist: LightGlue for correspondences, then
   a RANSAC homography. Inliers score the match, and the asset's own rectangle
   projected through the homography is the rectangle shown to the user. A
   homography covers anisotropic scaling, so a nine-sliced UI element stretched
   in one axis still verifies.

Results are ranked by `inlierRatio × min(1, inliers / 50)`. Ratio leads because
raw inlier counts favour large busy images: in testing, a lookalike produced 140
inliers at 89% while the true source produced 123 at 98%.

## Known limits

- **A search takes ~12 s** against 4500 assets: 200 candidates at ~60 ms of
  LightGlue each. The shortlist cannot simply be shrunk — the correct answer is
  not near the top of the bag-of-words ranking.
- **Different artwork of the same character verifies too.** Ranking separates it
  reliably, a yes/no threshold does not, which is why results are a ranked list.
- **Perceptual hashing (`dupes`, `query`) answers "is this the same picture"**,
  never "is this a similar subject". A character drawn with several expressions
  groups together at any usable radius.
- Near-duplicate groups mean *review these*, not *delete these*. Only exact
  groups are safe to act on automatically.

## Design notes

- Paths are stored **relative to the indexed root**, so an index built on one
  machine stays valid where the same storage is mounted elsewhere.
- Transparent pixels are flattened onto a fixed neutral grey before hashing, and
  keypoints landing on transparent areas are discarded: a sprite must not depend
  on what happened to be behind it.
- Descriptors live in flat append-only files in half precision, with SQLite
  holding only the offsets. Whole rows are always read together, so SQLite blobs
  would churn the page cache for nothing.
- Degenerate hash buckets are skipped above `--bucket-limit` and the count is
  reported, never silently dropped.
