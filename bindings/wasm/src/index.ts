import EventEmitter from 'events';
import Module from './vibra'

export class Singature {
  uri: string;
  samplems: number;

  constructor(uri: string, samplems: number) {
    this.uri = uri;
    this.samplems = samplems;
  }
}

export class Vibra extends EventEmitter {
  public initialized = false;

  constructor() {
    super();
    Module.onRuntimeInitialized = () => {
      this.initialized = true;
      this.emit('initialized');
    };
  }

  getSignature(rawwav: Uint8Array): Singature {
    if (!this.initialized) {
      throw new Error('Vibra not initialized');
    }

    const dataPtr = Module._malloc(rawwav.length);
    Module.HEAPU8.set(rawwav, dataPtr);
    const signaturePtr = Module.ccall(
      'GetWavSignature',
      'number',
      ['number', 'number'],
      [dataPtr, rawwav.length]
    );
    Module._free(dataPtr);

    const uri = Module.ccall('GetFingerprint', 'string', ['number'], [signaturePtr]);
    const samplems = Module.ccall('GetSampleMs', 'number', ['number'], [signaturePtr]);

    return new Singature(uri, samplems);
  }
}