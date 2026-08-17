## What this changes

<!-- What it accomplishes, not which files it touches. -->

## How it was verified

<!--
Which platform and backend it was built and run on. If the change touches
indexing, matching or ranking, the numbers before and after: score, inlier
ratio and the returned box on the same query.

Timings vary by seconds between runs on one machine, so quote the median of at
least three, and close the GUI first: a second process holding DirectML
sessions slows a search by an order of magnitude.
-->

## Checklist

- [ ] Public declarations carry Doxygen comments; the docs target still emits
      zero warnings
- [ ] No new dependency in `argus_core` beyond Qt Core, Gui and Sql
- [ ] Any new image decoding goes through the existing canonical path
- [ ] The CLI output contract is unchanged, or the change is in `CHANGELOG.md`
- [ ] New user-visible strings are translatable, with plurals via `%n`
