### WebAssembly Version Support

* Vibra now supports a WebAssembly (WASM) build, enabling it to run in web browsers. Before you can build Vibra for WebAssembly, you must first compile `libfftw3` for WebAssembly.

#### Building Dependency: `libfftw3` for WebAssembly

* To build `libfftw3` on macOS for use with Vibra, follow these steps:

1. Download the `libfftw3` source code:
```bash
wget http://www.fftw.org/fftw-3.3.10.tar.gz
```
2. Extract the downloaded file:
```bash
tar -xvf fftw-3.3.10.tar.gz
```
3. Navigate into the extracted directory:
```bash
cd fftw-3.3.10
```
4. Configure the build for WebAssembly without Fortran support and specify the installation prefix:
```bash
emconfigure ./configure --disable-fortran --enable-single --prefix="/path/to/fftw"
```
5. Compile the library using multiple cores (for example, using 9 cores):
```bash
emmake make -j9
```

* After completing these steps, you will find the WebAssembly version of `libfftw3` in the `/path/to/fftw` directory.

#### Building vibra for WebAssembly

* Once `libfftw3` is ready, you can proceed to build Vibra for WebAssembly:

1. Navigate to the wasm directory within the Vibra project:
```bash
cd vibra/wasm
```
2. Set the FFTW3_PATH environment variable to the path where `libfftw3` was installed:
```bash
export FFTW3_PATH=/path/to/fftw
```
3. Compile Vibra using the provided Makefile for WebAssembly, ensuring to include the FFTW3 include path:
```bash
emmake make -f Makefile.wasm -I"$FFTW3_PATH/include"
```
4. Verify the build outputs:
```bash
ls build
```
* You should see the following files indicating a successful build:
    * `vibra.js`
    * `vibra.wasm`

* These files constitute the WebAssembly version of Vibra, ready for integration into web applications.