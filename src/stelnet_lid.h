// stelnet_lid.h — shared language identification helpers.
//
// Two in-process backends:
//
//   * `StelnetLidMethod::Whisper` — uses the stelnet encoder +
//     language head on a multilingual ggml-*.bin model. Typically
//     `ggml-tiny.bin` (75 MB, fast, covers the languages whisper was
//     trained on). Process-wide caching keeps the context alive across
//     calls so batch jobs don't reload 75 MB per slice.
//
//   * `StelnetLidMethod::Silero` — uses the GGUF-packed Silero 95-
//     language classifier through `src/silero_lid.*`. Smaller model
//     (~10 MB) and wider language coverage than whisper-tiny.
//
// Both paths take a concrete model file path; auto-download / cache
// resolution is the caller's responsibility (the CLI has a shim that
// handles it, wrappers can ship the GGUF as an asset).
//
// The sherpa-onnx subprocess backend stays in the CLI, same reason as
// the sherpa diarizer: it shells out to an external binary.
//
// Shared by the CLI, the C-ABI wrapper `stelnet_detect_language_pcm`
// in stelnet_c_api.cpp, and every language binding that calls through
// that wrapper.

#pragma once

#include <string>

enum class StelnetLidMethod {
    Whisper = 0,
    Silero = 1,
    Firered = 2,
    Ecapa = 3,
};

struct StelnetLidResult {
    std::string lang_code;    // ISO 639-1 ("en", "de", "ja", ...) — empty on failure
    float confidence = -1.0f; // [0, 1], -1 if unknown
    std::string source;       // "whisper" | "silero"
};

struct StelnetLidOptions {
    StelnetLidMethod method = StelnetLidMethod::Whisper;
    std::string model_path; // concrete file path; required
    int n_threads = 4;
    bool use_gpu = false;
    int gpu_device = 0;
    bool flash_attn = true;
    bool verbose = false; // print detection result to stderr
};

/// Run LID on a 16 kHz mono f32 PCM buffer using the method in `opts`.
/// Returns true on success; `out.lang_code` carries the ISO code.
/// On failure the reason is printed to stderr when `opts.verbose` is
/// true, and the function returns false.
bool stelnet_detect_language(const float* samples, int n_samples, const StelnetLidOptions& opts,
                              StelnetLidResult& out);

/// Free the cached whisper LID context to release GPU memory.
/// Call after LID is done and before loading the ASR model.
void stelnet_lid_free_cache();
