<p align="center">
    <img src="docs/project_vibra.png" width="400"/>
    <br>
    <img src='docs/logo_license.svg'>
</p>

<span align="center">

# vibra

</span>


<p align="center">
    <img src="https://github.com/bayernmuller/vibra/actions/workflows/ci.yaml/badge.svg"/>
    <img src="https://img.shields.io/github/last-commit/bayernmuller/vibra"/>
    <img src="https://img.shields.io/github/license/bayernmuller/vibra"/>
</p>

### What is vibra?
* vibra is a powerful C++ library and command-line tool for music recognition, leveraging an unofficial Shazam API.
* It efficiently analyzes audio files, generates unique fingerprints, and queries the vast Shazam database to identify songs with high accuracy.
* Key features of vibra:
    * Fast and lightweight: Optimized for performance on various platforms, including embedded devices.
    * Cross-platform compatibility: Supports Linux, Windows, macOS, and **WebAssembly**.
    * Flexible input: Can process WAV files natively, with optional FFmpeg support for other audio formats.
* The core algorithm is based on Shazam's groundbreaking approach:
    * [An Industrial-Strength Audio Search Algorithm](https://www.ee.columbia.edu/~dpwe/papers/Wang03-shazam.pdf) - The original research paper.
    * [How does Shazam work](https://www.cameronmacleod.com/blog/how-does-shazam-work) - A detailed explanation of the algorithm.
* Implementation inspiration:
    * The project references the Rust implementation from [SongRec](https://github.com/marin-m/SongRec/tree/master), adapting it to C++ for broader compatibility.
* Target platforms:
    * Specifically designed for embedded devices like Raspberry Pi and Jetson Nano, where setting up Python or Rust environments can be challenging.
    * Equally effective on desktop and server environments for high-performance audio recognition tasks.


### Compatibility table

| Platform         | Architecture     | Build Status |
|------------------|------------------|--------------|
| [WebAssembly](wasm/README.md) | -    | ✅            |
| Linux            | x86_64, ARM64    | ✅            |
| Windows          | x86_64           | ✅            |
| macOS            | x86_64, ARM64    | ✅            |
| Raspberry Pi     | ARMv8-A, ARMv6   | ✅            |


### Build vibra

#### WebAssembly Version
* Please read **[wasm/README.md](wasm/README.md)** to build and run vibra webassembly version.


#### Native Version
##### Requirements

* vibra requires CMake for its build process. Install [CMake](https://cmake.org/) before building.
* The project is built using **C++11** standard features.
* vibra has the following dependencies:
    * [CMake](https://cmake.org/): Build system generator.
    * [libcurl](https://curl.se/libcurl/): Handles HTTP requests to the Shazam API.
    * [libfftw3](http://www.fftw.org/): Performs Fast Fourier Transform calculations.
    * [FFmpeg](https://ffmpeg.org/) (Optional): Enables support for non-WAV audio formats (e.g., MP3, FLAC).
        * Install FFmpeg if you need to process audio files other than WAV.

##### Install dependencies
* Ubuntu
    * `sudo apt-get update`
    * `sudo apt-get install cmake libcurl4-openssl-dev libfftw3-dev`
    * `sudo apt-get install ffmpeg` (Optional)

##### Build
* Clone repository **recursively** to include submodules.
    * `git clone --recursive https://github.com/bayernmuller/vibra.git`

* Run the following commands to build vibra:
    * `mkdir build && cd build`
    * `cmake ..`
    * `make`

##### Usage
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

##### - recognizing song from wav file
```bash
vibra --recognize --file sample.wav > result.json

jq .track.title result.json
"Stairway To Heaven"
jq .track.subtitle result.json
"Led Zeppelin"
jq .track.share.href result.json
"https://www.shazam.com/track/5933917/stairway-to-heaven"
```

##### - recognizing song from microphone
* You can use [sox](http://sox.sourceforge.net/) or [FFmpeg](https://ffmpeg.org/) to print raw PCM data from **microphone**.

```bash
sox -d -t raw -b 24 -e signed-integer -r 44100 -c 1 - 2>/dev/null
| vibra --recognize --seconds 5 --rate 44100 --channels 1 --bits 24 > result.json

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

##### - recognizing non-WAV files
* You need to install FFmpeg on your system to decode non-WAV media files.
* Vibra will try to locate FFmpeg in your PATH environment variable. Alternatively, you can specify the FFmpeg path by setting the `FFMPEG_PATH` environment variable.
```bash
# Automatically find FFmpeg in PATH
vibra --recognize --file out.mp3

# Specify the FFmpeg path
export FFMPEG_PATH=/opt/homebrew/bin/ffmpeg
vibra --recognize --file out.mp3
```


* You can see the sample shazam result json file in [here](https://gist.github.com/BayernMuller/b92fd43eef4471b7016009196e62e4d2)

### Performance comparison
<p align="center">
    <img src="docs/project_vibra_benchmark.png" width="700"/><br/>
    lower is better
</p>

* I compared the performance of vibra with the [SongRec](https://github.com/marin-m/SongRec/tree/master) rust and python version on the Raspberry Pi 4.
* vibra is about 2 times faster than the SongRec!

### Demo Video
* Real time music tracker and lighting solution "Mooding", which uses vibra.
* Click to view!

[![Mooding](https://img.youtube.com/vi/f88EUEIohI4/maxresdefault.jpg)](https://www.youtube.com/watch?v=f88EUEIohI4)

### License
* vibra is licensed under the GPLv3 license. See [LICENSE](LICENSE) for more details.