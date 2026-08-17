# Contributing

This is a personal project, but the conventions below are what the existing
code follows, and a change that ignores them costs more to review than it saves
to write.

## Getting a build

```bash
tools\build.bat          # Windows
```

```bash
tools/build.sh           # Linux, macOS
```

Both scripts find the compiler and Qt themselves and download whatever is
missing. `no-inference` builds without ONNX Runtime and OpenCV, which is enough
for anything touching scanning, hashing, the database or the duplicate finder,
and is what CI builds.

## House rules

- **C++20**, namespace `argus`, includes resolved from the repository root
  (`#include "core/Types.h"`).
- **Every public declaration carries a Doxygen comment** — `@brief`, plus
  `@param` and `@return` where they apply, and `@file` at the top of each
  header. `WARN_IF_UNDOCUMENTED` and `WARN_NO_PARAMDOC` are on and the tree
  generates zero warnings; a new warning is a defect, not a style opinion.
- The core library depends on Qt Core, Gui and Sql, and on nothing else. ONNX
  Runtime and OpenCV are reachable only behind `ARGUS_WITH_INFERENCE`.
- Parallelism goes through `core/Parallel.h`, not QtConcurrent: cancellation is
  predictable and no worker touches a slot.
- **One canonical decode path.** Two callers that ask for the same image at
  different intermediate sizes produce different hashes and different keypoints,
  and the failure looks like a matching bug rather than a decoding one. This has
  already cost two separate defects; new decode entry points must go through the
  same chain.
- The GUI never silently falls back to a weaker method. If the index lacks
  descriptors, the neural option is disabled with an explanation of which two
  commands build them — a quiet fallback that returns confident nonsense is
  worse than a refusal.

## The output contract

`argus` is meant to be driven by scripts and by coding agents, so its output is
part of the interface:

- `--json` writes newline-delimited JSON to **stdout**, one object per line;
- `--paths` writes bare absolute paths instead, one per line;
- progress, warnings and errors go to **stderr**, always;
- results come out in a deterministic order;
- exit codes are `0` found something, `1` found nothing, `2` error;
- destructive actions need an explicit `--yes`.

Changing any of these is a breaking change and belongs in `CHANGELOG.md`.

## Testing

There is no automated suite. What exists is a deterministic fixture:
`tools/make-test-screenshot.ps1` composes a synthetic screenshot with a known
asset at a known position, from a local asset library. A change to indexing,
matching or ranking is checked by re-running `find` against it and comparing the
score, the inlier ratio and the returned box against the previous run.

Two habits, both learned the hard way, are worth keeping:

- **One measurement means nothing.** Timings on the same machine vary by
  seconds. Take medians of at least three runs before concluding that a change
  helped, and close the GUI first — a second process holding DirectML sessions
  slows a search by an order of magnitude.
- **A threshold tuned on three examples is tuned to noise.** If a rule separates
  the truth from a lookalike in the examples at hand, find a second real object
  in the same frame before trusting it.

## Adding an interface language

1. Add the code and its endonym to `kEntries` in `src/gui/Localization.cpp`.
2. Add the code to `ARGUS_LANGUAGES` in `src/gui/CMakeLists.txt`.
3. Extract the strings:
   ```bash
   lupdate src/gui -ts translations/argus_<lang>.ts -no-obsolete
   ```
4. Translate in Qt Linguist, then build — `lrelease` compiles the `.qm` files.
5. Optionally translate the manuals as `src/gui/help/user-guide.<lang>.md` and
   `cli-reference.<lang>.md`; they are picked up by a glob, and a missing
   translation falls back to English rather than to an empty tab.

Plurals go through `%n` and `numerusform`, never through an appended "s":
Ukrainian and Polish have three forms, Arabic six. Do not build a sentence by
substituting a word into it — an adjective inflects with its noun.

## Commits

Present tense, saying what the change accomplishes rather than which files it
touched. Where a decision was made by measurement, put the numbers in the commit
message; they are the only record of why the alternative was rejected.

Build output stays out of the repository: `build/`, `dist/`, `doc/`,
`third_party/`, `models/`, `indexes/` and `packages/` are all ignored. Release
archives are published as release assets, never committed — see `.gitattributes`
for the reasoning.
