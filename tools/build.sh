#!/usr/bin/env bash
# ---------------------------------------------------------------------------
#  Build Argus on Linux or macOS.
#
#    tools/build.sh                configure and build, release, with inference
#    tools/build.sh debug          debug build
#    tools/build.sh package        build, then assemble dist/
#    tools/build.sh clean          rebuild from scratch
#    tools/build.sh no-inference   skip ONNX Runtime and OpenCV entirely
#
#  Missing dependencies are fetched automatically; see tools/fetch-deps.sh.
#  Override discovery with QT_PREFIX or ORT_ROOT.
# ---------------------------------------------------------------------------
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEPS="$HOME/argus-deps"

BUILD_TYPE=RelWithDebInfo
INFERENCE=ON
DO_PACKAGE=0
DO_CLEAN=0

for arg in "$@"; do
    case "$arg" in
        debug)        BUILD_TYPE=Debug ;;
        package)      DO_PACKAGE=1 ;;
        clean)        DO_CLEAN=1 ;;
        no-inference) INFERENCE=OFF ;;
        *) echo "unknown option: $arg"; exit 2 ;;
    esac
done

case "$(uname -s)" in
    Linux)  PLATFORM=linux;  PRESET_DIR=linux ;;
    Darwin) PLATFORM=macos;  PRESET_DIR=mac ;;
    *) echo "unsupported platform: $(uname -s)"; exit 1 ;;
esac
BUILD="$ROOT/build/$PRESET_DIR-$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"

# --- dependencies ------------------------------------------------------------
if [ "$INFERENCE" = ON ]; then
    if [ -z "${ORT_ROOT:-}" ]; then
        # Newest matching unpacked runtime wins; fetch-deps picks the variant.
        ORT_ROOT=$(ls -d "$DEPS"/onnxruntime-* 2>/dev/null | sort -V | tail -1)
    fi
    if [ -z "${ORT_ROOT:-}" ] || [ ! -f "$ORT_ROOT/include/onnxruntime_cxx_api.h" ]; then
        echo "ONNX Runtime not found — fetching it"
        bash "$ROOT/tools/fetch-deps.sh" || exit 1
        ORT_ROOT=$(ls -d "$DEPS"/onnxruntime-* 2>/dev/null | sort -V | tail -1)
    fi
    if [ ! -f "$ORT_ROOT/include/onnxruntime_cxx_api.h" ]; then
        echo "ERROR: still no ONNX Runtime. Build without it: tools/build.sh no-inference"
        exit 1
    fi
fi

if [ -z "${QT_PREFIX:-}" ] && [ "$PLATFORM" = macos ]; then
    QT_PREFIX="$(brew --prefix qt 2>/dev/null)"
fi

echo "platform  : $PLATFORM"
echo "build type: $BUILD_TYPE"
echo "inference : $INFERENCE"
[ "$INFERENCE" = ON ] && echo "ort       : $ORT_ROOT"
[ -n "${QT_PREFIX:-}" ] && echo "qt        : $QT_PREFIX"
echo

[ $DO_CLEAN -eq 1 ] && rm -rf "$BUILD"

CMAKE_ARGS=(
    -S "$ROOT" -B "$BUILD" -G Ninja
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DARGUS_WITH_INFERENCE="$INFERENCE"
)
[ -n "${QT_PREFIX:-}" ] && CMAKE_ARGS+=(-DCMAKE_PREFIX_PATH="$QT_PREFIX")
if [ "$INFERENCE" = ON ]; then
    CMAKE_ARGS+=(-DARGUS_ORT_ROOT="$ORT_ROOT")
    # Windows vendors a prebuilt OpenCV; here the system package is the right
    # one, and pointing the variable at nothing makes CMake fall back to it.
    CMAKE_ARGS+=(-DARGUS_OPENCV_DIR="/nonexistent-use-system-opencv")
fi

cmake "${CMAKE_ARGS[@]}" || exit 1

JOBS=$( (command -v nproc >/dev/null && nproc) || sysctl -n hw.ncpu || echo 4 )
if [ $DO_PACKAGE -eq 1 ]; then
    cmake --build "$BUILD" -j "$JOBS" --target install || exit 1
else
    cmake --build "$BUILD" -j "$JOBS" || exit 1
fi

echo
if [ $DO_PACKAGE -eq 1 ]; then
    echo "Package: $ROOT/dist"
else
    echo "Binaries: $BUILD/bin"
fi
[ -f "$DEPS/env.sh" ] && echo "Before running them: source $DEPS/env.sh"
