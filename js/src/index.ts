import EventEmitter from 'events';
import { readFileSync } from 'fs';
import { fileURLToPath } from 'url';
import createModule, { VibraModule, VibraModuleOptions } from './vibra.js';
import { Signature } from './signature.js';

export { Signature, Singature } from './signature.js';

function isNode(): boolean {
  return typeof process !== 'undefined' && process.versions?.node !== undefined;
}

function wasmUrl(): URL {
  return new URL('vibra.wasm', import.meta.url);
}

function defaultLocateFile(path: string): string {
  const url = new URL(path, import.meta.url);
  return isNode() ? fileURLToPath(url) : url.toString();
}

function defaultModuleOptions(): VibraModuleOptions {
  const options: VibraModuleOptions = {
    locateFile: defaultLocateFile,
  };

  if (isNode()) {
    options.wasmBinary = readFileSync(fileURLToPath(wasmUrl()));
  }

  return options;
}

export class Vibra extends EventEmitter {
  public initialized = false;
  private module: VibraModule | null = null;
  private readyPromise: Promise<VibraModule>;

  constructor(options: VibraModuleOptions = {}) {
    super();
    this.readyPromise = createModule({
      ...defaultModuleOptions(),
      ...options,
    }).then((module) => {
      this.module = module;
      this.initialized = true;
      this.emit('initialized');
      return module;
    });
  }

  static async create(options: VibraModuleOptions = {}): Promise<Vibra> {
    const vibra = new Vibra(options);
    await vibra.ready();
    return vibra;
  }

  async ready(): Promise<void> {
    await this.readyPromise;
  }

  getSignature(rawwav: Uint8Array): Signature {
    const module = this.module;
    if (module === null) {
      throw new Error('Vibra not initialized');
    }

    const dataPtr = module._malloc(rawwav.length);
    module.HEAPU8.set(rawwav, dataPtr);
    const signaturePtr = module.ccall(
      'GetWavSignature',
      'number',
      ['number', 'number'],
      [dataPtr, rawwav.length]
    ) as number;
    module._free(dataPtr);

    const uri = module.ccall(
      'GetFingerprint',
      'string',
      ['number'],
      [signaturePtr]
    ) as string;
    const samplems = module.ccall(
      'GetSampleMs',
      'number',
      ['number'],
      [signaturePtr]
    ) as number;

    module.ccall('FreeFingerprint', 'void', ['number'], [signaturePtr]);

    return new Signature(uri, samplems);
  }
}
