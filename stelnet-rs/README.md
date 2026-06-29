# stelnet

Safe Rust wrapper for [Stelnet](https://github.com/CrispStrobe/Stelnet) — lightweight on-device speech recognition via ggml.

Supports 17 ASR backends including Whisper, Qwen3-ASR, FastConformer, Canary, Parakeet, Cohere, Granite-Speech, Voxtral, wav2vec2, GLM-ASR, Kyutai-STT, Moonshine, FireRed, OmniASR, and VibeVoice-ASR.

## Install

```toml
[dependencies]
stelnet = "0.1"
```

You also need `libstelnet` installed on the system — this crate is a thin wrapper around `libstelnet` and does **not** build it. Same pattern as Stelnet's other language bindings.

```bash
git clone https://github.com/CrispStrobe/Stelnet
cd Stelnet
cmake -B build && cmake --build build -j
sudo cmake --install build
```

If `libstelnet` is in a non-standard location, set `STELNET_LIB_DIR`.

## Quick start

```rust
use stelnet::Stelnet;

let model = Stelnet::open("ggml-base.en.bin")?;
for seg in model.transcribe_file("audio.wav")? {
    println!("[{:.1}s - {:.1}s] {}", seg.start, seg.end, seg.text);
}
```

See the [main repo](https://github.com/CrispStrobe/Stelnet) for full documentation, the model registry, and the CLI.

## License

MIT — see [LICENSE](LICENSE).
