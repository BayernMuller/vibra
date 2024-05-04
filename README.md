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
* vibra is a C++ music file recognition tool that uses an unofficial Shazam API.
* vibra analyzes music files and returns fingerprint data. It searches the Shazam database to identify the song.
* The Shazam algorithm is based on:
    * [An Industrial-Strength Audio Search Algorithm](https://www.ee.columbia.edu/~dpwe/papers/Wang03-shazam.pdf)
    * [How does Shazam work](https://www.cameronmacleod.com/blog/how-does-shazam-work)
* I referenced the Rust implementation of the Shazam client code from [SongRec](https://github.com/marin-m/SongRec/tree/master).
* I created this project for embedded devices such as Raspberry Pi or Jetson Nano, which are challenging to set up with Python or Rust environments.

### Compatibility table

| Platform         |     Architecture | Build Status     |
|------------------|------------------|------------------|
| [WebAssembly](wasm/readme.md) | -                | ✅                |
| Linux            | x86_64           | ✅                |
| Linux            | ARM64            | ✅                |
| Windows          | x86_64           | ✅                |
| macOS            | x86_64           | ✅                |
| macOS            | ARM64            | ✅                |
| Raspberry Pi 4   | ARMv8-A          | ✅                |
| Raspberry Pi Zero| ARMv6            | ✅                |


### Build vibra

#### Requirements

* vibra uses CMake for building. You need to install [CMake](https://cmake.org/) to build vibra.
* vibra utilizes C++11 features.
* vibra depends on the following libraries:
    * [libcurl](https://curl.se/libcurl/): To send HTTP requests to the Shazam API.
    * [libfftw3](http://www.fftw.org/): To calculate the Fast Fourier Transform.
    * (Optional) [FFmpeg](https://ffmpeg.org/): To decode audio files other than WAV. (e.g., MP3, FLAC, etc.)
        * If you want to want recognize non-WAV files, you need to install FFmpeg on your system.

#### Build
* Clone repository **recursively** to include submodules.
    * `git clone --recursive https://github.com/bayernmuller/vibra.git`

* Run the following commands to build vibra:
    * `mkdir build && cd build`
    * `cmake ..`
    * `make`

#### Usage
* Use `vibra -h` to see the help message.

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

##### - recognizing song from wav file
```bash
$ vibra --recognize --file sample.wav > result.json
$ jq .track.title result.json
"Stairway To Heaven"
$ jq .track.subtitle result.json
"Led Zeppelin"
$ jq .track.share.href result.json
"https://www.shazam.com/track/5933917/stairway-to-heaven"
```

##### - recognizing song from microphone
* You can use [sox](http://sox.sourceforge.net/) or [FFmpeg](https://ffmpeg.org/) to print raw PCM data from **microphone**.

```bash
$ sox -d -t raw -b 24 -e signed-integer -r 44100 -c 1 - 2>/dev/null
  | vibra --recognize --seconds 5 --rate 44100 --channels 1 --bits 24 > result.json

$ jq .track.title result.json
"Bound 2"
$ jq .track.subtitle result.json
"Kanye West"
$ jq .track.sections[1].text result.json
[
  "B-B-B-Bound to fall in love",
  "Bound to fall in love",
  "(Uh-huh, honey)",
  "",
  "All them other niggas lame, and you know it now",
...
```

##### - recognizing non-WAV files
* You need to install FFmpeg on your system to decode non-WAV media files.
* Vibra will try to locate FFmpeg in your PATH environment variable. Alternatively, you can specify the FFmpeg path by setting the `FFMPEG_PATH` environment variable.
```bash
# Automatically find FFmpeg
vibra --recognize --file out.mp3

# Specify the FFmpeg path
export FFMPEG_PATH=/opt/homebrew/bin/FFmpeg
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

### WebAssembly Version Support
* Please read [wasm/readme.md](wasm/readme.md) to build vibra webassembly version.

### Demo Video
* Real time music tracker and lighting solution "Mooding", which uses vibra.
* Click to view!

[![Mooding](https://img.youtube.com/vi/f88EUEIohI4/maxresdefault.jpg)](https://www.youtube.com/watch?v=f88EUEIohI4)

### License
* vibra is licensed under the GPLv3 license. See [LICENSE](LICENSE) for more details.