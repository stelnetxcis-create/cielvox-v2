// stelnet_session.h — forward declarations for the session C ABI.
//
// AUTO-GENERATED from CA_EXPORT definitions in src/stelnet_c_api.cpp.
// Suppresses -Wmissing-declarations when stelnet_c_api.cpp includes this.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef STELNET_SHARED
#ifdef _WIN32
#ifdef STELNET_BUILD
#define STELNET_SESSION_API __declspec(dllexport)
#else
#define STELNET_SESSION_API __declspec(dllimport)
#endif
#else
#define STELNET_SESSION_API __attribute__((visibility("default")))
#endif
#else
#define STELNET_SESSION_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Whisper types (defined in whisper.h, forward-declared here so this
// header is self-contained without pulling in the full whisper API).
struct whisper_context;
struct whisper_context_params;
struct whisper_full_params;

struct stelnet_align_result;
typedef struct stelnet_align_result stelnet_align_result;
struct stelnet_diarize_opts_abi;
typedef struct stelnet_diarize_opts_abi stelnet_diarize_opts_abi;
struct stelnet_diarize_seg_abi;
typedef struct stelnet_diarize_seg_abi stelnet_diarize_seg_abi;
struct stelnet_open_params_v1;
typedef struct stelnet_open_params_v1 stelnet_open_params_v1;
struct stelnet_session;
typedef struct stelnet_session stelnet_session;
struct stelnet_session_result;
typedef struct stelnet_session_result stelnet_session_result;
struct stelnet_stream;
typedef struct stelnet_stream stelnet_stream;
struct stelnet_vad_abi_opts;
typedef struct stelnet_vad_abi_opts stelnet_vad_abi_opts;
struct parakeet_context;
typedef struct parakeet_context parakeet_context;
struct parakeet_result;
typedef struct parakeet_result parakeet_result;
struct whisper_context;
typedef struct whisper_context whisper_context;
struct whisper_context_params;
typedef struct whisper_context_params whisper_context_params;

STELNET_SESSION_API int stelnet_get_progress(void);
STELNET_SESSION_API void stelnet_reset_progress(void);
STELNET_SESSION_API void stelnet_params_set_language(whisper_full_params* p, const char* lang);
STELNET_SESSION_API void stelnet_params_set_translate(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_detect_language(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_token_timestamps(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_n_threads(whisper_full_params* p, int n);
STELNET_SESSION_API void stelnet_params_set_max_len(whisper_full_params* p, int n);
STELNET_SESSION_API void stelnet_params_set_best_of(whisper_full_params* p, int n);
STELNET_SESSION_API void stelnet_params_set_split_on_word(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_no_context(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_single_segment(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_print_realtime(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_print_progress(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_print_timestamps(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_print_special(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_suppress_blank(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_temperature(whisper_full_params* p, float t);
STELNET_SESSION_API void stelnet_params_set_max_tokens(whisper_full_params* p, int n);
STELNET_SESSION_API void stelnet_params_set_initial_prompt(whisper_full_params* p, const char* prompt);
STELNET_SESSION_API void stelnet_params_set_vad(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_params_set_vad_model_path(whisper_full_params* p, const char* path);
STELNET_SESSION_API void stelnet_params_set_vad_threshold(whisper_full_params* p, float t);
STELNET_SESSION_API void stelnet_params_set_vad_min_speech_ms(whisper_full_params* p, int ms);
STELNET_SESSION_API void stelnet_params_set_vad_min_silence_ms(whisper_full_params* p, int ms);
STELNET_SESSION_API void stelnet_params_set_tdrz(whisper_full_params* p, int v);
STELNET_SESSION_API void stelnet_ctx_params_set_dtw(whisper_context_params* p, bool enable, int aheads_preset,
                                                      int n_top);
STELNET_SESSION_API int64_t stelnet_token_t0(whisper_context* ctx, int i_seg, int i_tok);
STELNET_SESSION_API int64_t stelnet_token_t1(whisper_context* ctx, int i_seg, int i_tok);
STELNET_SESSION_API float stelnet_token_p(whisper_context* ctx, int i_seg, int i_tok);
STELNET_SESSION_API int64_t stelnet_token_dtw_t(whisper_context* ctx, int i_segment, int i_token);
STELNET_SESSION_API void stelnet_params_set_alt_n(whisper_full_params* p, int n);
STELNET_SESSION_API int stelnet_token_n_alts(whisper_context* ctx, int i_seg, int i_tok);
STELNET_SESSION_API int32_t stelnet_token_alt_id(whisper_context* ctx, int i_seg, int i_tok, int i_alt);
STELNET_SESSION_API float stelnet_token_alt_p(whisper_context* ctx, int i_seg, int i_tok, int i_alt);
STELNET_SESSION_API int stelnet_token_alt_text(whisper_context* ctx, int i_seg, int i_tok, int i_alt, char* out,
                                                 int out_cap);
STELNET_SESSION_API float stelnet_detect_language(whisper_context* ctx, const float* pcm, int n_samples,
                                                    int n_threads, char* out_code, int out_cap);
STELNET_SESSION_API int stelnet_vad_segments(const char* vad_model_path, const float* pcm, int n_samples,
                                               int sample_rate, float threshold, int min_speech_ms, int min_silence_ms,
                                               int n_threads, bool use_gpu, float** out_spans);
STELNET_SESSION_API int stelnet_vad_slices(const char* vad_model_path, const float* pcm, int n_samples,
                                             int sample_rate, float threshold, int min_speech_ms, int min_silence_ms,
                                             int speech_pad_ms, float max_chunk_duration_s, int n_threads,
                                             float** out_spans);
STELNET_SESSION_API void stelnet_vad_free(float* spans);
STELNET_SESSION_API int stelnet_watermark_load_model(const char* gguf_path);
STELNET_SESSION_API float stelnet_watermark_detect(const float* pcm, int n_samples);
STELNET_SESSION_API void stelnet_watermark_embed(float* pcm, int n_samples, float alpha);
STELNET_SESSION_API int stelnet_lcs_dedup_prefix_count(const int32_t* prev_tail_tokens, int n_prev,
                                                         const int32_t* curr_tokens, int n_curr, int min_lcs_length);
STELNET_SESSION_API stelnet_stream* stelnet_stream_open(whisper_context* ctx, int n_threads, int step_ms,
                                                           int length_ms, int keep_ms, const char* language,
                                                           int translate);
STELNET_SESSION_API void stelnet_stream_close(stelnet_stream* s);
STELNET_SESSION_API int stelnet_stream_feed(stelnet_stream* s, const float* pcm, int n_samples);
STELNET_SESSION_API int stelnet_stream_get_text(stelnet_stream* s, char* out_text, int out_cap, double* out_t0_s,
                                                  double* out_t1_s, int64_t* out_counter);
STELNET_SESSION_API int stelnet_stream_flush(stelnet_stream* s);
STELNET_SESSION_API void stelnet_stream_set_live_decode(stelnet_stream* s, int enabled);
STELNET_SESSION_API parakeet_context* stelnet_parakeet_init(const char* model_path, int n_threads, int use_flash);
STELNET_SESSION_API void stelnet_parakeet_free(parakeet_context* ctx);
STELNET_SESSION_API parakeet_result* stelnet_parakeet_transcribe(parakeet_context* ctx, const float* pcm,
                                                                   int n_samples, int64_t t_offset_cs);
STELNET_SESSION_API const char* stelnet_parakeet_result_text(parakeet_result* r);
STELNET_SESSION_API int stelnet_parakeet_result_n_words(parakeet_result* r);
STELNET_SESSION_API const char* stelnet_parakeet_result_word_text(parakeet_result* r, int i);
STELNET_SESSION_API int64_t stelnet_parakeet_result_word_t0(parakeet_result* r, int i);
STELNET_SESSION_API int64_t stelnet_parakeet_result_word_t1(parakeet_result* r, int i);
STELNET_SESSION_API int stelnet_parakeet_result_n_tokens(parakeet_result* r);
STELNET_SESSION_API const char* stelnet_parakeet_result_token_text(parakeet_result* r, int i);
STELNET_SESSION_API int64_t stelnet_parakeet_result_token_t0(parakeet_result* r, int i);
STELNET_SESSION_API int64_t stelnet_parakeet_result_token_t1(parakeet_result* r, int i);
STELNET_SESSION_API float stelnet_parakeet_result_token_p(parakeet_result* r, int i);
STELNET_SESSION_API void stelnet_parakeet_result_free(parakeet_result* r);
STELNET_SESSION_API int stelnet_detect_backend_from_gguf(const char* path, char* out_name, int out_cap);
STELNET_SESSION_API stelnet_session* stelnet_session_open_explicit(const char* model_path, const char* backend_name,
                                                                      int n_threads);
STELNET_SESSION_API stelnet_session* stelnet_session_open(const char* model_path, int n_threads);
STELNET_SESSION_API stelnet_session* stelnet_session_open_with_params(const char* model_path,
                                                                         const char* backend_name,
                                                                         const stelnet_open_params_v1* params);
STELNET_SESSION_API const char* stelnet_session_backend(stelnet_session* s);
STELNET_SESSION_API int stelnet_session_available_backends(char* out_csv, int out_cap);
STELNET_SESSION_API stelnet_session_result* stelnet_session_transcribe_lang(stelnet_session* s, const float* pcm,
                                                                               int n_samples, const char* language);
STELNET_SESSION_API stelnet_session_result* stelnet_session_transcribe(stelnet_session* s, const float* pcm,
                                                                          int n_samples);
STELNET_SESSION_API stelnet_session_result* stelnet_session_transcribe_vad_lang(
    stelnet_session* s, const float* pcm, int n_samples, int sample_rate, const char* vad_model_path,
    const stelnet_vad_abi_opts* opts_or_null, const char* language);
STELNET_SESSION_API stelnet_session_result* stelnet_session_transcribe_vad(
    stelnet_session* s, const float* pcm, int n_samples, int sample_rate, const char* vad_model_path,
    const stelnet_vad_abi_opts* opts_or_null);
STELNET_SESSION_API int stelnet_diarize_segments_abi(const float* left_pcm, const float* right_pcm, int32_t n_samples,
                                                       int32_t is_stereo, stelnet_diarize_seg_abi* segs,
                                                       int32_t n_segs, const stelnet_diarize_opts_abi* opts);
STELNET_SESSION_API int stelnet_detect_language_pcm(const float* samples, int32_t n_samples, int32_t method,
                                                      const char* model_path, int32_t n_threads, int32_t use_gpu,
                                                      int32_t gpu_device, int32_t flash_attn, char* out_lang_buf,
                                                      int32_t out_lang_cap, float* out_confidence);
STELNET_SESSION_API int stelnet_enhance_audio_rnnoise(const float* in_pcm, int32_t n_samples, float* out_pcm,
                                                        int32_t out_cap);
STELNET_SESSION_API int stelnet_text_detect_language(const char* text, const char* model_path, int32_t n_threads,
                                                       char* out_label_buf, int32_t out_label_cap,
                                                       float* out_confidence);
STELNET_SESSION_API stelnet_align_result* stelnet_align_words_abi(const char* aligner_model, const char* transcript,
                                                                     const float* samples, int32_t n_samples,
                                                                     int64_t t_offset_cs, int32_t n_threads);
STELNET_SESSION_API int stelnet_align_result_n_words(stelnet_align_result* r);
STELNET_SESSION_API const char* stelnet_align_result_word_text(stelnet_align_result* r, int i);
STELNET_SESSION_API int64_t stelnet_align_result_word_t0(stelnet_align_result* r, int i);
STELNET_SESSION_API int64_t stelnet_align_result_word_t1(stelnet_align_result* r, int i);
STELNET_SESSION_API void stelnet_align_result_free(stelnet_align_result* r);
STELNET_SESSION_API int stelnet_cache_ensure_file_abi(const char* filename, const char* url, int32_t quiet,
                                                        const char* cache_dir_override, char* out_buf, int32_t out_cap);
STELNET_SESSION_API int stelnet_cache_dir_abi(const char* cache_dir_override, char* out_buf, int32_t out_cap);
STELNET_SESSION_API int stelnet_registry_lookup_abi(const char* backend, char* out_filename, int32_t filename_cap,
                                                      char* out_url, int32_t url_cap, char* out_size, int32_t size_cap);
STELNET_SESSION_API int stelnet_registry_lookup_by_filename_abi(const char* filename, char* out_filename,
                                                                  int32_t filename_cap, char* out_url, int32_t url_cap,
                                                                  char* out_size, int32_t size_cap);
STELNET_SESSION_API int stelnet_registry_list_backends_abi(char* out_csv, int32_t out_cap);
STELNET_SESSION_API int stelnet_session_result_n_segments(stelnet_session_result* r);
STELNET_SESSION_API const char* stelnet_session_result_segment_text(stelnet_session_result* r, int i);
STELNET_SESSION_API int64_t stelnet_session_result_segment_t0(stelnet_session_result* r, int i);
STELNET_SESSION_API int64_t stelnet_session_result_segment_t1(stelnet_session_result* r, int i);
STELNET_SESSION_API int stelnet_session_result_n_words(stelnet_session_result* r, int i_seg);
STELNET_SESSION_API const char* stelnet_session_result_word_text(stelnet_session_result* r, int i_seg, int i_word);
STELNET_SESSION_API int64_t stelnet_session_result_word_t0(stelnet_session_result* r, int i_seg, int i_word);
STELNET_SESSION_API int64_t stelnet_session_result_word_t1(stelnet_session_result* r, int i_seg, int i_word);
STELNET_SESSION_API float stelnet_session_result_word_p(stelnet_session_result* r, int i_seg, int i_word);
STELNET_SESSION_API int stelnet_session_result_word_n_alts(stelnet_session_result* r, int i_seg, int i_word);
STELNET_SESSION_API const char* stelnet_session_result_word_alt_text(stelnet_session_result* r, int i_seg,
                                                                       int i_word, int i_alt);
STELNET_SESSION_API float stelnet_session_result_word_alt_p(stelnet_session_result* r, int i_seg, int i_word,
                                                              int i_alt);
STELNET_SESSION_API void stelnet_session_result_free(stelnet_session_result* r);
STELNET_SESSION_API int stelnet_session_set_codec_path(stelnet_session* s, const char* path);
STELNET_SESSION_API int stelnet_session_set_voice(stelnet_session* s, const char* path,
                                                    const char* ref_text_or_null);
STELNET_SESSION_API int stelnet_session_set_speaker_name(stelnet_session* s, const char* name);
STELNET_SESSION_API int stelnet_session_set_speaker_id(stelnet_session* s, int id);
STELNET_SESSION_API int stelnet_session_n_speakers(stelnet_session* s);
STELNET_SESSION_API const char* stelnet_session_get_speaker_name(stelnet_session* s, int i);
STELNET_SESSION_API int stelnet_session_set_instruct(stelnet_session* s, const char* instruct);
STELNET_SESSION_API int stelnet_session_is_custom_voice(stelnet_session* s);
STELNET_SESSION_API int stelnet_session_is_voice_design(stelnet_session* s);
STELNET_SESSION_API float* stelnet_session_synthesize_raw(stelnet_session* s, const char* text, int* out_n_samples);
STELNET_SESSION_API float* stelnet_session_synthesize(stelnet_session* s, const char* text, int* out_n_samples);
STELNET_SESSION_API void stelnet_pcm_free(float* pcm);
STELNET_SESSION_API float* stelnet_session_speech_to_speech(stelnet_session* s, const float* in_samples,
                                                              int n_in_samples, char** out_text, int* out_n_samples);
STELNET_SESSION_API int stelnet_session_set_hotwords(stelnet_session* s, const char* hotwords, float boost);
STELNET_SESSION_API const char* stelnet_session_last_synth_error(stelnet_session* s);
STELNET_SESSION_API char* stelnet_session_translate_text(stelnet_session* s, const char* text, const char* src_lang,
                                                           const char* tgt_lang, int max_tokens);
STELNET_SESSION_API void stelnet_session_translate_text_free(char* text);
STELNET_SESSION_API stelnet_stream* stelnet_session_stream_open(stelnet_session* s, int n_threads, int step_ms,
                                                                   int length_ms, int keep_ms, const char* language,
                                                                   int translate);
STELNET_SESSION_API void stelnet_session_close(stelnet_session* s);
STELNET_SESSION_API void* stelnet_punc_init(const char* model_path);
STELNET_SESSION_API const char* stelnet_punc_process(void* ctx, const char* text);
STELNET_SESSION_API void stelnet_punc_free_text(const char* text);
STELNET_SESSION_API void stelnet_punc_free(void* ctx);
STELNET_SESSION_API void* stelnet_punc_init(const char*);
STELNET_SESSION_API const char* stelnet_punc_process(void*, const char*);
STELNET_SESSION_API void stelnet_punc_free_text(const char*);
STELNET_SESSION_API void stelnet_punc_free(void*);
STELNET_SESSION_API void* stelnet_truecase_init(const char* model_path);
STELNET_SESSION_API const char* stelnet_truecase_process(void* ctx, const char* text);
STELNET_SESSION_API void stelnet_truecase_free_text(const char* text);
STELNET_SESSION_API void stelnet_truecase_free(void* ctx);
STELNET_SESSION_API void* stelnet_truecase_init(const char* model_path);
STELNET_SESSION_API const char* stelnet_truecase_process(void* ctx, const char* text);
STELNET_SESSION_API void stelnet_truecase_free_text(const char* text);
STELNET_SESSION_API void stelnet_truecase_free(void* ctx);
STELNET_SESSION_API void* stelnet_truecase_init(const char*);
STELNET_SESSION_API const char* stelnet_truecase_process(void*, const char*);
STELNET_SESSION_API void stelnet_truecase_free_text(const char*);
STELNET_SESSION_API void stelnet_truecase_free(void*);
STELNET_SESSION_API void* stelnet_pcs_init(const char* model_path);
STELNET_SESSION_API const char* stelnet_pcs_process(void* ctx, const char* text);
STELNET_SESSION_API void stelnet_pcs_free_text(const char* text);
STELNET_SESSION_API void stelnet_pcs_free(void* ctx);
STELNET_SESSION_API void* stelnet_pcs_init(const char*);
STELNET_SESSION_API const char* stelnet_pcs_process(void*, const char*);
STELNET_SESSION_API void stelnet_pcs_free_text(const char*);
STELNET_SESSION_API void stelnet_pcs_free(void*);
STELNET_SESSION_API int stelnet_transcribe_parallel(struct whisper_context* ctx, struct whisper_full_params params,
                                                      const float* samples, int n_samples, int n_processors);
STELNET_SESSION_API const char* stelnet_c_api_version(void);
STELNET_SESSION_API const char* stelnet_dart_helpers_version(void);
STELNET_SESSION_API bool stelnet_kokoro_lang_is_german_abi(const char* lang);
STELNET_SESSION_API bool stelnet_kokoro_lang_has_native_voice_abi(const char* lang);
STELNET_SESSION_API int stelnet_kokoro_resolve_model_for_lang_abi(const char* model_path, const char* lang,
                                                                    char* out_path, int out_path_len);
STELNET_SESSION_API int stelnet_kokoro_resolve_fallback_voice_abi(const char* model_path, const char* lang,
                                                                    char* out_path, int out_path_len, char* out_picked,
                                                                    int out_picked_len);
STELNET_SESSION_API int stelnet_session_kokoro_clear_phoneme_cache(stelnet_session* s);
STELNET_SESSION_API bool stelnet_kokoro_lang_is_german_abi(const char*);
STELNET_SESSION_API bool stelnet_kokoro_lang_has_native_voice_abi(const char*);
STELNET_SESSION_API int stelnet_kokoro_resolve_model_for_lang_abi(const char*, const char*, char*, int);
STELNET_SESSION_API int stelnet_kokoro_resolve_fallback_voice_abi(const char*, const char*, char*, int, char*, int);
STELNET_SESSION_API int stelnet_session_kokoro_clear_phoneme_cache(stelnet_session*);
STELNET_SESSION_API int stelnet_session_set_source_language(stelnet_session* s, const char* lang);
STELNET_SESSION_API int stelnet_session_set_target_language(stelnet_session* s, const char* lang);
STELNET_SESSION_API int stelnet_session_set_punctuation(stelnet_session* s, int enable);
// Select + load a punctuation-restoration model (alias auto|firered|fullstop|
// punctuate-all|pcs, or a .gguf path; "none"/NULL unloads). Auto-downloads on
// first use. Restores punctuation on backends that emit none (parakeet, CTC).
// Returns 0 on success/unload, -1 bad handle, -2 load failed, -3 not compiled.
STELNET_SESSION_API int stelnet_session_set_punc_model(stelnet_session* s, const char* punc_model);
STELNET_SESSION_API int stelnet_session_set_translate(stelnet_session* s, int enable);
STELNET_SESSION_API int stelnet_session_set_ask(stelnet_session* s, const char* prompt);
STELNET_SESSION_API int stelnet_session_set_temperature(stelnet_session* s, float temperature, uint64_t seed);
STELNET_SESSION_API int stelnet_session_set_tts_seed(stelnet_session* s, uint64_t seed);
STELNET_SESSION_API int stelnet_session_set_tts_steps(stelnet_session* s, int steps);
STELNET_SESSION_API int stelnet_session_set_g2p_dict(stelnet_session* s, const char* source);
STELNET_SESSION_API int stelnet_session_set_top_p(stelnet_session* s, float top_p);
STELNET_SESSION_API int stelnet_session_set_min_p(stelnet_session* s, float min_p);
STELNET_SESSION_API int stelnet_session_set_repetition_penalty(stelnet_session* s, float r);
STELNET_SESSION_API int stelnet_session_set_cfg_weight(stelnet_session* s, float cfg_weight);
STELNET_SESSION_API int stelnet_session_set_exaggeration(stelnet_session* s, float exaggeration);
STELNET_SESSION_API int stelnet_session_set_max_speech_tokens(stelnet_session* s, int n);
STELNET_SESSION_API int stelnet_session_set_length_scale(stelnet_session* s, float scale);
STELNET_SESSION_API int stelnet_session_set_best_of(stelnet_session* s, int n);
STELNET_SESSION_API int stelnet_session_set_max_new_tokens(stelnet_session* s, int n);
STELNET_SESSION_API int stelnet_session_set_frequency_penalty(stelnet_session* s, float penalty);
STELNET_SESSION_API int stelnet_session_set_beam_size(stelnet_session* s, int n);
STELNET_SESSION_API int stelnet_session_set_grammar_text(stelnet_session* s, const char* gbnf_text,
                                                           const char* root_rule, float penalty);
STELNET_SESSION_API int stelnet_session_set_fallback_thresholds(stelnet_session* s, float entropy_thold,
                                                                  float logprob_thold, float no_speech_thold,
                                                                  float temperature_inc);
STELNET_SESSION_API int stelnet_session_set_alt_n(stelnet_session* s, int n);
STELNET_SESSION_API int stelnet_session_set_whisper_decode_extras(stelnet_session* s, int suppress_nst,
                                                                    const char* suppress_regex,
                                                                    int carry_initial_prompt);
STELNET_SESSION_API int stelnet_session_detect_language(stelnet_session* s, const float* pcm, int n_samples,
                                                          const char* lid_model_path, int method, char* out_lang,
                                                          int out_lang_cap, float* out_prob);
STELNET_SESSION_API void* stelnet_titanet_init(const char* model_path, int32_t n_threads);
STELNET_SESSION_API void stelnet_titanet_free(void* ctx);
STELNET_SESSION_API int32_t stelnet_titanet_embed(void* ctx, const float* pcm_16k, int32_t n_samples, float* out);
STELNET_SESSION_API float stelnet_titanet_cosine_sim(const float* a, const float* b, int32_t dim);
STELNET_SESSION_API void* stelnet_speaker_db_load(const char* dir_path);
STELNET_SESSION_API void stelnet_speaker_db_free(void* db);
STELNET_SESSION_API int32_t stelnet_speaker_db_count(const void* db);
STELNET_SESSION_API float stelnet_speaker_db_match(const void* db, const float* embedding, int32_t dim,
                                                     float threshold, char* out_name, int32_t out_cap);
STELNET_SESSION_API int32_t stelnet_speaker_db_enroll(const char* dir_path, const char* name, const float* embedding,
                                                        int32_t dim);
STELNET_SESSION_API void* stelnet_speaker_embedder_make_abi(const char* model_spec, int32_t n_threads,
                                                              const char* cache_dir);
STELNET_SESSION_API void stelnet_speaker_embedder_free_abi(void* embedder);
STELNET_SESSION_API int32_t stelnet_speaker_embedder_dim_abi(const void* embedder);
STELNET_SESSION_API int32_t stelnet_speaker_embedder_embed_abi(void* embedder, const float* pcm_16k,
                                                                 int32_t n_samples, float* out);
STELNET_SESSION_API const char* stelnet_speaker_embedder_name_abi(const void* embedder);
STELNET_SESSION_API int32_t stelnet_speaker_cluster_abi(const float* embeddings, int32_t n, int32_t dim,
                                                          float merge_threshold, int32_t max_speakers,
                                                          int32_t* labels_out);
STELNET_SESSION_API void* stelnet_pyannote_cache_compute_abi(const float* full_audio, int32_t n_samples,
                                                               const char* model_path, int32_t n_threads);
STELNET_SESSION_API void stelnet_pyannote_cache_free_abi(void* cache);
STELNET_SESSION_API int32_t stelnet_pyannote_cache_apply_abi(const void* cache, int64_t slice_t0_cs,
                                                               stelnet_diarize_seg_abi* segs, int32_t n_segs);

#ifdef __cplusplus
}
#endif
