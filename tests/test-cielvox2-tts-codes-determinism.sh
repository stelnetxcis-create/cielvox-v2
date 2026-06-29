#!/bin/bash
# test-cielvox2-tts-codes-determinism.sh — CLI-level regression gates for the
# cielvox2-tts whole-clip synthesis path. Two properties, both byte-exact:
#
#   1. Determinism: two identical `--tts` runs produce a bit-identical WAV.
#      This is the gate for the shared talker AR loop (cielvox2_tts_generate_codes_ar,
#      PLAN streaming dedup) — if a future edit perturbs sampling/embedding/KV the
#      hash changes and this fails.
#   2. O15 equivalence: QWEN3_TTS_O15=1 (code-predictor cached-graph reuse, #56)
#      produces output bit-identical to QWEN3_TTS_O15=0. O15 is a graph-construction
#      optimisation only; it must never change the audio. (NOTE: this exercises the
#      default re-alloc-each-step O15 path. It does NOT set QWEN3_TTS_O15_SKIP_REALLOC
#      — that opt-in is unsafe on CUDA and current Metal; see cielvox2_tts.cpp.)
#
# Uses the CLI (`--tts`), which runs synthesize() -> synthesize_codes (whole clip)
# with the deterministic default RNG (seed 42). No server, no network.
#
# Model discovery (first hit wins), SKIPs (exit 0) if none found:
#   STELNET_TEST_QWEN3_TTS_MODEL (+ optional STELNET_TEST_QWEN3_TTS_CODEC)
#   else a cielvox2-tts *.gguf under STELNET_MODELS / STELNET_MODELS_DIR /
#        ~/stelnet-live-cache / ~/.cache/stelnet (base or customvoice).
# A base model auto-discovers its voice-default + tokenizer siblings; a
# customvoice model auto-selects its first speaker.

set -uo pipefail
cd "$(dirname "$0")/.."

STELNET=""
for cand in build/bin/stelnet build-ninja-compile/bin/stelnet ./bin/stelnet build/bin/Release/stelnet.exe; do
    if [ -x "$cand" ]; then STELNET="$cand"; break; fi
done
if [ -z "$STELNET" ]; then
    echo "ERROR: stelnet binary not found. Build first."
    exit 2
fi

# Resolve a cielvox2-tts talker model.
MODEL="${STELNET_TEST_QWEN3_TTS_MODEL:-}"
if [ -z "$MODEL" ] || [ ! -f "$MODEL" ]; then
    MODEL=""
    for dir in "${STELNET_MODELS:-}" "${STELNET_MODELS_DIR:-}" "$HOME/stelnet-live-cache" "$HOME/.cache/stelnet"; do
        [ -n "$dir" ] && [ -d "$dir" ] || continue
        for cand in "$dir"/cielvox2-tts-*base*.gguf "$dir"/cielvox2-tts-*customvoice*.gguf "$dir"/cielvox2-tts-*.gguf; do
            # Skip non-talker siblings (tokenizer / voice packs).
            case "$cand" in
                *tokenizer*|*codec*|*voice*) continue ;;
            esac
            [ -f "$cand" ] && { MODEL="$cand"; break; }
        done
        [ -n "$MODEL" ] && break
    done
fi
if [ -z "$MODEL" ] || [ ! -f "$MODEL" ]; then
    echo "SKIP: no cielvox2-tts GGUF found (set STELNET_TEST_QWEN3_TTS_MODEL or STELNET_MODELS_DIR)"
    exit 0
fi

# Pick the backend that matches the model variant.
case "$MODEL" in
    *customvoice*|*-cv-*|*voicedesign*) BACKEND="cielvox2-tts-customvoice" ;;
    *)                                  BACKEND="cielvox2-tts" ;;
esac

# Pass an explicit codec if a tokenizer sibling is present (otherwise the CLI
# auto-discovers it next to the model).
CODEC="${STELNET_TEST_QWEN3_TTS_CODEC:-}"
if [ -z "$CODEC" ]; then
    MODEL_DIR=$(dirname "$MODEL")
    for cand in "$MODEL_DIR"/cielvox2-tts-tokenizer-12hz.gguf "$MODEL_DIR"/cielvox2-tts-tokenizer*.gguf; do
        [ -f "$cand" ] && { CODEC="$cand"; break; }
    done
fi
CODEC_ARG=()
[ -n "$CODEC" ] && CODEC_ARG=(--codec-model "$CODEC")

OUT_DIR=$(mktemp -d -t stelnet-cielvox2-tts-det.XXXXXX)
trap 'rm -rf "$OUT_DIR"' EXIT

TEXT="Determinism gate. The quick brown fox jumps over the lazy dog."

synth() { # <env-O15> <out.wav>
    QWEN3_TTS_O15="$1" "$STELNET" -m "$MODEL" --backend "$BACKEND" "${CODEC_ARG[@]}" \
        --tts "$TEXT" --tts-output "$2" --no-prints > "$OUT_DIR/log" 2>&1
}
hash_file() { shasum -a 256 "$1" | awk '{print $1}'; }

echo "=== cielvox2-tts codes determinism + O15 equivalence ==="
echo "model:   $MODEL"
echo "backend: $BACKEND"
[ -n "$CODEC" ] && echo "codec:   $CODEC"

PASS=0
FAIL=0
assert_eq() {
    local name="$1" a="$2" b="$3"
    if [ "$a" = "$b" ]; then
        echo "  PASS $name"; PASS=$((PASS + 1))
    else
        echo "  FAIL $name"; echo "       a=$a"; echo "       b=$b"; FAIL=$((FAIL + 1))
    fi
}

synth 0 "$OUT_DIR/a.wav" || { echo "  FAIL synth O15=0 #1"; cat "$OUT_DIR/log"; exit 1; }
synth 0 "$OUT_DIR/b.wav" || { echo "  FAIL synth O15=0 #2"; cat "$OUT_DIR/log"; exit 1; }
synth 1 "$OUT_DIR/c.wav" || { echo "  FAIL synth O15=1 (crash/abort?)"; cat "$OUT_DIR/log"; exit 1; }

# A non-trivial WAV must have been produced (guard against both paths emitting
# empty audio, which would make the hashes trivially equal).
SZ=$(wc -c < "$OUT_DIR/a.wav" | tr -d ' ')
if [ "$SZ" -gt 1000 ]; then
    echo "  PASS produced non-trivial WAV ($SZ bytes)"; PASS=$((PASS + 1))
else
    echo "  FAIL WAV suspiciously small ($SZ bytes)"; FAIL=$((FAIL + 1))
fi

HA=$(hash_file "$OUT_DIR/a.wav")
HB=$(hash_file "$OUT_DIR/b.wav")
HC=$(hash_file "$OUT_DIR/c.wav")

assert_eq "whole-clip synthesis is deterministic (same args -> identical WAV)" "$HA" "$HB"
assert_eq "QWEN3_TTS_O15=1 is byte-identical to O15=0" "$HA" "$HC"

echo
echo "Results: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
