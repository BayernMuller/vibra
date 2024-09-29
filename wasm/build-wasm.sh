#!/bin/bash

set -e

# build libvibra as wasm
cd ..
rm -rf build
mkdir -p build
cd build || exit 1

emcmake cmake .. \
    -DLIBRARY_ONLY=ON \
    -DFFTW3_PATH=/usr/local \
    -DCMAKE_INSTALL_PREFIX=/usr/local

make -j
sudo make install

# build the wasm module
ls /usr/local/include
cd ../wasm || exit 1
pwd
emmake make