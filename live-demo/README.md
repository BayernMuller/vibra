<span align="center">

# vibra live demo

</span>

* **Try it out: https://bayernmuller.github.io/vibra/**
* [vibra](https://github.com/BayernMuller/vibra)'s WebAssembly version running fully in the browser — fingerprinting happens locally, only the signature is sent to the recognition proxy.
* Deployed automatically to GitHub Pages by [`pages-live-demo.yaml`](../.github/workflows/pages-live-demo.yaml) on every push to `main`, which builds `vibra.js` / `vibra.wasm` fresh with Emscripten.

### Running locally

```bash
# vibra.js / vibra.wasm are built by CI; build them with js/build-wasm.sh
# (requires emsdk) or copy them from the deployed site into public/.
python3 -m http.server 8765
# then open http://localhost:8765
```
