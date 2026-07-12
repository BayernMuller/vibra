#!/usr/bin/env node

import { Vibra } from './index.js';
import { Shazam } from './shazam.js';
import fs from 'fs';

async function main(): Promise<void> {
  if (process.argv.length < 3) {
    console.log('Usage: node dist/cli.js <wav file>');
    process.exit(1);
  }

  const filePath = process.argv[2];
  const buffer = fs.readFileSync(filePath);

  const vibra = await Vibra.create();
  const signature = vibra.getSignature(buffer);
  const result = await Shazam.recognize(signature);
  console.log(JSON.stringify(result, null, 4));
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
