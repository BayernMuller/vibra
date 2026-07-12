let vibraRuntimeReadyPromise;

function isVibraRuntimeReady() {
  return typeof Module !== 'undefined' && Module.HEAPU8 && Module._malloc && Module.ccall;
}

function waitForVibraRuntime() {
  if (isVibraRuntimeReady()) {
    return Promise.resolve();
  }

  if (vibraRuntimeReadyPromise) {
    return vibraRuntimeReadyPromise;
  }

  vibraRuntimeReadyPromise = new Promise((resolve, reject) => {
    if (typeof Module === 'undefined') {
      reject(new Error('vibra.js did not load'));
      return;
    }

    const previousOnRuntimeInitialized = Module.onRuntimeInitialized;
    const timeout = setTimeout(() => {
      reject(new Error('vibra wasm did not initialize'));
    }, 10000);

    Module.onRuntimeInitialized = () => {
      clearTimeout(timeout);
      if (typeof previousOnRuntimeInitialized === 'function') {
        previousOnRuntimeInitialized();
      }
      resolve();
    };
  });

  return vibraRuntimeReadyPromise;
}

async function getPcmSignature(rawpcm, pcm_size, sampleRate, sampleWidth, channelCount) {
  await waitForVibraRuntime();
  const dataPtr = Module._malloc(pcm_size);
  Module.HEAPU8.set(rawpcm, dataPtr);
  const signaturePtr = Module.ccall(
      'GetFloatPcmSignature',
      'number',
      ['number', 'number', 'number', 'number', 'number'],
      [dataPtr, pcm_size, sampleRate, sampleWidth, channelCount]
  );
  Module._free(dataPtr);

  const uri = Module.ccall('GetFingerprint', 'string', ['number'], [signaturePtr]);
  const samplems = Module.ccall('GetSampleMs', 'number', ['number'], [signaturePtr]);

  return {
    uri: uri,
    samplems: samplems
  }
}

function recognizeSuccess(album, title, artist, cover) {
  document.getElementById('album').textContent = album;
  document.getElementById('title').textContent = title;
  document.getElementById('artist').textContent = artist;
  document.getElementById('cover').src = cover;

  const trackContainer = document.getElementById('track-container');
  trackContainer.style.display = 'flex';

  const titleContent = document.getElementById('title-content');
  titleContent.style.display = 'none';

  const centerContent = document.getElementById('center-content');
  centerContent.style.display = 'none';

  const errorMessage = document.getElementById('error-message');
  errorMessage.style.display = 'none';
}

function recognizeFailed(error) {
  const errorMessage = document.getElementById('error-message');
  errorMessage.textContent = error;
  errorMessage.style.display = 'block';
}

(async () => {
  const startBtn = document.getElementById('startBtn');
  const stopBtn = document.getElementById('stopBtn');
  let audioContext;
  let recorderNode;
  let recordedChunks = [];
  const durations = [5000, 7000, 9000, 12000];
  let currentDurationIndex = 0;
  let recognitionTimeout;
  let isRecording = false;
  let stream;

  stopBtn.disabled = true;
  try {
    await waitForVibraRuntime();
    startBtn.disabled = false;
  } catch (error) {
    console.log(error);
    recognizeFailed(error.message);
  }

  startBtn.onclick = async () => {
    // Reset variables before starting a new recording session
    currentDurationIndex = 0;
    recordedChunks = [];
    isRecording = false;

    startBtn.disabled = true;
    stopBtn.disabled = false;
    console.log('Recording started...');

    const errorMessage = document.getElementById('error-message');
    errorMessage.style.display = 'none';

    // Start the first recording with the initial duration
    await startRecording();
  };

  stopBtn.onclick = async () => {
    console.log('Recording stopped by user.');
    await stopRecording();

    // Reset the current duration index to stop any further recording attempts
    currentDurationIndex = durations.length;

    // Re-enable the start button and disable the stop button
    startBtn.disabled = false;
    stopBtn.disabled = true;

    recognizeFailed("Recording stopped by user.");

    // Clean up recorded chunks
    recordedChunks = [];
  };

  async function startRecording() {
    isRecording = true;
    const duration = durations[currentDurationIndex];
    console.log(`Recording for ${duration / 1000} seconds...`);

    stream = await navigator.mediaDevices.getUserMedia({ audio: true });
    audioContext = new AudioContext();
    const sourceNode = audioContext.createMediaStreamSource(stream);
    await audioContext.audioWorklet.addModule('public/recorderProcessor.js');
    recorderNode = new AudioWorkletNode(audioContext, 'recorder-processor');
    recorderNode.port.onmessage = event => {
      const audioChunk = event.data;
      recordedChunks.push(audioChunk);
    };
    sourceNode.connect(recorderNode);

    // Stop recording after the specified duration
    recognitionTimeout = setTimeout(async () => {
      await stopRecording();
      const success = await recognizeCurrentRecording();
      if (success) {
        // Recognition successful, no need to proceed further
        console.log('Recognition successful.');
        // Disable the stop button
        stopBtn.disabled = true;
        startBtn.disabled = false;

        // Reset variables after success
        resetVariables();
      } else {
        // Recognition failed, proceed to the next duration
        currentDurationIndex++;
        if (currentDurationIndex < durations.length) {
          // Reset recorded data and start recording for the next duration
          recordedChunks = [];
          await startRecording();
        } else {
          // All durations attempted, show error message
          recognizeFailed("Please reduce the surrounding noise and try again");
          startBtn.disabled = false;
          stopBtn.disabled = true;

          // Reset variables after failure
          resetVariables();
        }
      }
    }, duration);
  }

  async function stopRecording() {
    if (isRecording) {
      console.log('Stopping recording...');
      recorderNode.disconnect();
      audioContext.close();
      stream.getTracks().forEach(track => track.stop());
      clearTimeout(recognitionTimeout);
      isRecording = false;
    }
  }

  async function recognizeCurrentRecording() {
    console.log(`Attempting recognition with ${durations[currentDurationIndex] / 1000} seconds of audio...`);

    let audioBuffer = mergeBuffers(recordedChunks);
    const buffer_byte_length = audioBuffer.length * audioBuffer.BYTES_PER_ELEMENT;

    try {
      let signature = await getPcmSignature(audioBuffer, buffer_byte_length, 44100, 32, 1);
      let response = await fetch(`https://vercel-proxy-rust-three.vercel.app/api/shazam?uri=${signature.uri}&samplems=${signature.samplems}`);
      let data = await response.json();

      if (data.retryms) {
        // Recognition failed
        return false;
      } else {
        // Recognition successful
        const track = data.track || {};
        const sections = track.sections || [];
        const metadata = sections[0]?.metadata || [];
        const album = metadata[0]?.text;
        const title = track.title;
        const artist = track.subtitle;
        const cover = track.images?.coverart;

        if (album && title && artist && cover) {
          recognizeSuccess(album, title, artist, cover);
          return true;
        } else {
          return false;
        }
      }
    } catch (error) {
      console.log(error);
      recognizeFailed(error.message);
      return false;
    }
  }

  function mergeBuffers(buffers){
    let length = buffers.reduce((total, buffer) => total + buffer.length, 0);
    let result = new Float32Array(length);
    let offset = 0;
    buffers.forEach(buffer => {
      result.set(buffer, offset);
      offset += buffer.length;
    });
    return new Uint8Array(result.buffer);
  }

  function resetVariables() {
    // Reset all variables to their initial state
    audioContext = null;
    recorderNode = null;
    recordedChunks = [];
    currentDurationIndex = 0;
    recognitionTimeout = null;
    isRecording = false;
    stream = null;
  }
})();
