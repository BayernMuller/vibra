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
  // older wasm builds don't export FreeFingerprint
  if (typeof Module._FreeFingerprint === 'function') {
    Module.ccall('FreeFingerprint', null, ['number'], [signaturePtr]);
  }

  return { uri, samplems };
}

(async () => {
  const PROXY_URL = 'https://vercel-proxy-rust-three.vercel.app/api/shazam';
  // Recognition is attempted on the growing recording at these offsets,
  // so a later attempt reuses everything captured so far.
  const ATTEMPT_TIMES = [5000, 7000, 9000, 12000];

  const listenBtn = document.getElementById('listenBtn');
  const listenLabel = document.getElementById('listen-label');
  const statusEl = document.getElementById('status');
  const listenView = document.getElementById('listen-view');
  const resultView = document.getElementById('result-view');
  const againBtn = document.getElementById('againBtn');

  let audioContext = null;
  let recorderNode = null;
  let stream = null;
  let recordedChunks = [];
  let attemptIndex = 0;
  let attemptTimer = null;
  let listening = false;
  let recordingStartedAt = 0;

  function setStatus(message, isError = false) {
    statusEl.textContent = message || '';
    statusEl.classList.toggle('error', isError);
  }

  function showResult(track) {
    const sections = track.sections || [];
    const metadata = sections[0]?.metadata || [];

    const cover = document.getElementById('cover');
    const coverUrl = track.images?.coverart || track.images?.coverarthq;
    cover.hidden = !coverUrl;
    if (coverUrl) {
      cover.onerror = () => {
        cover.hidden = true;
      };
      cover.src = coverUrl;
    } else {
      cover.removeAttribute('src');
    }
    document.getElementById('title').textContent = track.title;
    document.getElementById('artist').textContent = track.subtitle || '';
    document.getElementById('album').textContent = metadata[0]?.text || '';

    listenView.hidden = true;
    resultView.hidden = false;
  }

  function showListenView() {
    resultView.hidden = true;
    listenView.hidden = false;
    setStatus('');
  }

  function setListeningUi(active) {
    listenBtn.classList.toggle('listening', active);
    listenLabel.textContent = active ? 'Listening…' : 'Tap to identify';
  }

  async function startListening() {
    recordedChunks = [];
    attemptIndex = 0;
    listening = true;
    setListeningUi(true);
    setStatus('');

    try {
      // The browser's default voice processing (echo cancellation, noise
      // suppression, auto gain) mangles music, so turn it all off.
      stream = await navigator.mediaDevices.getUserMedia({
        audio: {
          echoCancellation: false,
          noiseSuppression: false,
          autoGainControl: false,
        },
      });

      audioContext = new AudioContext();
      // Chrome can start the context suspended even inside a user gesture
      if (audioContext.state === 'suspended') {
        await audioContext.resume();
      }
      const sourceNode = audioContext.createMediaStreamSource(stream);
      await audioContext.audioWorklet.addModule('public/recorderProcessor.js');
      recorderNode = new AudioWorkletNode(audioContext, 'recorder-processor');
      recorderNode.port.onmessage = event => {
        recordedChunks.push(event.data);
      };
      sourceNode.connect(recorderNode);

      recordingStartedAt = Date.now();
      attemptTimer = setTimeout(runAttempt, ATTEMPT_TIMES[0]);
    } catch (error) {
      console.error(error);
      stopListening();
      setStatus(
          error.name === 'NotAllowedError'
              ? 'Microphone access was denied. Please allow it and try again.'
              : error.message,
          true);
    }
  }

  function stopListening() {
    listening = false;
    clearTimeout(attemptTimer);
    attemptTimer = null;
    if (recorderNode) {
      recorderNode.disconnect();
      recorderNode = null;
    }
    if (audioContext) {
      audioContext.close();
    }
    if (stream) {
      stream.getTracks().forEach(track => track.stop());
      stream = null;
    }
    setListeningUi(false);
  }

  async function runAttempt() {
    if (!listening) {
      return;
    }
    setStatus(`Identifying… (attempt ${attemptIndex + 1} of ${ATTEMPT_TIMES.length})`);

    // AudioContext keeps its own device-native rate (44.1k, 48k, ...);
    // the fingerprinter must be told the real one.
    const sampleRate = audioContext.sampleRate;
    const pcm = mergeBuffers(recordedChunks);

    let track = null;
    try {
      const signature = await getPcmSignature(pcm, pcm.byteLength, sampleRate, 32, 1);
      const response = await fetch(
          `${PROXY_URL}?uri=${signature.uri}&samplems=${signature.samplems}`);
      const data = await response.json();
      if (!data.retryms && data.track?.title) {
        track = data.track;
      }
    } catch (error) {
      console.error(error);
      stopListening();
      setStatus(error.message, true);
      return;
    }

    if (!listening) {
      return;  // cancelled while the request was in flight
    }

    if (track) {
      stopListening();
      showResult(track);
      return;
    }

    attemptIndex++;
    if (attemptIndex >= ATTEMPT_TIMES.length) {
      stopListening();
      setStatus('Couldn’t identify the song. Reduce background noise and try again.', true);
      return;
    }

    const elapsed = Date.now() - recordingStartedAt;
    setStatus('Listening for a bit longer…');
    attemptTimer = setTimeout(runAttempt, Math.max(0, ATTEMPT_TIMES[attemptIndex] - elapsed));
  }

  function mergeBuffers(buffers) {
    const length = buffers.reduce((total, buffer) => total + buffer.length, 0);
    const result = new Float32Array(length);
    let offset = 0;
    buffers.forEach(buffer => {
      result.set(buffer, offset);
      offset += buffer.length;
    });
    return new Uint8Array(result.buffer);
  }

  listenBtn.onclick = () => {
    if (listening) {
      stopListening();
      setStatus('');
    } else {
      startListening();
    }
  };

  againBtn.onclick = () => {
    showListenView();
    startListening();
  };

  listenBtn.disabled = true;
  try {
    await waitForVibraRuntime();
    listenBtn.disabled = false;
  } catch (error) {
    console.error(error);
    setStatus(error.message, true);
  }
})();
