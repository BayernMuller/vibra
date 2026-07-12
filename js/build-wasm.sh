#!/bin/bash
set -e

repo_root="$(cd "$(dirname "$0")/.." && pwd)"
build_dir="$repo_root/build-js-wasm"

rm -rf "$build_dir"
export EM_CACHE="${EM_CACHE:-/tmp/vibra-emscripten-cache}"

emcmake cmake -B "$build_dir" -S "$repo_root" \
    -DLIBRARY_ONLY=ON \
    -DBUILD_TESTING=OFF \
    -DBUILD_JS=ON

cmake --build "$build_dir" --target vibra_wasm -j
