import { Singature } from './index';
import { USER_AGENTS } from './uger_agent';
import { v4 as UUID } from 'uuid';
import axios from 'axios';

export class Shazam {
    static endpoint: string = "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/";

    static async recognize(signature: Singature) {
        let uuid1: string = UUID().toUpperCase();
        let uuid2: string = UUID().toUpperCase();
        
        let url: string = `${Shazam.endpoint}${uuid1}/${uuid2}`;
        url += "?sync=true&webv3=true&sampling=true&connected=&shazamapiversion=v3&sharehub=true&video=v3";
        
        let body = {
            geolocation: {
                altitude: 300,
                latitude: 45,
                longitude: 2
            },
            signature: {
                uri: signature.uri,
                samplems: signature.samplems,
                timestamp: Date.now()
            },
            timestamp: Date.now(),
            timezone: "Europe/Paris"
        };

        let headers = {
            "Content-Type": "application/json",
            "User-Agent": USER_AGENTS[Math.floor(Math.random() * USER_AGENTS.length)],
            "Content-Language": "en_US"
        };

        let response = await axios.post(url, body, { headers: headers });
        return response.data;
    }
}