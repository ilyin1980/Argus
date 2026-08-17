# Security policy

## Supported versions

The latest release is the only supported one. This is a small project; fixes go
onto `main` and into the next release rather than into patch branches.

## Reporting a vulnerability

Report privately through GitHub's
[security advisory form](https://github.com/ilyin1980/Argus/security/advisories/new)
rather than in a public issue.

Expect an acknowledgement within a week. If a report is valid, the fix and the
advisory are published together.

## What is in scope

Argus reads image files, SQLite databases and Git object stores that a user
points it at, and runs ONNX models from a local folder. The interesting failure
modes are therefore:

- a crafted image, index database, descriptor store or vocabulary file that
  causes memory corruption rather than a clean error;
- a path in an index or a Git tree that escapes the indexed root when a result
  is opened or revealed;
- a model file loaded from a location a user did not intend.

## What is not

- **The bundled dependencies.** Qt, OpenCV and ONNX Runtime carry their own
  advisories; report those upstream. Say so here only if a release ships a
  version with a known advisory that has not been picked up.
- **The macOS bundle being unsigned.** It is documented, not accidental.
- **Denial of service from a large library.** Indexing millions of files is slow
  and memory-hungry by nature; that is a performance report, not a security one.

Argus never contacts the network. The one place that could look like an
exception is Git LFS: pointer files are resolved out of the **local** LFS
object store only, and a missing object is reported instead of fetched.
