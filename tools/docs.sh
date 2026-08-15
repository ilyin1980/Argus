#!/usr/bin/env bash
# Generate the API documentation.
#
# Unlike the Windows script this one does not fetch a portable Doxygen: the
# upstream project ships no portable Linux build, and every distribution has the
# package. What it does instead is say exactly what to install.
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
output="${1:-$root/build/doc}"

if ! command -v doxygen >/dev/null 2>&1; then
    echo "doxygen not found. Install it first:" >&2
    echo "  Debian/Ubuntu : sudo apt install doxygen graphviz" >&2
    echo "  Fedora        : sudo dnf install doxygen graphviz" >&2
    echo "  Arch          : sudo pacman -S doxygen graphviz" >&2
    echo "  macOS         : brew install doxygen graphviz" >&2
    exit 1
fi

mkdir -p "$output"

# Same trick the CMake target uses: include the checked-in Doxyfile and only
# redirect the output, so there is one description of the documentation.
generated="$(mktemp)"
trap 'rm -f "$generated"' EXIT
{
    echo "@INCLUDE = $root/Doxyfile"
    echo "OUTPUT_DIRECTORY = $output"
} > "$generated"

cd "$root"
doxygen "$generated"

echo "Documentation written to $output/html/index.html"
