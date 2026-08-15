# Argus

\image html argus-logo.png width=520px

> *Argus Panoptes, the hundred-eyed giant: half his eyes stayed open while the
> rest slept, so nothing ever passed him unseen.*

Finds which image assets appear inside a screenshot, and where; and groups
duplicate images in a folder. It ships as two executables built from one core
library: the window, `argus-gui`, and the headless `argus`.

This is the API reference, generated from the sources. For how to *use* the
tool, open the manual inside the window (**F1**) or read
`src/gui/help/user-guide.md`.

## How the pieces fit

The core library holds every algorithm and knows nothing about windows,
languages or the command line. Both executables are thin: they collect options,
call into the core, and render what comes back.

```
argus-gui ─┐
                 ├─ argus_core ─ SQLite index + flat descriptor files
argus ─────┘
```

### Reading an image

- argus::decodeForIndex — one decode per file, producing every downstream input.
- argus::kHashSourceBox — the fixed resolution the fingerprints are derived from,
  and the reason a file matches itself.
- argus::dctPerceptualHash, argus::differenceHash — the two 64-bit fingerprints.
- argus::fileContentHash — byte-exact identity, the cheapest stage of the cascade.

### Answering "which files are duplicates"

- argus::Indexer walks the folder, decodes and hashes in parallel, and writes in
  batched transactions.
- argus::findDuplicates groups by content hash first, then by perceptual distance
  through a multi-index scheme over four 16-bit bands.

### Answering "where is this asset in that screenshot"

- argus::FeatureExtractor runs DISK over each image and stores its descriptors in
  argus::DescriptorStore, a flat append-only file rather than a database blob.
- argus::Vocabulary and argus::BowIndex reduce thousands of candidates to a shortlist.
- argus::FeatureMatcher runs LightGlue over the shortlist and verifies each
  candidate with a homography, rejecting the degenerate ones.
- argus::TemplateMatcher is the second channel, for artwork that yields too few
  keypoints for local features to say anything.
- argus::ObjectFinder is the orchestration of all of the above.

### Where the images come from

- argus::scanDirectory — the working tree.
- argus::git::listBlobs and argus::git::BlobReader — other branches, read out of the
  object store without checking anything out.

### The window

- MainWindow owns no search logic; every operation is a call into the core on a
  worker thread.
- i18n — thirteen languages, switchable while running.
- theme — four themes, likewise.
