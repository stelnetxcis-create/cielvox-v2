// stelnet_aligner_cli.h — CLI-side aligner shim.
//
// The dispatch + inference for both canary-ctc and qwen3-forced-aligner
// now lives in `src/stelnet_aligner.{h,cpp}`. This header keeps the
// thin CLI adapter that returns results as `stelnet_word` (the CLI
// type) instead of the library's `StelnetAlignedWord`.

#pragma once

#include "stelnet_backend.h"

#include <string>
#include <vector>

std::vector<stelnet_word> stelnet_ctc_align(const std::string& aligner_model, const std::string& transcript,
                                              const float* samples, int n_samples, int64_t t_offset_cs, int n_threads);
