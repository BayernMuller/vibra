### vibra: WebAssembly Version Support

* vibra now supports a WebAssembly (WASM) build, enabling it to run in web browsers
* You can compile the project into WebAssembly using Docker.

### Building the WebAssembly version vibra with Docker

1. Install Docker on your system if you haven't already. Visit the official Docker website for installation instructions specific to your operating system.

2. Clone the vibra repository:
```bash
git clone https://github.com/bayernmuller/vibra
```

3. Open a terminal and navigate to the `vibra` directory
```bash
cd vibra
```

4. Build the Docker image:
```bash
docker build -f wasm/Dockerfile -t vibra-wasm . 
```

5. Run the Docker container:
```bash
docker run --rm -v $(pwd)/wasm/build:/app/wasm/build vibra-wasm
```

6. The compiled files will be located in the `wasm/build` directory.
```bash
$ ls -l wasm/build
.rw-r--r-- jayden staff  88 KB Wed Sep 11 23:07:52 2024 vibra.js
.rwxr-xr-x jayden staff 804 KB Wed Sep 11 23:07:52 2024 vibra.wasm
```