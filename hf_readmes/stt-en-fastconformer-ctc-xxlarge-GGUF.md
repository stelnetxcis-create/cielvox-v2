---
license: cc-by-4.0
base_model: nvidia/stt_en_fastconformer_ctc_xxlarge
language:
  - en
tags:
  - automatic-speech-recognition
  - gguf
  - stelnet
pipeline_tag: automatic-speech-recognition
---

# stt-en-fastconformer-ctc-xxlarge-GGUF

GGUF quantisations of [nvidia/stt_en_fastconformer_ctc_xxlarge](https://huggingface.co/nvidia/stt_en_fastconformer_ctc_xxlarge) for [Stelnet](https://github.com/CrispStrobe/Stelnet).

| Quant | Description |
|---|---|
| F16 | Full precision |
| Q8_0 | 8-bit |
| Q5_0 | 5-bit |
| Q4_K | 4-bit K-quant (recommended) |

## Usage

```bash
stelnet -m stt-en-fastconformer-ctc-xxlarge-q4_k.gguf -f audio.wav
```
