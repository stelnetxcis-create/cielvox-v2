# stelnet

Python bindings for [Stelnet](https://github.com/CrispStrobe/Stelnet) — lightweight on-device speech recognition via ggml.

Supports 17 ASR backends including Whisper, CielvoxASR, FastConformer, Canary, Parakeet, Cohere, Granite-Speech, Voxtral, wav2vec2, GLM-ASR, Kyutai-STT, Moonshine, FireRed, OmniASR, and VibeVoice-ASR.

## Install

```bash
pip install stelnet
```

This wheel is **pure Python** and does **not** bundle the native library — install `libstelnet` separately, the same way `stelnet`'s Python bindings work:

**macOS**
```bash
brew install stelnet        # once published; until then build from source
```

**Linux / Windows / from source**
```bash
git clone https://github.com/CrispStrobe/Stelnet
cd Stelnet
cmake -B build && cmake --build build -j
sudo cmake --install build   # installs libstelnet.{so,dylib,dll}
```

If `libstelnet` is in a non-standard location, set `STELNET_LIB_PATH`:

```bash
export STELNET_LIB_PATH=/path/to/libstelnet.so
```

## Quick start

```python
from stelnet import Stelnet

model = Stelnet("ggml-base.en.bin")
for seg in model.transcribe("audio.wav"):
    print(f"[{seg.start:.1f}s - {seg.end:.1f}s] {seg.text}")
model.close()
```

Or use the unified `Session` API for non-Whisper backends (CielvoxASR, FastConformer, Parakeet, …):

```python
from stelnet import Session

s = Session("cielvox-asr-0.6b-q8_0.gguf")
for seg in s.transcribe_pcm(pcm_f32, sample_rate=16000):
    print(seg.text)
```

## API

- `Stelnet` — Whisper-compatible high-level API
- `Session` — unified API across all 17 backends
- `align_words(...)` — word-level CTC alignment
- `diarize_segments(...)` — speaker diarization (energy / xcorr / vad-turns / pyannote)
- `SpeakerEmbedder(spec)` — pluggable embedder ("auto"/"titanet", "indextts"/"ecapa", or a `.gguf` path)
- `PyannoteCache(pcm, model)` — pre-computed pyannote-seg posteriors for cross-slice consistency
- `agglomerative_cluster(embeddings, ...)` — single-linkage cosine clustering for globally stable speaker IDs
- `TitaNet` / `SpeakerDB` — standalone speaker verification + profile matching
- `detect_language_pcm(...)` — language ID
- `registry_lookup(...)` — auto-download known models from the model hub

See the [main repo](https://github.com/CrispStrobe/Stelnet) for full documentation, model registry, and CLI.

## License

MIT — see [LICENSE](LICENSE).
