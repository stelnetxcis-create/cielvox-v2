// stelnet_enhance.h — audio-enhancement helpers.
//
// Single backend so far:
//
//   * `StelnetEnhanceMethod::Rnnoise` — RNNoise (xiph/rnnoise v0.1,
//     classic GRU model, ~425 KB weights embedded in libstelnet).
//     16 kHz mono float32 input/output in [-1, 1]. Internally
//     upsamples to 48 kHz, runs RNNoise's 480-sample / 10 ms frames,
//     and downsamples back. State is allocated and freed per call —
//     callers can use this freely from worker isolates.
//
// Shared by the CLI shim and by the C-ABI
// `stelnet_enhance_audio_rnnoise` in stelnet_c_api.cpp. Same
// "consume PCM → produce PCM" layering as stelnet_lid.{h,cpp}.

#pragma once

#include <stdint.h>

enum class StelnetEnhanceMethod {
    Rnnoise = 0,
};

struct StelnetEnhanceOptions {
    StelnetEnhanceMethod method = StelnetEnhanceMethod::Rnnoise;
    bool verbose = false;
};

/// Apply enhancement to a 16 kHz mono f32 buffer. Writes exactly
/// `n_samples` floats into `out` (same length, [-1, 1]). Returns
/// true on success; on failure the reason is printed to stderr
/// when `opts.verbose` is true, and `out` is left untouched.
bool stelnet_enhance_audio(const float* in_samples, int n_samples, float* out_samples,
                            const StelnetEnhanceOptions& opts);
