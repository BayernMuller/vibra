from __future__ import annotations

import ctypes
import os
import sys
from importlib import resources
from os import PathLike
from pathlib import Path
from typing import TYPE_CHECKING, List, Optional, Union

if TYPE_CHECKING:
    from ._api import Signature

BytesLike = Union[bytes, bytearray, memoryview]
PathValue = Union[str, PathLike]


class NativeVibra:
    def __init__(self, library_path: Optional[Union[str, PathLike]] = None) -> None:
        self._library = ctypes.CDLL(str(library_path or _find_library()))
        self._configure_functions()

    def get_signature(self, path: PathValue) -> "Signature":
        path_bytes = os.fsencode(path)
        pointer = self._library.vibra_get_fingerprint_from_music_file(path_bytes)
        return self._take_signature(pointer)

    def get_signature_from_wav_data(self, raw_wav: BytesLike) -> "Signature":
        data = _as_bytes(raw_wav)
        pointer = self._library.vibra_get_fingerprint_from_wav_data(
            data,
            len(data),
        )
        return self._take_signature(pointer)

    def get_signature_from_signed_pcm(
        self,
        raw_pcm: BytesLike,
        *,
        sample_rate: int,
        sample_width: int,
        channel_count: int,
    ) -> "Signature":
        data = _as_bytes(raw_pcm)
        pointer = self._library.vibra_get_fingerprint_from_signed_pcm(
            data,
            len(data),
            sample_rate,
            sample_width,
            channel_count,
        )
        return self._take_signature(pointer)

    def get_signature_from_float_pcm(
        self,
        raw_pcm: BytesLike,
        *,
        sample_rate: int,
        sample_width: int,
        channel_count: int,
    ) -> "Signature":
        data = _as_bytes(raw_pcm)
        pointer = self._library.vibra_get_fingerprint_from_float_pcm(
            data,
            len(data),
            sample_rate,
            sample_width,
            channel_count,
        )
        return self._take_signature(pointer)

    def _take_signature(self, pointer: int | None) -> "Signature":
        from ._api import Signature

        if not pointer:
            raise RuntimeError("libvibra failed to generate a fingerprint")

        try:
            uri = self._library.vibra_get_uri_from_fingerprint(pointer)
            sample_ms = self._library.vibra_get_sample_ms_from_fingerprint(pointer)
            if uri is None:
                raise RuntimeError("libvibra returned a fingerprint without a URI")
            return Signature(uri=uri.decode("utf-8"), sample_ms=int(sample_ms))
        finally:
            self._library.vibra_free_fingerprint(pointer)

    def _configure_functions(self) -> None:
        self._library.vibra_get_fingerprint_from_music_file.argtypes = [
            ctypes.c_char_p,
        ]
        self._library.vibra_get_fingerprint_from_music_file.restype = ctypes.c_void_p

        self._library.vibra_get_fingerprint_from_wav_data.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
        ]
        self._library.vibra_get_fingerprint_from_wav_data.restype = ctypes.c_void_p

        self._library.vibra_get_fingerprint_from_signed_pcm.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.c_int,
            ctypes.c_int,
            ctypes.c_int,
        ]
        self._library.vibra_get_fingerprint_from_signed_pcm.restype = ctypes.c_void_p

        self._library.vibra_get_fingerprint_from_float_pcm.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
            ctypes.c_int,
            ctypes.c_int,
            ctypes.c_int,
        ]
        self._library.vibra_get_fingerprint_from_float_pcm.restype = ctypes.c_void_p

        self._library.vibra_get_uri_from_fingerprint.argtypes = [
            ctypes.c_void_p,
        ]
        self._library.vibra_get_uri_from_fingerprint.restype = ctypes.c_char_p

        self._library.vibra_get_sample_ms_from_fingerprint.argtypes = [
            ctypes.c_void_p,
        ]
        self._library.vibra_get_sample_ms_from_fingerprint.restype = ctypes.c_uint

        self._library.vibra_free_fingerprint.argtypes = [
            ctypes.c_void_p,
        ]
        self._library.vibra_free_fingerprint.restype = None


def _as_bytes(data: BytesLike) -> bytes:
    if isinstance(data, bytes):
        return data
    return bytes(data)


def _find_library() -> Path:
    override = os.environ.get("VIBRA_LIBRARY_PATH")
    if override:
        return Path(override)

    candidates = _library_candidates()
    native_dir = resources.files(__package__).joinpath("native")
    for candidate in candidates:
        path = native_dir.joinpath(candidate)
        if path.is_file():
            return Path(str(path))

    raise RuntimeError(
        "Could not find libvibra. Install the package, or set VIBRA_LIBRARY_PATH."
    )


def _library_candidates() -> List[str]:
    if sys.platform == "darwin":
        return ["libvibra.dylib"]
    if sys.platform == "win32":
        return ["vibra.dll", "libvibra.dll"]
    return ["libvibra.so"]
