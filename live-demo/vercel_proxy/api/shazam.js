import { v4 as uuidv4 } from "uuid";
import fetch from "node-fetch";

const USER_AGENTS = [
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0 Safari/605.1.15",
    "Mozilla/5.0 (Linux; Android 10; SM-G973F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.120 Mobile Safari/537.36"
];

const TIMEZONES = [
    "America/New_York",
    "Europe/Paris",
    "Asia/Tokyo"
];

export default async function handler(req, res) {
    const { uri, samplems } = req.query;

    if (!uri || !samplems) {
        res.status(400).json({ error: "Missing required query parameters: url and samplems" });
        return;
    }

    const uuid1 = uuidv4();
    const uuid2 = uuidv4();
    const nowMs = Date.now();

    let apiUrl = `https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/${uuid1}/${uuid2}`;
    apiUrl += "?sync=true&webv3=true&sampling=true&connected=&shazamapiversion=v3&sharehub=true&video=v3";

    const body = {
        geolocation: {
            altitude: 300,
            latitude: 45,
            longitude: 2
        },
        signature: {
            uri: uri,
            samplems: parseInt(samplems),
            timestamp: nowMs
        },
        timestamp: nowMs,
        timezone: TIMEZONES[Math.floor(Math.random() * TIMEZONES.length)]
    };

    const headers = {
        "Content-Type": "application/json",
        "User-Agent": USER_AGENTS[Math.floor(Math.random() * USER_AGENTS.length)],
        "Content-Language": "en_US"
    };

    try {
        const response = await fetch(apiUrl, {
            method: "POST",
            headers: headers,
            body: JSON.stringify(body)
        });

        const data = await response.json();
        res.status(200).json(data);
    } catch (error) {
        res.status(500).json({ error: "Failed to make request to Shazam API", details: error.message });
    }
}