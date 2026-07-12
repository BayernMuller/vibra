import struct
import unittest

from vibra import (
    Shazam,
    Signature,
    Vibra,
    get_signature_from_float_pcm,
    get_signature_from_signed_pcm,
    get_signature_from_wav_data,
)


class FakeResponse:
    def __init__(self, data):
        self.data = data
        self.raised = False

    def raise_for_status(self):
        self.raised = True

    def json(self):
        return self.data


class FakeSession:
    def __init__(self):
        self.calls = []
        self.response = FakeResponse({"track": {"title": "Misty"}})

    def post(self, url, **kwargs):
        self.calls.append((url, kwargs))
        return self.response


def make_wav(samples):
    data = b"".join(struct.pack("<h", sample) for sample in samples)
    return b"".join(
        [
            b"RIFF",
            struct.pack("<I", 36 + len(data)),
            b"WAVE",
            b"fmt ",
            struct.pack("<I", 16),
            struct.pack("<H", 1),
            struct.pack("<H", 1),
            struct.pack("<I", 16000),
            struct.pack("<I", 16000 * 2),
            struct.pack("<H", 2),
            struct.pack("<H", 16),
            b"data",
            struct.pack("<I", len(data)),
            data,
        ]
    )


class VibraTest(unittest.TestCase):
    def test_generates_signature_from_signed_pcm(self):
        pcm = struct.pack("<128h", *([0] * 128))

        signature = get_signature_from_signed_pcm(
            pcm,
            sample_rate=16000,
            sample_width=16,
            channel_count=1,
        )

        self.assertEqual(signature.sample_ms, 8)
        self.assertTrue(signature.uri.startswith("data:audio/vnd.shazam.sig;base64,"))

    def test_generates_same_signature_for_equivalent_wav_and_pcm(self):
        samples = [index % 64 for index in range(512)]
        pcm = struct.pack("<512h", *samples)
        wav_signature = get_signature_from_wav_data(make_wav(samples))
        pcm_signature = Vibra().get_signature_from_signed_pcm(
            pcm,
            sample_rate=16000,
            sample_width=16,
            channel_count=1,
        )

        self.assertEqual(wav_signature, pcm_signature)

    def test_generates_signature_from_float_pcm(self):
        pcm = struct.pack("<128f", *([0.0] * 128))

        signature = get_signature_from_float_pcm(
            pcm,
            sample_rate=16000,
            sample_width=32,
            channel_count=1,
        )

        self.assertEqual(signature.sample_ms, 8)

    def test_shazam_adapter_posts_signature_with_requests_session(self):
        session = FakeSession()
        signature = Signature(
            uri="data:audio/vnd.shazam.sig;base64,abc",
            sample_ms=8000,
        )

        result = Shazam.recognize(signature, session=session, timeout=3.0)

        self.assertEqual(result["track"]["title"], "Misty")
        self.assertTrue(session.response.raised)
        self.assertEqual(len(session.calls), 1)

        url, kwargs = session.calls[0]
        self.assertTrue(url.startswith(Shazam.endpoint))
        self.assertIn("shazamapiversion=v3", url)
        self.assertEqual(kwargs["timeout"], 3.0)
        self.assertEqual(kwargs["headers"]["Content-Type"], "application/json")
        self.assertEqual(kwargs["headers"]["Content-Language"], "en_US")
        self.assertIn("User-Agent", kwargs["headers"])
        self.assertEqual(
            kwargs["json"]["signature"]["uri"],
            "data:audio/vnd.shazam.sig;base64,abc",
        )
        self.assertEqual(kwargs["json"]["signature"]["samplems"], 8000)


if __name__ == "__main__":
    unittest.main()
