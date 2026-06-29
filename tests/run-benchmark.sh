#!/usr/bin/env bash
# run-benchmark.sh — quick smoke test for the ASR benchmark framework.
#
# Runs parakeet on the JFK sample (11s) with auto settings and verifies
# the framework produces output with reasonable metrics. Designed to be
# registered as a CTest entry (label: benchmark).
#
# Usage: ./tests/run-benchmark.sh [--quick|--full]
# Exit:  0 = pass, 1 = fail, 2 = skip (model/binary missing).

set -euo pipefail
cd "$(dirname "$0")/.."

STELNET="${STELNET_BIN:-./build/bin/stelnet}"
if [ ! -x "$STELNET" ]; then
    STELNET="./build-ninja-compile/bin/stelnet"
fi
if [ ! -x "$STELNET" ]; then
    STELNET="/tmp/build-issue89/bin/stelnet"
fi
if [ ! -x "$STELNET" ]; then
    echo "SKIP: stelnet binary not found"
    exit 2
fi

# Check for parakeet model — respect STELNET_MODELS_DIR, then fall back to
# the auto-download cache.
MODELS_DIR="${STELNET_MODELS_DIR:-$HOME/.cache/stelnet}"
MODEL="${STELNET_BENCHMARK_MODEL:-}"
if [ -z "$MODEL" ]; then
    for name in parakeet-tdt-0.6b-v3.gguf parakeet-tdt-0.6b-v3-q4_k.gguf; do
        if [ -f "$MODELS_DIR/$name" ]; then
            MODEL="$MODELS_DIR/$name"
            break
        fi
    done
fi
if [ -z "$MODEL" ]; then
    echo "SKIP: parakeet model not found (set STELNET_MODELS_DIR or STELNET_BENCHMARK_MODEL)"
    exit 2
fi

AUDIO="samples/jfk.wav"
if [ ! -f "$AUDIO" ]; then
    echo "SKIP: $AUDIO not found"
    exit 2
fi

echo "=== Benchmark smoke test ==="
echo "Binary: $STELNET"
echo "Model:  $MODEL"
echo "Audio:  $AUDIO"

# Run the benchmark driver
export STELNET_BIN="$STELNET"
python3 tests/benchmark_asr.py \
    --audio "$AUDIO" \
    --model "$MODEL" \
    --backend parakeet \
    --settings auto \
    --format table

rc=$?
if [ $rc -ne 0 ]; then
    echo "FAIL: benchmark driver exited with $rc"
    exit 1
fi

# Verify results file was created
RESULTS="/mnt/storage/benchmark-results/runs.jsonl"
if [ ! -f "$RESULTS" ]; then
    echo "FAIL: $RESULTS not created"
    exit 1
fi

# Check last result has reasonable word count (JFK = ~30 words)
LAST_WORDS=$(tail -1 "$RESULTS" | python3 -c "import json,sys; print(json.load(sys.stdin)['metrics']['word_count'])")
if [ "$LAST_WORDS" -lt 5 ]; then
    echo "FAIL: word_count=$LAST_WORDS (expected > 5 for JFK)"
    exit 1
fi

echo ""
echo "PASS: benchmark smoke test (word_count=$LAST_WORDS)"
exit 0
