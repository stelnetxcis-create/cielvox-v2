"""Smoke test: every CA_EXPORT symbol from stelnet_c_api.cpp is reachable
from the Python ctypes binding.

Does NOT instantiate models or run inference — purely checks that the
binding declares (and can look up) all 150 exported C-ABI functions.
Requires STELNET_LIB_PATH pointing at a built libstelnet.{so,dylib}.

    STELNET_LIB_PATH=build/src/libstelnet.so python -m pytest tests/test_binding_parity.py -v
"""

import ctypes
import os
import sys

import pytest

# All 150 CA_EXPORT symbols (sorted, from:
#   grep -oP 'CA_EXPORT\s+\w+[\s*]+\K(stelnet_\w+)' src/stelnet_c_api.cpp | sort -u
# )
ALL_SYMBOLS = [
    "stelnet_align_result_free",
    "stelnet_align_result_n_words",
    "stelnet_align_result_word_t0",
    "stelnet_align_result_word_t1",
    "stelnet_align_words_abi",
    "stelnet_cache_dir_abi",
    "stelnet_cache_ensure_file_abi",
    "stelnet_detect_backend_from_gguf",
    "stelnet_detect_language",
    "stelnet_detect_language_pcm",
    "stelnet_diarize_segments_abi",
    "stelnet_enhance_audio_rnnoise",
    "stelnet_kokoro_lang_has_native_voice_abi",
    "stelnet_kokoro_lang_is_german_abi",
    "stelnet_kokoro_resolve_fallback_voice_abi",
    "stelnet_kokoro_resolve_model_for_lang_abi",
    "stelnet_lcs_dedup_prefix_count",
    "stelnet_parakeet_free",
    "stelnet_parakeet_init",
    "stelnet_parakeet_result_free",
    "stelnet_parakeet_result_n_tokens",
    "stelnet_parakeet_result_n_words",
    "stelnet_parakeet_result_token_p",
    "stelnet_parakeet_result_token_t0",
    "stelnet_parakeet_result_token_t1",
    "stelnet_parakeet_result_word_t0",
    "stelnet_parakeet_result_word_t1",
    "stelnet_parakeet_transcribe",
    "stelnet_params_set_alt_n",
    "stelnet_params_set_best_of",
    "stelnet_params_set_detect_language",
    "stelnet_params_set_initial_prompt",
    "stelnet_params_set_language",
    "stelnet_params_set_max_len",
    "stelnet_params_set_max_tokens",
    "stelnet_params_set_no_context",
    "stelnet_params_set_n_threads",
    "stelnet_params_set_print_progress",
    "stelnet_params_set_print_realtime",
    "stelnet_params_set_print_special",
    "stelnet_params_set_print_timestamps",
    "stelnet_params_set_single_segment",
    "stelnet_params_set_split_on_word",
    "stelnet_params_set_suppress_blank",
    "stelnet_params_set_tdrz",
    "stelnet_params_set_temperature",
    "stelnet_params_set_token_timestamps",
    "stelnet_params_set_translate",
    "stelnet_params_set_vad",
    "stelnet_params_set_vad_min_silence_ms",
    "stelnet_params_set_vad_min_speech_ms",
    "stelnet_params_set_vad_model_path",
    "stelnet_params_set_vad_threshold",
    "stelnet_pcm_free",
    "stelnet_punc_free",
    "stelnet_punc_free_text",
    "stelnet_punc_init",
    "stelnet_pyannote_cache_apply_abi",
    "stelnet_pyannote_cache_compute_abi",
    "stelnet_pyannote_cache_free_abi",
    "stelnet_registry_list_backends_abi",
    "stelnet_registry_lookup_abi",
    "stelnet_registry_lookup_by_filename_abi",
    "stelnet_session_available_backends",
    "stelnet_session_close",
    "stelnet_session_detect_language",
    "stelnet_session_is_custom_voice",
    "stelnet_session_is_voice_design",
    "stelnet_session_kokoro_clear_phoneme_cache",
    "stelnet_session_n_speakers",
    "stelnet_session_open",
    "stelnet_session_open_explicit",
    "stelnet_session_open_with_params",
    "stelnet_session_result_free",
    "stelnet_session_result_n_segments",
    "stelnet_session_result_n_words",
    "stelnet_session_result_segment_t0",
    "stelnet_session_result_segment_t1",
    "stelnet_session_result_word_alt_p",
    "stelnet_session_result_word_n_alts",
    "stelnet_session_result_word_p",
    "stelnet_session_result_word_t0",
    "stelnet_session_result_word_t1",
    "stelnet_session_set_alt_n",
    "stelnet_session_set_ask",
    "stelnet_session_set_beam_size",
    "stelnet_session_set_best_of",
    "stelnet_session_set_cfg_weight",
    "stelnet_session_set_codec_path",
    "stelnet_session_set_exaggeration",
    "stelnet_session_set_fallback_thresholds",
    "stelnet_session_set_frequency_penalty",
    "stelnet_session_set_grammar_text",
    "stelnet_session_set_instruct",
    "stelnet_session_set_length_scale",
    "stelnet_session_set_max_new_tokens",
    "stelnet_session_set_max_speech_tokens",
    "stelnet_session_set_min_p",
    "stelnet_session_set_punc_model",
    "stelnet_session_set_punctuation",
    "stelnet_session_set_repetition_penalty",
    "stelnet_session_set_source_language",
    "stelnet_session_set_speaker_name",
    "stelnet_session_set_target_language",
    "stelnet_session_set_temperature",
    "stelnet_session_set_top_p",
    "stelnet_session_set_translate",
    "stelnet_session_set_tts_seed",
    "stelnet_session_set_tts_steps",
    "stelnet_session_set_voice",
    "stelnet_session_set_whisper_decode_extras",
    "stelnet_session_stream_open",
    "stelnet_session_synthesize",
    "stelnet_session_transcribe",
    "stelnet_session_transcribe_lang",
    "stelnet_session_transcribe_vad",
    "stelnet_session_transcribe_vad_lang",
    "stelnet_session_translate_text",
    "stelnet_session_translate_text_free",
    "stelnet_speaker_cluster_abi",
    "stelnet_speaker_db_count",
    "stelnet_speaker_db_enroll",
    "stelnet_speaker_db_free",
    "stelnet_speaker_db_load",
    "stelnet_speaker_db_match",
    "stelnet_speaker_embedder_dim_abi",
    "stelnet_speaker_embedder_embed_abi",
    "stelnet_speaker_embedder_free_abi",
    "stelnet_speaker_embedder_make_abi",
    "stelnet_stream_close",
    "stelnet_stream_feed",
    "stelnet_stream_flush",
    "stelnet_stream_get_text",
    "stelnet_stream_open",
    "stelnet_stream_set_live_decode",
    "stelnet_text_detect_language",
    "stelnet_titanet_cosine_sim",
    "stelnet_titanet_embed",
    "stelnet_titanet_free",
    "stelnet_titanet_init",
    "stelnet_token_alt_id",
    "stelnet_token_alt_p",
    "stelnet_token_alt_text",
    "stelnet_token_n_alts",
    "stelnet_token_p",
    "stelnet_token_t0",
    "stelnet_token_t1",
    "stelnet_vad_free",
    "stelnet_vad_segments",
    "stelnet_vad_slices",
]


@pytest.fixture(scope="module")
def lib():
    lib_path = os.environ.get("STELNET_LIB_PATH")
    if not lib_path:
        # Try common build paths
        for candidate in [
            "build/src/libstelnet.so",
            "/mnt/volume1/build-main/src/libstelnet.so",
            "build/libstelnet.so",
        ]:
            if os.path.exists(candidate):
                lib_path = candidate
                break
    if not lib_path or not os.path.exists(lib_path):
        pytest.skip("libstelnet not found — set STELNET_LIB_PATH")
    return ctypes.CDLL(lib_path)


@pytest.mark.parametrize("symbol", ALL_SYMBOLS)
def test_symbol_resolves(lib, symbol):
    """Every CA_EXPORT symbol from stelnet_c_api.cpp must be resolvable."""
    assert hasattr(lib, symbol), f"symbol {symbol} not found in loaded libstelnet"


def test_symbol_count(lib):
    """Sanity: we expect exactly 150 stelnet_* symbols."""
    assert len(ALL_SYMBOLS) == 150, f"expected 150 symbols, got {len(ALL_SYMBOLS)}"


def test_python_binding_imports():
    """The Python binding module must import without error."""
    # This validates syntax and top-level structure.
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))
    import ast
    binding_path = os.path.join(
        os.path.dirname(__file__), "..", "python", "stelnet", "_binding.py"
    )
    with open(binding_path) as f:
        ast.parse(f.read())


def test_python_binding_declares_all_symbols():
    """Every CA_EXPORT symbol must appear somewhere in _binding.py."""
    binding_path = os.path.join(
        os.path.dirname(__file__), "..", "python", "stelnet", "_binding.py"
    )
    with open(binding_path) as f:
        content = f.read()
    missing = [s for s in ALL_SYMBOLS if s not in content]
    assert missing == [], f"Python binding missing symbols: {missing}"
