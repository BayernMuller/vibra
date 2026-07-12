class RecorderProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
  }

  process(inputs) {
    const input = inputs[0];
    if (input.length > 0) {
      const channelData = input[0];
      this.port.postMessage(channelData.slice());
    }
    return true;
  }
}

registerProcessor('recorder-processor', RecorderProcessor);