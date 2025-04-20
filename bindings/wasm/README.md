<p align="center">
    <img width="20%" src="https://upload.wikimedia.org/wikipedia/commons/1/1f/WebAssembly_Logo.svg" alt="WebAssembly Logo"/>
</p>

### 1. Building the WebAssembly Version of Vibra Using Docker

Follow these steps to build the WebAssembly version of Vibra:

1. Ensure Docker is installed on your system. If not, visit the official Docker website for installation instructions specific to your operating system.

2. Clone the Vibra repository:
   ```bash
   git clone https://github.com/bayernmuller/vibra
   ```

3. Navigate to the Vibra directory:
   ```bash
   cd vibra
   ```

4. Build the Docker image:
   ```bash
   docker build -f bindings/wasm/Dockerfile -t vibra-wasm .
   ```

5. Run the Docker container to compile the WebAssembly files:
   ```bash
   docker run --rm -v $(pwd):/app vibra-wasm bash -c "cd bindings/wasm && ./build-wasm.sh"
   ```

6. After compilation, you'll find the following files in the `wasm/build` directory:
   ```bash
   $ ls -l bindings/wasm/build
   .rw-r--r-- jayden staff  88 KB Wed Sep 11 23:07:52 2024 vibra.js
   .rwxr-xr-x jayden staff 804 KB Wed Sep 11 23:07:52 2024 vibra.wasm
   ```

##### If you can't build the WebAssembly version, you can download `vibra.wasm` and `vibra.js` from GitHub workflow artifacts.
* https://github.com/BayernMuller/vibra/actions/workflows/build-webassembly.yaml
* Please download `vibra.wasm` and `vibra.js` from the latest successful run.
* and put them in the `vibra/bindings/wasm/build` directory to run the demo Node.js app.

### 2. Running the Demo Node.js App "vibrajs"

To run the demo Node.js application:

1. Install dependencies and build the CLI app:
   ```bash
   cd bindings/wasm
   npm install
   npm run build
   npm link
   ```

2. Use the CLI app to recognize a song:
   ```bash
   $ vibrajs "stairway_to_heaven.wav" | jq .track.subtitle
   "Led Zeppelin"
   ```

This example demonstrates how to use Vibrajs to identify the artist of the song "Stairway to Heaven".