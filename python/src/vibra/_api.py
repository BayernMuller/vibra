from __future__ import annotations

from dataclasses import dataclass
from os import PathLike
from typing import Optional, Union

from ._native import NativeVibra

BytesLike = Union[bytes, bytearray, memoryview]
PathValue = Union[str, PathLike]


@dataclass(frozen=True)
class Signature:
    uri: str
    sample_ms: int


class Vibra:
    def __init__(self) -> None:
        self._native = NativeVibra()

    def get_signature(self, path: PathValue) -> Signature:
        return get_signature(path, self._native)

    def get_signature_from_wav_data(self, raw_wav: BytesLike) -> Signature:
        return get_signature_from_wav_data(raw_wav, self._native)

    def get_signature_from_signed_pcm(
        self,
        raw_pcm: BytesLike,
        *,
        sample_rate: int,
        sample_width: int,
        channel_count: int,
    ) -> Signature:
        return get_signature_from_signed_pcm(
            raw_pcm,
            sample_rate=sample_rate,
            sample_width=sample_width,
            channel_count=channel_count,
            native=self._native,
        )

    def get_signature_from_float_pcm(
        self,
        raw_pcm: BytesLike,
        *,
        sample_rate: int,
        sample_width: int,
        channel_count: int,
    ) -> Signature:
        return get_signature_from_float_pcm(
            raw_pcm,
            sample_rate=sample_rate,
            sample_width=sample_width,
            channel_count=channel_count,
            native=self._native,
        )


def get_signature(path: PathValue, native: Optional[NativeVibra] = None) -> Signature:
    return (native or NativeVibra()).get_signature(path)


def get_signature_from_wav_data(
    raw_wav: BytesLike,
    native: Optional[NativeVibra] = None,
) -> Signature:
    return (native or NativeVibra()).get_signature_from_wav_data(raw_wav)


def get_signature_from_signed_pcm(
    raw_pcm: BytesLike,
    *,
    sample_rate: int,
    sample_width: int,
    channel_count: int,
    native: Optional[NativeVibra] = None,
) -> Signature:
    return (native or NativeVibra()).get_signature_from_signed_pcm(
        raw_pcm,
        sample_rate=sample_rate,
        sample_width=sample_width,
        channel_count=channel_count,
    )


def get_signature_from_float_pcm(
    raw_pcm: BytesLike,
    *,
    sample_rate: int,
    sample_width: int,
    channel_count: int,
    native: Optional[NativeVibra] = None,
) -> Signature:
    return (native or NativeVibra()).get_signature_from_float_pcm(
        raw_pcm,
        sample_rate=sample_rate,
        sample_width=sample_width,
        channel_count=channel_count,
    )
