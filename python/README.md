# vibra Python

Python bindings for vibra fingerprint generation.

This package does not use pybind11. It builds the native `libvibra` shared
library with CMake and calls the public C ABI through `ctypes`.

## Build

```bash
python3 -m pip install .
```

## Usage

```python
from pathlib import Path

from vibra import Vibra

vibra = Vibra()
signature = vibra.get_signature(Path("sample.wav"))

print(signature.uri)
print(signature.sample_ms)
```

Raw PCM input is also supported:

```python
from vibra import get_signature_from_signed_pcm

signature = get_signature_from_signed_pcm(
    pcm_bytes,
    sample_rate=16000,
    sample_width=16,
    channel_count=1,
)
```

## Shazam recognition

The Shazam communication adapter uses `requests` and is separate from the core
fingerprint wrapper:

```python
from vibra import Shazam, Vibra

signature = Vibra().get_signature("sample.wav")
result = Shazam.recognize(signature)

print(result["track"]["title"])
```

Non-WAV file input uses vibra's native FFmpeg lookup. Install FFmpeg or set
`FFMPEG_PATH` when using formats such as MP3 or FLAC.
