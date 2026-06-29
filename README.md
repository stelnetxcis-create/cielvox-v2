# cielvox-v2

Local speech recognition and text-to-speech — no internet required at runtime. GPU accelerated via Vulkan, CUDA, or Metal.

---

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DGGML_VULKAN=ON
cmake --build build --parallel
```

---

## ASR — Speech Recognition

```bash
./build/bin/stelnet --backend cielvox-asr-1.7b -m auto -l en audio.wav
```

First run downloads the model to `~/.cache/stelnet/` automatically (~2.5 GB).

### Useful flags

```bash
# Word-level timestamps
./build/bin/stelnet --backend cielvox-asr-1.7b -m auto -l en --aligner-model auto audio.wav

# More accurate (slower)
./build/bin/stelnet --backend cielvox-asr-1.7b -m auto -l en --beam-size 5 --best-of 5 audio.wav

# Live mic transcription
./build/bin/stelnet --backend cielvox-asr-1.7b -m auto -l en --mic --live

# Output to SRT
./build/bin/stelnet --backend cielvox-asr-1.7b -m auto -l en --output-srt --output-file out audio.wav
```

> 0.6b model is in progress.

---

## TTS — Text to Speech

Two variants for the 1.7b: voice cloning from a reference clip, or voice creation via text description. Both require the shared tokenizer (~291 MB).

| Backend | Size |
|---|---|
| `cielvox2-1.7b-stelnetvoice` | ~2.1 GB |
| `cielvox2-1.7b-stelnetvoicecreation` | ~2.0 GB |
| tokenizer (shared) | ~291 MB |

> 0.6b models are in progress.

### Voice cloning — clone from a reference WAV

Provide a 16–30 second reference clip and its transcription.

```bash
./build/bin/stelnet \
  --backend cielvox2-1.7b-stelnetvoice \
  -m stelnet/cielvox-12hz-1.7b-base-q8_0.gguf \
  --codec-model stelnet/cielvox-tokenizer-12hz.gguf \
  --gpu-backend vulkan \
  --no-spoken-disclaimer \
  --voice /path/to/reference.wav \
  --ref-text "Exact words spoken in the reference audio." \
  --i-have-rights \
  --tts "Text you want spoken." \
  --tts-output output/result.wav \
  --tts-play
```

Skip `--ref-text` to auto-transcribe the reference using ASR:

```bash
  --ref-asr cielvox-asr-1.7b
```

### Voice creation — describe the voice in plain text

No reference clip needed. Use `--instruct` to describe the voice you want.

```bash
./build/bin/stelnet \
  --backend cielvox2-1.7b-stelnetvoicecreation \
  -m stelnet/cielvox-12hz-1.7b-stelnetvoicecreation-q8_0.gguf \
  --codec-model stelnet/cielvox-tokenizer-12hz.gguf \
  --gpu-backend vulkan \
  --no-spoken-disclaimer \
  --seed 42 \
  --instruct "A calm, deep male narrator with slight British accent." \
  --tts "Text to synthesise." \
  --tts-output output/result.wav \
  --tts-play
```

### Server mode

Load the model once, serve multiple requests without reloading.

```bash
# Start server
./build/bin/stelnet \
  --backend cielvox2-1.7b-stelnetvoice \
  -m stelnet/cielvox-12hz-1.7b-base-q8_0.gguf \
  --codec-model stelnet/cielvox-tokenizer-12hz.gguf \
  --gpu-backend vulkan \
  --no-spoken-disclaimer \
  --voice /path/to/reference.wav \
  --ref-text "Reference transcription." \
  --i-have-rights \
  --server --port 8080

# Query it
curl -X POST http://localhost:8080/v1/audio/speech \
  -H "Content-Type: application/json" \
  -d '{"input":"Text to speak."}' \
  --output speech.wav
```

---

## Model cache

All auto-downloaded models are saved to `~/.cache/stelnet/`. Override with `--cache-dir /your/path`.

Check what would download before committing:

```bash
./build/bin/stelnet --backend cielvox-asr-1.7b -m auto --dry-run-resolve
```

---

## GPU

```bash
--gpu-backend vulkan   # AMD / Intel
--gpu-backend cuda     # NVIDIA
--gpu-backend metal    # Apple
--gpu-backend cpu      # force CPU
```

---

## License

MIT
