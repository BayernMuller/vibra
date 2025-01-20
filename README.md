<p align="center">
    <img src="docs/project_vibra.png" width="400"/>
    <br>
    <img src='docs/logo_license.svg'>
</p>

<span align="center">

# vibra

</span>

<div align="center">

![webassembly-main] ![linux-amd64-main] ![linux-arm64-main] ![macos-amd64-main] ![macos-arm64-main] ![windows-amd64-main]

</div>

<p align="center">
    <img src="https://img.shields.io/github/stars/bayernmuller/vibra?style=social" alt="GitHub stars"/>
    <img src="https://img.shields.io/github/last-commit/bayernmuller/vibra"/>
    <img src="https://img.shields.io/github/license/bayernmuller/vibra"/>
</p>


### Overview

* **vibra** is a library and CLI tool for music recognition using the **unofficial** Shazam API.
* It analyzes audio files, generates fingerprints, and queries Shazam's database for song identification.
* **Key features**:
    * Fast and lightweight, optimized for various platforms, including embedded devices.
    * Cross-platform support: Linux, Windows, macOS, **WebAssembly**, and **FFI bindings** for other languages.
    * Flexible input processing: native support for WAV files, optional FFmpeg for other formats.
* **Based on Shazam's algorithm**:
    * [An Industrial-Strength Audio Search Algorithm](https://www.ee.columbia.edu/~dpwe/papers/Wang03-shazam.pdf)
    * [How does Shazam work](https://www.cameronmacleod.com/blog/how-does-shazam-work)
* **Inspired by** [SongRec](https://github.com/marin-m/SongRec/tree/master), adapted to C++ 11.
* **Target platforms**:
    * Embedded devices (e.g., Raspberry Pi, Jetson Nano)
    * Desktop and server environments for high-performance recognition
    * WebAssembly for web-based use
    * Additional support for iOS, Android, and other languages via FFI bindings

### Live Demo

* You can try the music recognition with the **[WebAssembly version of vibra here](https://bayernmuller.github.io/vibra-live-demo/)**
* The source code for the demo is available at [vibra-live-demo](https://github.com/bayernmuller/vibra-live-demo)

### Building the WebAssembly Version
* Please refer to **[bindings/wasm/README.md](bindings/wasm/README.md)** for instructions on building and running the WebAssembly version of vibra.

### Building the Native Version

#### Prerequisites
* vibra requires CMake for its build process. Please install [CMake](https://cmake.org/) before proceeding.
* The project is developed using the **C++11** standard.
* vibra has the following dependencies:
    * [CMake](https://cmake.org/): A cross-platform build system generator.
    * [libfftw3](http://www.fftw.org/): A library for computing Fast Fourier Transforms.
    * [libcurl](https://curl.se/libcurl/) (CLI tool only): A library for transferring data with URLs.
      * If you don't need CLI tool, libcurl is not required.
      * You can disable it by setting the `-DLIBRARY_ONLY=ON` option in the CMake command.
    * [FFmpeg](https://ffmpeg.org/) (Optional): Provides support for audio formats other than WAV (e.g., MP3, FLAC).
        * Install FFmpeg if you need to process audio files in formats other than WAV.

#### Install dependencies
* **Ubuntu**
    * `sudo apt-get update`
    * `sudo apt-get install cmake libcurl4-openssl-dev libfftw3-dev`
    * `sudo apt-get install ffmpeg` (Optional)
* **Windows**
    * Install [CMake](https://cmake.org/download/)
    * Install [vcpkg](https://github.com/Microsoft/vcpkg)
    * Install dependencies using vcpkg:
        * `vcpkg install curl:x64-windows fftw3:x64-windows`
    * Add the vcpkg toolchain file to your CMake command (see Build section)
    * Install [FFmpeg](https://ffmpeg.org/download.html#build-windows) (Optional)
* **macOS**
    * Install [Homebrew](https://brew.sh/)
    * `brew install cmake curl fftw`
    * `brew install ffmpeg` (Optional)


#### Build
* Clone vibra repository
    * `git clone https://github.com/bayernmuller/vibra.git`

* Run the following commands to build vibra:
    * `cd vibra`
    * `mkdir build && cd build`
    * `cmake ..`
    * `make`
    * `sudo make install` (Optional)
      * Installs the libvibra static, shared libraries and the vibra command-line tool.

#### Usage
<details>
<summary>Use --help option to see the help message.</summary>

```
vibra {COMMAND} [OPTIONS]

Options:

  Commands:
      -F, --fingerprint                     Generate a fingerprint
      -R, --recognize                       Recognize a song
      -h, --help                            Display this help menu
  Sources:
      File sources:
          -f, --file                            File path
      Raw PCM sources:
          -s, --seconds                         Chunk seconds
          -r, --rate                            Sample rate
          -c, --channels                        Channels
          -b, --bits                            Bits per sample
```

</details>

##### Recognizing a song from a WAV file
```bash
vibra --recognize --file sample.wav > result.json

jq .track.title result.json
"Stairway To Heaven"
jq .track.subtitle result.json
"Led Zeppelin"
jq .track.share.href result.json
"https://www.shazam.com/track/5933917/stairway-to-heaven"
```

##### Recognizing a song from a microphone
* You can use [sox](http://sox.sourceforge.net/) or [FFmpeg](https://ffmpeg.org/) to capture raw PCM data from the **microphone**.
* **sox**
```bash
sox -d -t raw -b 24 -e signed-integer -r 44100 -c 1 - 2>/dev/null
| vibra --recognize --seconds 5 --rate 44100 --channels 1 --bits 24 > result.json
```

* **FFmpeg**
```bash
ffmpeg -f avfoundation -i ":2" -f s32le -ar 44100 -ac 1 - 2>/dev/null
| vibra --recognize --seconds 5 --rate 44100 --channels 1 --bits 32 > result.json
# - "avfoundation" can be replaced depending on your system.
# - Make sure to use the correct device index for your system.
```
* **output**
```bash
jq .track.title result.json
"Bound 2"
jq .track.subtitle result.json
"Kanye West"
jq .track.sections[1].text result.json
[
  "B-B-B-Bound to fall in love",
  "Bound to fall in love",
  "(Uh-huh, honey)",
  ...
]
```

##### Recognizing non-WAV files
* To decode non-WAV media files, FFmpeg must be installed on your system.
* Vibra will attempt to locate FFmpeg in your system's PATH environment variable. If you prefer, you can explicitly specify the FFmpeg path by setting the `FFMPEG_PATH` environment variable.
```bash
# Automatically find FFmpeg in PATH
vibra --recognize --file sample.mp3

# Specify the FFmpeg path
export FFMPEG_PATH=/opt/homebrew/bin/ffmpeg
vibra --recognize --file sample.mp3
# You can use your own FFmpeg which is optimized for your system.
```
* You can see the sample shazam result json file in [here](https://gist.github.com/BayernMuller/b92fd43eef4471b7016009196e62e4d2)

### FFI Bindings
* vibra provides FFI bindings, allowing other languages to leverage its music recognition functionality.
* After building vibra, the shared library `libvibra.so` will be located in the `build` directory.
* This shared library can be integrated into languages such as Python or Swift using FFI mechanisms.
* For detailed function signatures, please refer to the vibra header file [vibra.h](include/vibra.h).


### Performance comparison
<p align="center">
    <img src="docs/project_vibra_benchmark.png" width="700"/><br/>
    lower is better
</p>

* I compared the performance of vibra with the [SongRec](https://github.com/marin-m/SongRec/tree/master) rust and python version on the Raspberry Pi 4.
* vibra is about 2 times faster than the SongRec!

### License
* vibra is licensed under the GPLv3 license. See [LICENSE](LICENSE) for more details.


[linux-amd64-main]: https://github.com/bayernmuller/vibra/actions/workflows/build-linux-amd64.yaml/badge.svg?branch=main
[linux-arm64-main]: https://github.com/bayernmuller/vibra/actions/workflows/build-linux-arm64.yaml/badge.svg?branch=main

[macos-amd64-main]: https://github.com/bayernmuller/vibra/actions/workflows/build-macos-amd64.yaml/badge.svg?branch=main
[macos-arm64-main]: https://github.com/bayernmuller/vibra/actions/workflows/build-macos-arm64.yaml/badge.svg?branch=main

[windows-amd64-main]: https://github.com/bayernmuller/vibra/actions/workflows/build-windows-amd64.yaml/badge.svg

[webassembly-main]: https://github.com/bayernmuller/vibra/actions/workflows/build-webassembly.yaml/badge.svg?branch=main
