from __future__ import annotations

import random
import time
from typing import Any, Dict, Optional
from uuid import uuid4

import requests

from ._api import Signature


USER_AGENTS = [
    "Dalvik/2.1.0 (Linux; U; Android 5.0.2; VS980 4G Build/LRX22G)",
    "Dalvik/1.6.0 (Linux; U; Android 4.4.2; SM-T210 Build/KOT49H)",
    "Dalvik/2.1.0 (Linux; U; Android 5.1.1; SM-P905V Build/LMY47X)",
    "Dalvik/2.1.0 (Linux; U; Android 6.0.1; SM-G920F Build/MMB29K)",
    "Dalvik/2.1.0 (Linux; U; Android 5.0; SM-G900F Build/LRX21T)",
    "Dalvik/2.1.0 (Linux; U; Android 6.0.1; SM-G928F Build/MMB29K)",
    "Dalvik/2.1.0 (Linux; U; Android 5.1.1; SM-J500FN Build/LMY48B)",
    "Dalvik/1.6.0 (Linux; U; Android 4.4.2; SAMSUNG-SGH-I747 Build/KOT49H)",
    "Dalvik/2.1.0 (Linux; U; Android 5.1.1; SM-N910T Build/LMY47X)",
    "Dalvik/2.1.0 (Linux; U; Android 6.0.1; SM-G930F Build/MMB29K)",
]

EUROPE_TIMEZONES = [
    "Europe/Amsterdam",
    "Europe/Athens",
    "Europe/Belgrade",
    "Europe/Berlin",
    "Europe/Brussels",
    "Europe/Bucharest",
    "Europe/Budapest",
    "Europe/Copenhagen",
    "Europe/Dublin",
    "Europe/Helsinki",
    "Europe/Istanbul",
    "Europe/Lisbon",
    "Europe/London",
    "Europe/Madrid",
    "Europe/Paris",
    "Europe/Prague",
    "Europe/Rome",
    "Europe/Stockholm",
    "Europe/Vienna",
    "Europe/Warsaw",
    "Europe/Zurich",
]


class Shazam:
    endpoint = "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/"
    query = (
        "sync=true&"
        "webv3=true&"
        "sampling=true&"
        "connected=&"
        "shazamapiversion=v3&"
        "sharehub=true&"
        "video=v3"
    )

    @classmethod
    def recognize(
        cls,
        signature: Signature,
        *,
        session: Optional[requests.Session] = None,
        timeout: float = 10.0,
    ) -> Any:
        client = session or requests.Session()
        response = client.post(
            cls._url(),
            json=cls._body(signature),
            headers=cls._headers(),
            timeout=timeout,
        )
        response.raise_for_status()
        return response.json()

    @classmethod
    def _url(cls) -> str:
        return f"{cls.endpoint}{str(uuid4()).upper()}/{str(uuid4()).upper()}?{cls.query}"

    @staticmethod
    def _headers() -> Dict[str, str]:
        return {
            "Accept": "*/*",
            "Accept-Encoding": "gzip, deflate, br",
            "Connection": "keep-alive",
            "Content-Type": "application/json",
            "Content-Language": "en_US",
            "User-Agent": random.choice(USER_AGENTS),
        }

    @staticmethod
    def _body(signature: Signature) -> Dict[str, Any]:
        timestamp = int(time.time() * 1000)
        fuzz = random.random() * 15.3 - 7.65
        return {
            "geolocation": {
                "altitude": random.random() * 400 + 100 + fuzz,
                "latitude": random.random() * 180 - 90 + fuzz,
                "longitude": random.random() * 360 - 180 + fuzz,
            },
            "signature": {
                "uri": signature.uri,
                "samplems": signature.sample_ms,
                "timestamp": timestamp,
            },
            "timestamp": timestamp,
            "timezone": random.choice(EUROPE_TIMEZONES),
        }


def recognize(
    signature: Signature,
    *,
    session: Optional[requests.Session] = None,
    timeout: float = 10.0,
) -> Any:
    return Shazam.recognize(signature, session=session, timeout=timeout)
