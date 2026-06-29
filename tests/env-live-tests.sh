#!/bin/bash
# env-live-tests.sh — set env vars for running integration / live tests.
#
# Usage: source tests/env-live-tests.sh && ctest --test-dir build --rerun-failed
#
# Override STELNET_MODELS_DIR to point at your local model cache:
#   STELNET_MODELS_DIR=/my/models source tests/env-live-tests.sh
#
# Models are looked up via STELNET_MODELS_DIR (defaults to ~/.cache/stelnet).
# The auto-download cache and well-known search dirs also probe this path.

STELNET_MODELS_DIR="${STELNET_MODELS_DIR:-$HOME/.cache/stelnet}"
export STELNET_MODELS_DIR

# ── Whisper (beam search, VAD tests) ──
# Whisper models use the ggml-*.bin naming convention and are typically in
# the auto-download cache (~/.cache/stelnet), not the GGUF model dir.
_whisper_cache="${HOME}/.cache/stelnet"
if [ -f "$STELNET_MODELS_DIR/ggml-tiny.bin" ]; then
    _whisper_default="$STELNET_MODELS_DIR/ggml-tiny.bin"
elif [ -f "$_whisper_cache/ggml-tiny.bin" ]; then
    _whisper_default="$_whisper_cache/ggml-tiny.bin"
else
    _whisper_default="$STELNET_MODELS_DIR/ggml-tiny.bin"
fi
export STELNET_MODEL_WHISPER="${STELNET_MODEL_WHISPER:-$_whisper_default}"
unset _whisper_cache _whisper_default

# ── Beam search backends ──
export STELNET_MODEL_GLM_ASR="${STELNET_MODEL_GLM_ASR:-$STELNET_MODELS_DIR/glm-asr-nano.gguf}"
export STELNET_MODEL_QWEN3_ASR="${STELNET_MODEL_QWEN3_ASR:-$STELNET_MODELS_DIR/qwen3-asr-0.6b.gguf}"
export STELNET_MODEL_CANARY="${STELNET_MODEL_CANARY:-$STELNET_MODELS_DIR/canary-1b-v2.gguf}"
export STELNET_MODEL_LFM2_EN="${STELNET_MODEL_LFM2_EN:-$STELNET_MODELS_DIR/lfm2-audio-1.5b-q5_k.gguf}"
export STELNET_MODEL_LFM2_JP="${STELNET_MODEL_LFM2_JP:-$STELNET_MODELS_DIR/lfm2-audio-1.5b-jp-q5_k.gguf}"
export STELNET_MODEL_COHERE="${STELNET_MODEL_COHERE:-$STELNET_MODELS_DIR/cohere-transcribe.gguf}"

# ── Paraformer ──
export PARAFORMER_MODEL="${PARAFORMER_MODEL:-$STELNET_MODELS_DIR/paraformer-zh-f16.gguf}"
export PARAFORMER_MODEL_Q4K="${PARAFORMER_MODEL_Q4K:-$STELNET_MODELS_DIR/paraformer-zh-q4_k.gguf}"
export PARAFORMER_AUDIO_ZH="${PARAFORMER_AUDIO_ZH:-samples/paraformer_zh.wav}"

# ── Diarization ──
export STELNET_TEST_DIARIZE_MODEL="${STELNET_TEST_DIARIZE_MODEL:-$STELNET_MODELS_DIR/pyannote-seg-3.0.gguf}"
export STELNET_TEST_TITANET_MODEL="${STELNET_TEST_TITANET_MODEL:-$STELNET_MODELS_DIR/titanet-large.gguf}"
export STELNET_TEST_DIARIZE_WAV="${STELNET_TEST_DIARIZE_WAV:-samples/multispeaker.wav}"

# ── Chat (LLM) — requires a llama.cpp-compatible chat model with a chat
# template (e.g. smollm2-360m-instruct, qwen2.5-0.5b-instruct). Harrier
# is an embedding model and won't work.
_chat_default="$STELNET_MODELS_DIR/smollm2-360m-instruct-q4_k.gguf"
if [ -n "${STELNET_CHAT_TEST_MODEL:-}" ]; then
    export STELNET_CHAT_TEST_MODEL
elif [ -f "$_chat_default" ]; then
    export STELNET_CHAT_TEST_MODEL="$_chat_default"
fi
unset _chat_default

# MOSS-Audio (OpenMOSS-Team/MOSS-Audio-4B-Instruct): audio understanding + ASR
export STELNET_MODEL_MOSS_AUDIO="${STELNET_MODEL_MOSS_AUDIO:-$STELNET_MODELS_DIR/moss-audio-4b-instruct-q4_k.gguf}"

# Mini-Omni2 (gpt-omni/mini-omni2): Whisper-small + Qwen2-0.5B
export STELNET_MODEL_MINI_OMNI2="${STELNET_MODEL_MINI_OMNI2:-$STELNET_MODELS_DIR/mini-omni2-q4_k.gguf}"
export STELNET_MODEL_SNAC="${STELNET_MODEL_SNAC:-$STELNET_MODELS_DIR/snac-24khz.gguf}"

# ── Nemotron (streaming ASR) ──
export STELNET_MODEL_NEMOTRON="${STELNET_MODEL_NEMOTRON:-$STELNET_MODELS_DIR/nemotron-3.5-asr-streaming-0.6b-q4_k.gguf}"
export STELNET_MODEL_NEMOTRON_F16="${STELNET_MODEL_NEMOTRON_F16:-$STELNET_MODELS_DIR/nemotron-3.5-asr-streaming-0.6b-f16.gguf}"

# ── LFM2-Audio ──
export STELNET_MODEL_LFM2="${STELNET_MODEL_LFM2:-$STELNET_MODELS_DIR/lfm2-audio-1.5b-q5_k.gguf}"

# ── Dia TTS ──
export STELNET_MODEL_DIA="${STELNET_MODEL_DIA:-$STELNET_MODELS_DIR/dia-1.6b-q4_k.gguf}"

# ── OuteTTS + WavTokenizer ──
export STELNET_MODEL_OUTETTS="${STELNET_MODEL_OUTETTS:-$STELNET_MODELS_DIR/outetts-0.3-1b-q4k-final.gguf}"
export STELNET_MODEL_WAVTOK="${STELNET_MODEL_WAVTOK:-$STELNET_MODELS_DIR/wavtokenizer-decoder-f16.gguf}"

echo "Live test env configured (STELNET_MODELS_DIR=$STELNET_MODELS_DIR)"
