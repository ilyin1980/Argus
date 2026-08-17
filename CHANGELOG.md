# Changelog

Notable changes, newest first. Versions follow [semantic versioning](https://semver.org);
while the major version is 0, the CLI output contract is the part treated as
stable.

## 0.1.0 — first public release

The first build published outside the machine it was written on. Everything
below is in it.

### Locating an asset inside a screenshot

- Local features from **DISK**, matched with **LightGlue** and verified with a
  RANSAC homography, so a result comes with the rectangle showing where the
  asset sits in the frame. The homography covers anisotropic scaling, so a
  nine-sliced UI element stretched along one axis still verifies.
- A **bag-of-words shortlist** over a visual vocabulary trained from the
  library's own descriptors. It is a recall filter: its scores are deliberately
  not used for ranking, because they do not rank.
- Ranking by `inlierRatio × min(1, inliers / 50)`. Raw inlier counts favour
  large busy images — a lookalike scored 140 inliers at 89% where the true
  source scored 123 at 98%.
- **Large textures are indexed in tiles**, each tile its own document, so a crop
  covering one percent of a source texture is still findable.
- **Masked template matching** as a second channel for flat UI art that carries
  too few keypoints, with a deliberately strict threshold when scanning a whole
  library and a lenient one when checking a single named asset.
- Geometric plausibility checks on the recovered outline — convexity, area,
  skew — with the reason for a rejection reported rather than swallowed.

### Finding duplicates

- Exact groups by BLAKE2b, near-duplicate groups by a double perceptual
  fingerprint (pHash and dHash) with multi-index hashing for candidate
  generation.
- Near groups mean *review these*, never *delete these*. Only exact groups are
  safe to act on automatically, and the interface says so.

### Interface

- Qt GUI and a headless CLI built from one core library, so a person and a
  script get identical answers.
- Clipboard paste, drag-and-drop, and a drag-to-select region on the query.
- Four themes switchable at runtime, **13 interface languages** with the manual
  translated into each, and an in-window help browser.
- Thumbnail grid with full paths, copy-to-clipboard on every result, and a
  status line that explains a weak result instead of only counting it.

### Indexing

- Incremental: a file is re-read only when its size or mtime changed.
- **Images inside other Git branches** can be indexed straight out of the object
  store, with no checkout and no change to the working tree, **including Git
  LFS** — pointer files are resolved from the local LFS store, and missing
  objects are reported once rather than fetched.
- Descriptors live in flat append-only half-precision files with SQLite holding
  only the offsets.
- The index folder can be pointed away from the image folder, so a read-only
  share or someone else's repository stays untouched.

### Platforms

- **DirectML** on Windows, **CUDA** on Linux, **CoreML** on macOS, each falling
  back to the CPU provider quietly, all three run on real hardware.
- The matcher model is chosen by what was measured on each backend rather than
  by rule of thumb: the fp16 export wins only on DirectML, is nearly four times
  slower than full precision on CUDA, returns zero matches without an error on
  CoreML, and does not run at all on CPU.
- Self-contained packages: unpack and run, with nothing read from `PATH`. The
  Windows package is verified by running it with `PATH` cut down to
  `C:\Windows\system32`.

### Known limits at this release

- A search takes seconds, not milliseconds. Selecting a region is both the
  speed-up and the accuracy fix.
- Different artwork of the same character verifies too. Ranking separates it
  reliably; a yes-or-no threshold does not, which is why results are a list.
- Artwork whose shape lives only in its alpha channel — shadows, glows — cannot
  be located after compositing, and the tool says so instead of guessing.
