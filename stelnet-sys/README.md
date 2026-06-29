# stelnet-sys

Raw FFI bindings for [Stelnet](https://github.com/CrispStrobe/Stelnet) — lightweight on-device speech recognition via ggml.

This crate is a thin `extern "C"` shim and **does not** build the native library. Users install `libstelnet.{so,dylib,dll}` separately — same pattern as Stelnet's other language bindings.

## Install

```toml
[dependencies]
stelnet-sys = "0.1"
```

You also need the native library:

```bash
git clone https://github.com/CrispStrobe/Stelnet
cd Stelnet
cmake -B build && cmake --build build -j
sudo cmake --install build
```

If `libstelnet` is in a non-standard location, point the linker at it:

```bash
export STELNET_LIB_DIR=/path/to/lib
```

The legacy `libwhisper` alias also works:

```bash
export STELNET_LIB_NAME=whisper
```

For the safe high-level wrapper see the [`stelnet`](https://crates.io/crates/stelnet) crate.

## License

MIT — see [LICENSE](LICENSE).
