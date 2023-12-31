# 🎵 vibra

### What is vibra?
* vibra is a C++ music file recognition tool that uses an unofficial Shazam API.
* vibra analyzes music files and returns fingerprint data. It searches the Shazam database to identify the song.
* The Shazam algorithm is based on:
    * [An Industrial-Strength Audio Search Algorithm](https://www.ee.columbia.edu/~dpwe/papers/Wang03-shazam.pdf)
    * [How does Shazam work](https://www.cameronmacleod.com/blog/how-does-shazam-work)
* I referenced the Rust implementation of the Shazam client code from [SongRec](https://github.com/marin-m/SongRec/tree/master).
* I created this project for embedded devices such as Raspberry Pi or Jetson Nano, which are challenging to set up with Python or Rust environments.

### Run vibra

##### Requirements

* vibra uses CMake for building. You need to install [CMake](https://cmake.org/) to build vibra.
* vibra utilizes C++14 features. You need to install gcc-5 or a higher version.
* vibra depends on the following libraries:
    * [libcurl](https://curl.se/libcurl/): To send HTTP requests to the Shazam API.
    * [libfftw3](http://www.fftw.org/): To calculate the Fast Fourier Transform.

##### Build
* You can build vibra with CMake using the following commands:
    * `mkdir build && cd build`
    * `cmake ..`
    * `make`

* If you want to specify the paths for libraries or include files, you can use the commands below:
    * `cmake -DINCLUDE_PATH=/path/to/include`
    * `cmake -DLIBRARY_PATH=/path/to/lib`
    

##### Run
* Use `./vibra help` to see the help message.

```
Usage: vibra <action> <param>

* Actions:
        fingerprinting-by-file <param>
        recognize-song-by-file <param>
```

##### Examples
```bash
$ ./vibra recognize-song-by-file "stairway_to_heaven.wav" > result.json
$ jq .track.title result.json
"Stairway To Heaven"
$ jq .track.subtitle result.json
"Led Zeppelin"
$ jq .track.share.href result.json
"https://www.shazam.com/track/5933917/stairway-to-heaven"
```
* You can see more metadata in the result json.

### License
* vibra is licensed under the GPLv3 license. See [LICENSE](LICENSE) for more details.
