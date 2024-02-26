### WebAssembly Version Support

Vibra now supports a WebAssembly (WASM) build, enabling it to run in web browsers. Before you can build Vibra for WebAssembly, you must first compile libfftw3 for WebAssembly.

#### Building Dependency: libfftw3 for WebAssembly

To build libfftw3 on macOS for use with Vibra, follow these steps:

1. Download the libfftw3 source code:
```bash
wget http://www.fftw.org/fftw-3.3.10.tar.gz

