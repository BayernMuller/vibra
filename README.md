<p align="center">
    <img src="docs/project_vibra.png" width="400"/>
    <br>
    <img src='docs/logo_license.svg'>
</p>

<span align="center">

# vibra

</span>

<p align="center">
    <img src="https://img.shields.io/github/stars/bayernmuller/vibra?style=social" alt="GitHub stars"/>
    <img src="https://img.shields.io/github/last-commit/bayernmuller/vibra"/>
    <img src="https://img.shields.io/github/license/bayernmuller/vibra"/>
</p>

### Overview

* **vibra** is a library and CLI tool for music recognition using the **unofficial** Shazam API.
* It analyzes audio files, generates fingerprints, and queries Shazam's database for song identification.
* **Key features**:
    * **Smart Segment Selection**: Automatically analyzes audio to find the most distinctive part for fingerprinting, significantly improving recognition accuracy.
    * Fast and lightweight, optimized for various platforms, including embedded devices.
    * Cross-platform support: Linux, Windows, macOS, **WebAssembly**, and **Python**.
    * Flexible input processing: native support for WAV files, optional FFmpeg for other formats.
* **Based on Shazam's algorithm**:
    * [An Industrial-Strength Audio Search Algorithm](https://www.ee.columbia.edu/~dpwe/papers/Wang03-shazam.pdf)
    * [How does Shazam work](https://www.cameronmacleod.com/blog/how-does-shazam-work)
* **Inspired by** [SongRec](https://github.com/marin-m/SongRec/tree/master), adapted to C++ 11.
* **Target platforms**:
    * Embedded devices (e.g., Raspberry Pi, Jetson Nano)
    * Desktop and server environments for high-performance recognition
    * WebAssembly for web-based use
    * Python applications through the ctypes-based Python package
    * Additional support for iOS, Android, and other languages through the public C ABI

### Smart Segment Selection

vibra features an intelligent audio analysis system that automatically selects the optimal segment for fingerprinting:

* **Multi-position Analysis**: Tests 3 strategic positions in each audio file:
    * 5 seconds (skips fade-ins and silence)
    * 30 seconds (skips typical intros)
    * 50% point (middle of song, often the chorus)

* **Audio Quality Scoring**: Each segment is scored based on:
    * **RMS Energy**: Measures loudness and activity level
    * **Spectral Variance**: Measures frequency complexity and instrument diversity
    * Combined weighted score (60% energy + 40% variance)

* **Best Segment Selection**: Automatically fingerprints the highest-scoring segment, ensuring:
    * Better recognition of songs with quiet intros or outros
    * More accurate matching by analyzing distinctive parts
    * Improved success rate compared to fixed-offset approaches

* **Output Transparency**: The JSON response includes `vibra_offset_ms` field showing where fingerprinting started:
```json
{
  "matches": [...],
  "track": {...},
  "vibra_offset_ms": 30000
}
```

This smart selection dramatically improves recognition accuracy without requiring any user configuration.

### Precise Mode (Multi-Segment Recognition)

For difficult tracks (mashups, remixes, songs with long intros), vibra offers a precise mode that checks multiple segments and uses voting:

```bash
vibra --recognize --precise --file song.mp3
```

* **Adaptive Skip**: Skips intro based on song duration (3s for short, 10s for medium, 15s for long songs)
* **Sequential Segments**: Sends up to 5 segments of 12 seconds each
* **Early Termination**: Returns immediately when 2 consecutive segments match the same song
* **Voting System**: If 2 out of 3+ segments match, returns confident result
* **Connection Reuse**: All requests use the same HTTP connection for speed

The response includes confidence information:
```json
{
  "track": {...},
  "vibra_segments_checked": 2,
  "vibra_confident": true
}
```

### Apple Music Enrichment

Fetch additional metadata from Apple Music for richer results:

```bash
vibra --recognize --apple-music --file song.mp3
```

This adds:
* **Exact release date** (not just year)
* **Track duration** in ISO 8601 format
* **Genre array** (multiple genres)
* **Individual artists** (separated from collaborations)
* **High-resolution artwork** (1200x630)
* **Preview audio URL**

### Unified JSON Output

Get a clean, standardized JSON response with all metadata organized:

```bash
vibra --recognize --precise --apple-music --unified --file song.mp3
```

Output structure:
```json
{
  "status": "success",
  "result": {
    "title": "Song Title",
    "artist": "Main Artist",
    "full_artist": "Main Artist & Featured Artist",
    "feat_artists": ["Featured Artist"],
    "album": "Album Name",
    "label": "Record Label",
    "year": 2024,
    "genre": "Pop",
    "isrc": "USRC12345678",
    "release_date": "2024-01-15",
    "duration": "PT3M45S",
    "genres": ["Pop", "Dance"],
    "images": {
      "coverart": "...",
      "coverart_hq": "...",
      "background": "...",
      "large": "..."
    },
    "external_ids": {
      "shazam": "123456",
      "shazam_artist": "789",
      "apple_music": "1234567890",
      "apple_music_album": "9876543210",
      "apple_music_artist": "456789123"
    },
    "links": {
      "shazam": "https://www.shazam.com/track/...",
      "apple_music": "https://music.apple.com/song/...",
      "preview": "https://audio-ssl.itunes.apple.com/...",
      "spotify": "spotify:search:...",
      "youtube_music": "https://music.youtube.com/search?q=...",
      "deezer": "deezer-query://..."
    },
    "match": {
      "offset": 27.91,
      "timeskew": -0.00014,
      "frequencyskew": 0.0
    },
    "related_tracks_url": "https://cdn.shazam.com/...",
    "request": {
      "timestamp": 1234567890000,
      "timezone": "Europe/London",
      "location": {"latitude": 51.5, "longitude": -0.1, "altitude": 100}
    },
    "vibra": {
      "segments_checked": 2,
      "confident": true
    }
  }
}
```

### Tor Support

Use Tor for anonymous recognition (requires Tor service running on port 9050):

```bash
vibra --recognize --tor --file song.mp3
```

* Uses SOCKS5 proxy at 127.0.0.1:9050
* Automatically requests new circuit after each recognition
* Shows exit IP in response

**Note**: Shazam may block some Tor exit nodes.

### Live Demo
* You can try the music recognition with the **[WebAssembly version of vibra here](https://bayernmuller.github.io/vibra-live-demo/)**
* The source code for the demo is available at [vibra-live-demo](https://github.com/bayernmuller/vibra-live-demo)

### Platform Compatibility and Build Status

| Platform | Status | Workflows |
|--------|--------|--------|
| **Code Checks** | [![code-main]][code-main] | [ci-code] |
| **Linux AMD64** | [![linux-amd64-main]][linux-amd64-main] | [ci-linux-amd64] |
| **Linux ARM64** | [![linux-arm64-main]][linux-arm64-main] | [ci-linux-arm64] |
| **MacOS ARM64** | [![macos-arm64-main]][macos-arm64-main] | [ci-macos-arm64] |
| **Windows AMD64** | [![windows-amd64-main]][windows-amd64-main] | [ci-windows-amd64] |
| **WebAssembly** | [![webassembly-main]][webassembly-main] | [ci-webassembly] |
| **Python** | [![python-main]][python-main] | [ci-python] |

### Building the WebAssembly Version
* Please refer to **[js/README.md](js/README.md)** for instructions on building and running the JavaScript/WebAssembly version of vibra.

### Building the Python Package
* Please refer to **[python/README.md](python/README.md)** for instructions on building and using the Python package.

### Building the Native Version

#### Prerequisites
* vibra requires CMake for its build process. Please install [CMake](https://cmake.org/) before proceeding.
* The project is developed using the **C++11** standard.
* vibra has the following dependencies:
    * [CMake](https://cmake.org/): A cross-platform build system generator.
    * [libcurl](https://curl.se/libcurl/) (CLI tool only): A library for transferring data with URLs.
      * If you don't need CLI tool, libcurl is not required.
      * You can disable it by setting the `-DLIBRARY_ONLY=ON` option in the CMake command.
    * [FFmpeg](https://ffmpeg.org/) (Optional): Provides support for audio formats other than WAV (e.g., MP3, FLAC).
        * Install FFmpeg if you need to process audio files in formats other than WAV.

#### Install dependencies
* **Ubuntu**
    * `sudo apt-get update`
    * `sudo apt-get install cmake libcurl4-openssl-dev`
    * `sudo apt-get install ffmpeg` (Optional)
* **Windows**
    * Install [CMake](https://cmake.org/download/)
    * Install [vcpkg](https://github.com/Microsoft/vcpkg)
    * Install dependencies using vcpkg:
        * `vcpkg install curl:x64-windows`
    * Add the vcpkg toolchain file to your CMake command (see Build section)
    * Install [FFmpeg](https://ffmpeg.org/download.html#build-windows) (Optional)
* **macOS**
    * Install [Homebrew](https://brew.sh/)
    * `brew install cmake curl`
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
      -B, --bulk                            Bulk recognize all audio files in a directory
      -h, --help                            Display this help menu
  Sources:
      File sources:
          -f, --file                            File path
          -d, --dir                             Directory path for bulk recognition
      Raw PCM sources:
          -s, --seconds                         Chunk seconds
          -r, --rate                            Sample rate
          -c, --channels                        Channels
          -b, --bits                            Bits per sample
  Bulk options:
      -o, --output                          Output JSON file path (default: results.json)
      -t, --threads                         Number of parallel threads (default: 1)
      -w, --delay                           Delay in seconds after each file (default: 2)
      --resume                              Resume from previous run (skip already processed files)
  Proxy options:
      --proxy-host                          Proxy host address
      --proxy-port                          Proxy port (default: 8080)
      --proxy-user                          Proxy username
      --proxy-pass                          Proxy password
      --proxy-type                          Proxy type: http or socks5 (default: http)
      --proxy-rotation-url                  URL to fetch new proxy from for rotation
      --tor                                 Use Tor as proxy (SOCKS5 on 127.0.0.1:9050)
  Recognition options:
      --precise                             Use multiple segments for more accurate recognition
      --apple-music                         Fetch additional metadata from Apple Music
      --unified                             Output clean unified JSON format
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

##### Bulk recognition for music libraries
* vibra supports bulk processing of entire music directories with advanced features:
  * **Parallel processing** with configurable thread count
  * **Resume capability** to skip already-processed files
  * **Progress tracking** with real-time statistics
  * **Automatic rate limiting** with exponential backoff
  * **Configurable delays** to avoid API rate limits
  * **JSON output** with detailed results and statistics

**Basic bulk recognition:**
```bash
vibra --bulk --dir /path/to/music/folder
```

**Advanced options:**
```bash
# Parallel processing with 4 threads
vibra --bulk --dir ./music --threads 4

# Custom output file
vibra --bulk --dir ./music --output results.json

# Resume interrupted processing
vibra --bulk --dir ./music --resume

# Custom delay between files (default: 2 seconds)
vibra --bulk --dir ./music --delay 5

# Complete example with all options
vibra --bulk --dir ./music --output songs.json --threads 4 --delay 3 --resume
```

**Output format:**
```json
{
  "results": [
    {
      "file": "/path/to/song.mp3",
      "success": true,
      "response": { ...full Shazam JSON response... }
    },
    {
      "file": "/path/to/unknown.mp3",
      "success": false,
      "error": "Rate limited by Shazam API"
    }
  ],
  "stats": {
    "total": 100,
    "processed": 100,
    "successful": 95,
    "failed": 5,
    "skipped": 0
  }
}
```

**Supported formats:** MP3, WAV, FLAC, OGG, M4A, AAC

**Rate limiting:**
* vibra automatically handles Shazam API rate limits with progressive backoff (30/60/120 seconds)
* All threads pause during rate limit cooldown
* Processing stops gracefully after 3 rate limit attempts
* Use `--delay` to proactively avoid rate limiting

**Proxy support:**
vibra supports HTTP and SOCKS5 proxies for bulk recognition to avoid IP-based rate limiting:

```bash
# Basic HTTP proxy
vibra --bulk --dir ./music --proxy-host proxy.example.com --proxy-port 8080

# HTTP proxy with authentication
vibra --bulk --dir ./music --proxy-host proxy.example.com --proxy-port 8080 \
      --proxy-user myuser --proxy-pass mypassword

# SOCKS5 proxy
vibra --bulk --dir ./music --proxy-host proxy.example.com --proxy-port 1080 \
      --proxy-type socks5

# Proxy rotation with URL (automatically fetches new proxy from URL)
vibra --bulk --dir ./music --proxy-rotation-url https://api.example.com/get-proxy

# Complete example with proxy and all options
vibra --bulk --dir ./music --output results.json --threads 4 --delay 3 \
      --proxy-host proxy.example.com --proxy-port 8080 --proxy-user user --proxy-pass pass
```

**Proxy rotation:**
* Use `--proxy-rotation-url` to fetch proxy configuration from an external URL
* The URL should return a proxy string in format: `[type://][user:pass@]host:port`
* Examples: `http://proxy.com:8080`, `socks5://user:pass@proxy.com:1080`
* Proxy is fetched once at startup from the rotation URL

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

### Contribution

#### Unit tests
* Unit tests are built with [GoogleTest](https://github.com/google/googletest), fetched by CMake.
* Native CI builds release artifacts with `-DBUILD_TESTING=OFF`, then configures a separate test build with `-DBUILD_TESTING=ON`.
* Run the following commands to build and run unit tests:
    * `cmake -B build-test -DCMAKE_BUILD_TYPE=Debug -DLIBRARY_ONLY=ON -DBUILD_TESTING=ON`
    * `cmake --build build-test --target vibra_unit_tests`
    * `ctest --test-dir build-test --output-on-failure`

#### Code checks
* Code CI runs `cpplint` and `clang-format`.
* Run the following commands locally:
    * `cpplint --recursive lib include tests/algorithm tests/audio tests/utils tests/public_api_test.cpp`
    * `find include lib tests -path tests/e2e -prune -o \( -name '*.h' -o -name '*.cpp' -o -name '*.cc' -o -name '*.c' \) -print0 | xargs -0 clang-format --dry-run --Werror`

### License
* vibra is licensed under the GPLv3 license. See [LICENSE](LICENSE) for more details.



[code-main]: https://github.com/bayernmuller/vibra/actions/workflows/ci-code.yaml/badge.svg
[ci-code]: https://github.com/bayernmuller/vibra/tree/main/.github/workflows/ci-code.yaml

[linux-amd64-main]: https://github.com/bayernmuller/vibra/actions/workflows/ci-linux-amd64.yaml/badge.svg
[ci-linux-amd64]: https://github.com/bayernmuller/vibra/tree/main/.github/workflows/ci-linux-amd64.yaml

[linux-arm64-main]: https://github.com/bayernmuller/vibra/actions/workflows/ci-linux-arm64.yaml/badge.svg
[ci-linux-arm64]: https://github.com/bayernmuller/vibra/tree/main/.github/workflows/ci-linux-arm64.yaml

[macos-arm64-main]: https://github.com/bayernmuller/vibra/actions/workflows/ci-macos-arm64.yaml/badge.svg
[ci-macos-arm64]: https://github.com/bayernmuller/vibra/tree/main/.github/workflows/ci-macos-arm64.yaml

[windows-amd64-main]: https://github.com/bayernmuller/vibra/actions/workflows/ci-windows-amd64.yaml/badge.svg
[ci-windows-amd64]: https://github.com/bayernmuller/vibra/tree/main/.github/workflows/ci-windows-amd64.yaml

[webassembly-main]: https://github.com/bayernmuller/vibra/actions/workflows/ci-webassembly.yaml/badge.svg
[ci-webassembly]: https://github.com/bayernmuller/vibra/tree/main/.github/workflows/ci-webassembly.yaml

[python-main]: https://github.com/bayernmuller/vibra/actions/workflows/ci-python.yaml/badge.svg
[ci-python]: https://github.com/bayernmuller/vibra/tree/main/.github/workflows/ci-python.yaml
