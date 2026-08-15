# Using ImageWorker

ImageWorker answers two questions about a folder of `png` and `jpg` files:

- **Where does this asset appear in my screenshot?** — the main job.
- **Which files in this folder are duplicates?** — the secondary one.

---

## 1. Point it at a folder

**Images** is the folder to search. **Index in** is where ImageWorker keeps its
own data: the database, the previews and the descriptors.

By default the index goes into a hidden `.imageworker` folder inside the image
folder. Change **Index in** when the images live on a read-only or network
share, when you would rather not add anything to someone else's repository, or
when the index belongs on a faster disk. **Default** puts it back.

Press **Index**. The first run reads every file; later runs only re-read files
whose size or modification time changed, so keeping the index up to date is
cheap.

> Indexing 4600 images of about 400 MB takes roughly three minutes, including
> the neural descriptors.

---

## 2. Find an asset inside a screenshot

Open **Find by example**, then get the screenshot in:

- press **Ctrl+V** to paste it from the clipboard — works from either tab;
- drag an image file onto the window;
- or use **Browse…**.

**Drag a box around the object you are looking for.** This matters more than it
looks: on a full frame the search spends most of its effort on background. On a
4600-asset library a boxed object takes about **2.6 seconds against 5.6** for
the whole frame, and finds roughly **twice as many matching points**, because
every one of them lands on the object instead of the scenery. Drag outside the
box to clear it and search the whole frame again.

Press **Search**. The best match is outlined on the screenshot, and every result
appears below, ranked.

### Reading the results

Each result shows a **score** and an **inlier count**. Inliers are the number of
matched points that agree on one consistent placement — that is the evidence.
The score weighs how *consistent* those matches were, discounted when there are
few of them.

**Results are a ranked list, not a yes/no answer.** Different artwork of the
same character will also pass verification, because most of the picture really
does match. The correct one ranks clearly above them, so read the order rather
than looking for a single winner.

### Method

- **Locate object in the image** — the neural search described above. Needs an
  index built with descriptors; if the entry is greyed out, see below.
- **Whole-image similarity** — a perceptual hash comparing pictures as a whole.
  Good for finding a rescaled or re-encoded copy of the *same picture*, useless
  for finding a small object inside a screenshot.

If **Locate object** is disabled, the index has no descriptors yet. Build them
once from the command line:

```
imageworker index "<image folder>" --db "<index folder>\index.db" --features
imageworker vocab "<image folder>" --db "<index folder>\index.db"
```

---

## 3. Find duplicates

Open **Duplicates**, set **Max distance** and press **Find duplicates**.

- **Exact** groups are byte-identical files. These are safe to act on.
- **Near** groups look alike to a perceptual hash. **Review them; do not delete
  them blindly.** The hash describes layout, so one character drawn with several
  expressions lands in one group at any usable distance.

Distance 0 means identical fingerprints; 4 tolerates re-encoding and rescaling.
Above 8 the groups are for review only.

Groups are listed by how much space they waste, largest first. Inside a group
the largest and highest-resolution file is listed first — usually the one to
keep.

---

## 4. Getting paths out

Every result carries the full path:

- the bar at the bottom shows the path of the current selection, and **Copy**
  copies every selected path, one per line;
- the small copy glyph beside a file name copies just that one;
- right-click for the full path, the path relative to the folder, or to reveal
  the file in Explorer.

---

## 5. Other git branches

If the image folder is inside a git repository, **Branches…** becomes available.
Tick the branches you want and press **Index**: their images are read straight
out of the repository, so nothing is checked out and your working tree is never
touched. The branch you have checked out is not listed — it *is* the working
tree.

The branch list is the complete picture of what the index holds. Unticking a
branch and indexing again removes it.

Results from a branch show the branch name and identify themselves as
`branch:path` rather than as a file path, because there is no such file on disk.
**Open** extracts that version to a temporary file so you can look at it;
revealing it in a file manager, and searching with it, are only for files in the
working tree.

Re-indexing is cheap: a branch whose tip has not moved is skipped entirely, and
within a branch only files whose content actually changed are re-read.

> **Git LFS**: the images are resolved from your local LFS store. If a branch
> was never fetched, ImageWorker says how many objects are missing and that
> `git lfs fetch --all` will get them — it never downloads anything by itself.

---

## 6. Themes and language

**View → Theme** switches between System, Sci-fi, Material and Blossom while the
program is running. The choice is remembered.

**View → Language** switches the interface, also without a restart. The list
holds every language this build carries; **System language** follows the
operating system. This manual is translated too, and falls back to English for
a language whose manual is not ready yet.

---

## What it cannot do

- Perceptual hashing finds **the same picture**, never merely a similar subject.
- The neural search finds assets that are **actually present** in the image. It
  is not a search by description.
- An asset that produced no keypoints — a flat colour, a tiny icon — cannot be
  located. Roughly 4% of a typical asset library falls into this category.
