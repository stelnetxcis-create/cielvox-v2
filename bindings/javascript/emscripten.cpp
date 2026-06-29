//
// This is the Javascript API of stelnet
//
// Very crude at the moment.
// Feel free to contribute and make this better!
//
// See tests/test-stelnet.js for sample usage.
//

#include "stelnet.h"

#include <emscripten.h>
#include <emscripten/bind.h>

#include <thread>
#include <vector>

// The unified Session C-ABI is declared in stelnet.h (included above)
// as `struct stelnet_session`. Legacy name alias for the Embind wrappers:
extern "C" {
typedef struct stelnet_session StelnetSession;
StelnetSession* stelnet_session_open(const char* model_path, int n_threads);
void                    stelnet_session_close(StelnetSession* s);
int                     stelnet_session_set_codec_path(StelnetSession* s, const char* path);
int                     stelnet_session_set_voice(StelnetSession* s, const char* path,
                                                   const char* ref_text_or_null);
int                     stelnet_session_set_speaker_name(StelnetSession* s, const char* name);
int                     stelnet_session_n_speakers(StelnetSession* s);
const char*             stelnet_session_get_speaker_name(StelnetSession* s, int i);
int                     stelnet_session_set_instruct(StelnetSession* s, const char* instruct);
int                     stelnet_session_is_custom_voice(StelnetSession* s);
int                     stelnet_session_is_voice_design(StelnetSession* s);
float*                  stelnet_session_synthesize(StelnetSession* s, const char* text,
                                                    int* out_n_samples);
void                    stelnet_pcm_free(float* pcm);
int                     stelnet_session_kokoro_clear_phoneme_cache(StelnetSession* s);
int                     stelnet_kokoro_resolve_model_for_lang_abi(const char* model_path, const char* lang,
                                                                   char* out_path, int out_path_len);
int                     stelnet_kokoro_resolve_fallback_voice_abi(const char* model_path, const char* lang,
                                                                   char* out_path, int out_path_len,
                                                                   char* out_picked, int out_picked_len);

// --- Full C-ABI parity declarations ---
// Session extras
int          stelnet_session_available_backends(char* out_csv, int out_cap);
StelnetSession* stelnet_session_open_explicit(const char* model_path, const char* backend_name, int n_threads);
StelnetSession* stelnet_session_open_with_params(const char* model_path, const char* backend_name, const void* params);
const char*  stelnet_session_backend(StelnetSession* s);
int          stelnet_session_set_source_language(StelnetSession* s, const char* lang);
int          stelnet_session_set_target_language(StelnetSession* s, const char* lang);
int          stelnet_session_set_punctuation(StelnetSession* s, int enable);
int          stelnet_session_set_punc_model(StelnetSession* s, const char* punc_model);
int          stelnet_session_set_hotwords(StelnetSession* s, const char* hotwords, float boost);
int          stelnet_session_set_translate(StelnetSession* s, int enable);
int          stelnet_session_set_temperature(StelnetSession* s, float temperature, unsigned long long seed);
int          stelnet_session_set_tts_seed(StelnetSession* s, unsigned long long seed);
int          stelnet_session_set_tts_steps(StelnetSession* s, int steps);
int          stelnet_session_set_max_new_tokens(StelnetSession* s, int n);
int          stelnet_session_set_frequency_penalty(StelnetSession* s, float penalty);
int          stelnet_session_set_top_p(StelnetSession* s, float top_p);
int          stelnet_session_set_min_p(StelnetSession* s, float min_p);
int          stelnet_session_set_repetition_penalty(StelnetSession* s, float r);
int          stelnet_session_set_cfg_weight(StelnetSession* s, float cfg_weight);
int          stelnet_session_set_exaggeration(StelnetSession* s, float exaggeration);
int          stelnet_session_set_max_speech_tokens(StelnetSession* s, int n);
int          stelnet_session_set_length_scale(StelnetSession* s, float scale);
int          stelnet_session_set_best_of(StelnetSession* s, int n);
int          stelnet_session_set_beam_size(StelnetSession* s, int n);
int          stelnet_session_set_grammar_text(StelnetSession* s, const char* gbnf_text,
                                               const char* root_rule, float penalty);
int          stelnet_session_set_fallback_thresholds(StelnetSession* s, float entropy_thold,
                                                      float logprob_thold, float no_speech_thold,
                                                      float temperature_inc);
int          stelnet_session_set_alt_n(StelnetSession* s, int n);
int          stelnet_session_set_whisper_decode_extras(StelnetSession* s, int suppress_nst,
                                                        const char* suppress_regex, int carry_initial_prompt);
int          stelnet_session_set_ask(StelnetSession* s, const char* prompt);
int          stelnet_session_detect_language(StelnetSession* s, const float* pcm, int n_samples,
                                              const char* lid_model_path, int method,
                                              char* out_lang, int out_lang_cap, float* out_prob);

// Session ASR transcription
struct stelnet_session_result;
struct stelnet_session_result* stelnet_session_transcribe(StelnetSession* s, const float* pcm, int n_samples);
struct stelnet_session_result* stelnet_session_transcribe_lang(StelnetSession* s, const float* pcm, int n_samples,
                                                                  const char* language);
struct stelnet_session_result* stelnet_session_transcribe_vad(StelnetSession* s, const float* pcm, int n_samples,
                                                                 int sample_rate, const char* vad_model_path, void* opts);
struct stelnet_session_result* stelnet_session_transcribe_vad_lang(StelnetSession* s, const float* pcm, int n_samples,
                                                                      int sample_rate, const char* vad_model_path, void* opts,
                                                                      const char* language);
int          stelnet_session_result_n_segments(struct stelnet_session_result* r);
const char*  stelnet_session_result_segment_text(struct stelnet_session_result* r, int i);
long long    stelnet_session_result_segment_t0(struct stelnet_session_result* r, int i);
long long    stelnet_session_result_segment_t1(struct stelnet_session_result* r, int i);
int          stelnet_session_result_n_words(struct stelnet_session_result* r, int i_seg);
const char*  stelnet_session_result_word_text(struct stelnet_session_result* r, int i_seg, int i_word);
long long    stelnet_session_result_word_t0(struct stelnet_session_result* r, int i_seg, int i_word);
long long    stelnet_session_result_word_t1(struct stelnet_session_result* r, int i_seg, int i_word);
float        stelnet_session_result_word_p(struct stelnet_session_result* r, int i_seg, int i_word);
int          stelnet_session_result_word_n_alts(struct stelnet_session_result* r, int i_seg, int i_word);
const char*  stelnet_session_result_word_alt_text(struct stelnet_session_result* r, int i_seg, int i_word, int i_alt);
float        stelnet_session_result_word_alt_p(struct stelnet_session_result* r, int i_seg, int i_word, int i_alt);
void         stelnet_session_result_free(struct stelnet_session_result* r);
char*        stelnet_session_translate_text(StelnetSession* s, const char* text, const char* src_lang,
                                             const char* tgt_lang, int max_tokens);
void         stelnet_session_translate_text_free(char* text);

// Streaming
struct StelnetStream;
struct StelnetStream* stelnet_session_stream_open(StelnetSession* s, int n_threads, int step_ms,
                                                     int length_ms, int keep_ms, const char* language, int translate);
struct StelnetStream* stelnet_stream_open(void* ctx, int n_threads, int step_ms,
                                             int length_ms, int keep_ms, const char* language, int translate);
int                    stelnet_stream_feed(struct StelnetStream* s, const float* pcm, int n_samples);
int                    stelnet_stream_get_text(struct StelnetStream* s, char* out_text, int out_cap,
                                                double* out_t0_s, double* out_t1_s, long long* out_counter);
int                    stelnet_stream_flush(struct StelnetStream* s);
void                   stelnet_stream_close(struct StelnetStream* s);
void                   stelnet_stream_set_live_decode(struct StelnetStream* s, int enabled);

// Punctuation
void*        stelnet_punc_init(const char* model_path);
const char*  stelnet_punc_process(void* ctx, const char* text);
void         stelnet_punc_free_text(const char* text);
void         stelnet_punc_free(void* ctx);

// Alignment
struct stelnet_align_result;
struct stelnet_align_result* stelnet_align_words_abi(const char* aligner_model, const char* transcript,
                                                       const float* samples, int n_samples, long long t_offset_cs,
                                                       int n_threads);
int          stelnet_align_result_n_words(struct stelnet_align_result* r);
const char*  stelnet_align_result_word_text(struct stelnet_align_result* r, int i);
long long    stelnet_align_result_word_t0(struct stelnet_align_result* r, int i);
long long    stelnet_align_result_word_t1(struct stelnet_align_result* r, int i);
void         stelnet_align_result_free(struct stelnet_align_result* r);

// VAD — declared in stelnet.h

// LCS dedup
int stelnet_lcs_dedup_prefix_count(const int* prev_tail_tokens, int n_prev,
                                    const int* curr_tokens, int n_curr, int min_lcs_length);

// params_set_* — declared in stelnet.h (use whisper_full_params*, not void*)

// Token-level accessors
long long stelnet_token_t0(void* ctx, int i_seg, int i_tok);
long long stelnet_token_t1(void* ctx, int i_seg, int i_tok);
float     stelnet_token_p(void* ctx, int i_seg, int i_tok);
int       stelnet_token_n_alts(void* ctx, int i_seg, int i_tok);
int       stelnet_token_alt_id(void* ctx, int i_seg, int i_tok, int i_alt);
float     stelnet_token_alt_p(void* ctx, int i_seg, int i_tok, int i_alt);
int       stelnet_token_alt_text(void* ctx, int i_seg, int i_tok, int i_alt, char* out, int out_cap);

// Language detection
float stelnet_detect_language(void* ctx, const float* pcm, int n_samples,
                               int n_threads, char* out_code, int out_cap);
int   stelnet_detect_language_pcm(const float* samples, int n_samples, int method,
                                    const char* model_path, int n_threads, int use_gpu,
                                    int gpu_device, int flash_attn,
                                    char* out_lang, int out_lang_cap, float* out_confidence);

// Direct Parakeet API
void* stelnet_parakeet_init(const char* model_path, int n_threads, int use_flash);
void  stelnet_parakeet_free(void* ctx);
void* stelnet_parakeet_transcribe(void* ctx, const float* pcm, int n_samples, const char* language);
const char* stelnet_parakeet_result_text(void* r);
int         stelnet_parakeet_result_n_words(void* r);
const char* stelnet_parakeet_result_word_text(void* r, int i);
long long   stelnet_parakeet_result_word_t0(void* r, int i);
long long   stelnet_parakeet_result_word_t1(void* r, int i);
int         stelnet_parakeet_result_n_tokens(void* r);
const char* stelnet_parakeet_result_token_text(void* r, int i);
long long   stelnet_parakeet_result_token_t0(void* r, int i);
long long   stelnet_parakeet_result_token_t1(void* r, int i);
float       stelnet_parakeet_result_token_p(void* r, int i);
void        stelnet_parakeet_result_free(void* r);

// Backend detection
int stelnet_detect_backend_from_gguf(const char* path, char* out_name, int out_cap);

// RNNoise audio enhancement
int stelnet_enhance_audio_rnnoise(const float* in_pcm, int n_samples, float* out_pcm, int out_cap);

// Text-LID
int stelnet_text_detect_language(const char* text, const char* model_path, int n_threads,
                                   char* out_label, int out_label_cap, float* out_confidence);

// TitaNet
void* stelnet_titanet_init(const char* model_path, int n_threads);
void  stelnet_titanet_free(void* ctx);
int   stelnet_titanet_embed(void* ctx, const float* pcm_16k, int n_samples, float* out);
float stelnet_titanet_cosine_sim(const float* a, const float* b, int dim);

// Speaker database
void* stelnet_speaker_db_load(const char* dir_path);
void  stelnet_speaker_db_free(void* db);
int   stelnet_speaker_db_count(const void* db);
float stelnet_speaker_db_match(const void* db, const float* embedding, int dim,
                                float threshold, char* out_name, int out_cap);
int   stelnet_speaker_db_enroll(const char* dir_path, const char* name,
                                 const float* embedding, int dim);

// Pluggable speaker embedder + clustering + pyannote cache
void*  stelnet_speaker_embedder_make_abi(const char* model_spec, int n_threads, const char* cache_dir);
void   stelnet_speaker_embedder_free_abi(void* embedder);
int    stelnet_speaker_embedder_dim_abi(const void* embedder);
int    stelnet_speaker_embedder_embed_abi(void* embedder, const float* pcm_16k, int n_samples, float* out);
const char* stelnet_speaker_embedder_name_abi(const void* embedder);
int    stelnet_speaker_cluster_abi(const float* embeddings, int n, int dim,
                                    float merge_threshold, int max_speakers, int* labels_out);
void*  stelnet_pyannote_cache_compute_abi(const float* full_audio, int n_samples,
                                            const char* model_path, int n_threads);
void   stelnet_pyannote_cache_free_abi(void* cache);
int    stelnet_pyannote_cache_apply_abi(const void* cache, long long slice_t0_cs,
                                         void* segs, int n_segs);

// Kokoro lang helpers
int stelnet_kokoro_lang_is_german_abi(const char* lang);
int stelnet_kokoro_lang_has_native_voice_abi(const char* lang);

// Registry + cache
int stelnet_registry_lookup_abi(const char* backend, char* out_filename, int filename_cap,
                                 char* out_url, int url_cap, char* out_size, int size_cap);
int stelnet_registry_lookup_by_filename_abi(const char* filename, char* out_filename, int filename_cap,
                                             char* out_url, int url_cap, char* out_size, int size_cap);
int stelnet_registry_list_backends_abi(char* out_csv, int out_cap);
int stelnet_cache_ensure_file_abi(const char* filename, const char* url, int quiet,
                                   const char* cache_dir_override, char* out_buf, int out_cap);
int stelnet_cache_dir_abi(const char* cache_dir_override, char* out_buf, int out_cap);

// Diarization
int stelnet_diarize_segments_abi(const float* left_pcm, const float* right_pcm, int n_samples,
                                  int is_stereo, void* segs, int n_segs, const void* opts);
}

static StelnetSession* g_tts_session = nullptr;
// Backend-agnostic ASR session (parakeet/canary/whisper/…) for the asr* surface
// below — distinct from the whisper-only g_context used by init()/full_default().
static StelnetSession* g_asr_session = nullptr;

struct whisper_context* g_context;

EMSCRIPTEN_BINDINGS(whisper) {
    emscripten::function("init", emscripten::optional_override([](const std::string& path_model) {
                             if (g_context == nullptr) {
                                 g_context = whisper_init_from_file_with_params(path_model.c_str(),
                                                                                whisper_context_default_params());
                                 if (g_context != nullptr) {
                                     return true;
                                 } else {
                                     return false;
                                 }
                             }

                             return false;
                         }));

    emscripten::function("free", emscripten::optional_override([]() {
                             if (g_context) {
                                 whisper_free(g_context);
                                 g_context = nullptr;
                             }
                         }));

    emscripten::function(
        "full_default",
        emscripten::optional_override([](const emscripten::val& audio, const std::string& lang, bool translate) {
            if (g_context == nullptr) {
                return -1;
            }

            struct whisper_full_params params =
                whisper_full_default_params(whisper_sampling_strategy::STELNET_SAMPLING_GREEDY);

            params.print_realtime = true;
            params.print_progress = false;
            params.print_timestamps = true;
            params.print_special = false;
            params.translate = translate;
            params.language = whisper_is_multilingual(g_context) ? lang.c_str() : "en";
            params.n_threads = std::min(8, (int)std::thread::hardware_concurrency());
            params.offset_ms = 0;

            std::vector<float> pcmf32;
            const int n = audio["length"].as<int>();

            emscripten::val heap = emscripten::val::module_property("HEAPU8");
            emscripten::val memory = heap["buffer"];

            pcmf32.resize(n);

            emscripten::val memoryView =
                audio["constructor"].new_(memory, reinterpret_cast<uintptr_t>(pcmf32.data()), n);
            memoryView.call<void>("set", audio);

            // print system information
            {
                printf("\n");
                printf("system_info: n_threads = %d / %d | %s\n", params.n_threads, std::thread::hardware_concurrency(),
                       whisper_print_system_info());

                printf("\n");
                printf("%s: processing %d samples, %.1f sec, %d threads, %d processors, lang = %s, task = %s ...\n",
                       __func__, int(pcmf32.size()), float(pcmf32.size()) / STELNET_SAMPLE_RATE, params.n_threads, 1,
                       params.language, params.translate ? "translate" : "transcribe");

                printf("\n");
            }

            // run whisper
            {
                whisper_reset_timings(g_context);
                whisper_full(g_context, params, pcmf32.data(), pcmf32.size());
                whisper_print_timings(g_context);
            }

            return 0;
        }));

    // -------------------------------------------------------------------
    // Backend-agnostic ASR session surface (parakeet, canary, …) over the
    // stelnet_session C-ABI — the WASM analogue of the native bindings'
    // Session. Unlike init()/full_default() above (whisper-only), this reaches
    // every ASR backend plus the session post-processors (punctuation,
    // punc-model, beam, hotwords, translate). Mirrors the tts* surface.
    // -------------------------------------------------------------------
    emscripten::function("asrOpen", emscripten::optional_override(
                                        [](const std::string& model_path, const std::string& backend, int n_threads) {
                                            if (g_asr_session) {
                                                stelnet_session_close(g_asr_session);
                                                g_asr_session = nullptr;
                                            }
                                            g_asr_session =
                                                backend.empty()
                                                    ? stelnet_session_open(model_path.c_str(), n_threads)
                                                    : stelnet_session_open_explicit(model_path.c_str(),
                                                                                     backend.c_str(), n_threads);
                                            return g_asr_session != nullptr;
                                        }));

    emscripten::function("asrClose", emscripten::optional_override([]() {
                             if (g_asr_session) {
                                 stelnet_session_close(g_asr_session);
                                 g_asr_session = nullptr;
                             }
                         }));

    emscripten::function("asrSetSourceLanguage", emscripten::optional_override([](const std::string& lang) {
                             return g_asr_session ? stelnet_session_set_source_language(g_asr_session, lang.c_str())
                                                  : -1;
                         }));
    emscripten::function("asrSetTargetLanguage", emscripten::optional_override([](const std::string& lang) {
                             return g_asr_session ? stelnet_session_set_target_language(g_asr_session, lang.c_str())
                                                  : -1;
                         }));
    emscripten::function("asrSetTranslate", emscripten::optional_override([](bool enable) {
                             return g_asr_session ? stelnet_session_set_translate(g_asr_session, enable ? 1 : 0) : -1;
                         }));
    emscripten::function("asrSetPunctuation", emscripten::optional_override([](bool enable) {
                             return g_asr_session ? stelnet_session_set_punctuation(g_asr_session, enable ? 1 : 0)
                                                  : -1;
                         }));
    emscripten::function("asrSetPuncModel", emscripten::optional_override([](const std::string& m) {
                             return g_asr_session ? stelnet_session_set_punc_model(g_asr_session, m.c_str()) : -1;
                         }));
    emscripten::function("asrSetBeamSize", emscripten::optional_override([](int n) {
                             return g_asr_session ? stelnet_session_set_beam_size(g_asr_session, n) : -1;
                         }));
    emscripten::function("asrSetHotwords", emscripten::optional_override([](const std::string& w, double boost) {
                             return g_asr_session
                                        ? stelnet_session_set_hotwords(g_asr_session, w.c_str(), (float)boost)
                                        : -1;
                         }));
    emscripten::function("asrSetAsk", emscripten::optional_override([](const std::string& prompt) {
                             return g_asr_session ? stelnet_session_set_ask(g_asr_session, prompt.c_str()) : -1;
                         }));
    emscripten::function("asrSetTemperature", emscripten::optional_override([](double temp, double seed) {
                             return g_asr_session ? stelnet_session_set_temperature(g_asr_session, (float)temp,
                                                                                     (unsigned long long)seed)
                                                  : -1;
                         }));

    // Transcribe a Float32Array of 16 kHz mono PCM. Returns an array of
    // { t0, t1, text } segments (timestamps in centiseconds). Empty on failure.
    emscripten::function(
        "asrTranscribe",
        emscripten::optional_override([](const emscripten::val& audio, const std::string& lang) -> emscripten::val {
            emscripten::val out = emscripten::val::array();
            if (!g_asr_session)
                return out;
            const int n = audio["length"].as<int>();
            std::vector<float> pcmf32(n);
            emscripten::val heap = emscripten::val::module_property("HEAPU8");
            emscripten::val memory = heap["buffer"];
            emscripten::val view =
                audio["constructor"].new_(memory, reinterpret_cast<uintptr_t>(pcmf32.data()), n);
            view.call<void>("set", audio);

            stelnet_session_result* r = stelnet_session_transcribe_lang(
                g_asr_session, pcmf32.data(), n, lang.empty() ? nullptr : lang.c_str());
            if (!r)
                return out;
            const int ns = stelnet_session_result_n_segments(r);
            for (int i = 0; i < ns; i++) {
                emscripten::val seg = emscripten::val::object();
                seg.set("t0", (double)stelnet_session_result_segment_t0(r, i));
                seg.set("t1", (double)stelnet_session_result_segment_t1(r, i));
                const char* text = stelnet_session_result_segment_text(r, i);
                seg.set("text", std::string(text ? text : ""));
                out.call<void>("push", seg);
            }
            stelnet_session_result_free(r);
            return out;
        }));

    // -------------------------------------------------------------------
    // TTS surface (kokoro / vibevoice / qwen3-tts) + kokoro per-language
    // routing (PLAN #56 opt 2b).
    // -------------------------------------------------------------------

    emscripten::function("ttsOpen", emscripten::optional_override([](const std::string& model_path,
                                                                     int n_threads) {
                             if (g_tts_session != nullptr) {
                                 stelnet_session_close(g_tts_session);
                                 g_tts_session = nullptr;
                             }
                             g_tts_session = stelnet_session_open(model_path.c_str(),
                                                                   n_threads <= 0 ? 1 : n_threads);
                             return g_tts_session != nullptr;
                         }));

    emscripten::function("ttsClose", emscripten::optional_override([]() {
                             if (g_tts_session) {
                                 stelnet_session_close(g_tts_session);
                                 g_tts_session = nullptr;
                             }
                         }));

    emscripten::function("ttsSetCodecPath", emscripten::optional_override([](const std::string& path) {
                             return g_tts_session ? stelnet_session_set_codec_path(g_tts_session,
                                                                                    path.c_str())
                                                  : -1;
                         }));

    // Drop the kokoro per-session phoneme cache. (PLAN #56 #5)
    emscripten::function("ttsClearPhonemeCache", emscripten::optional_override([]() {
                             return g_tts_session ? stelnet_session_kokoro_clear_phoneme_cache(g_tts_session)
                                                  : -1;
                         }));

    emscripten::function("ttsSetVoice", emscripten::optional_override([](const std::string& path,
                                                                         const std::string& ref_text) {
                             if (!g_tts_session) return -1;
                             const char* rt = ref_text.empty() ? nullptr : ref_text.c_str();
                             return stelnet_session_set_voice(g_tts_session, path.c_str(), rt);
                         }));

    // Orpheus preset speakers — set by NAME, not by file path.
    emscripten::function("ttsSetSpeakerName",
                         emscripten::optional_override([](const std::string& name) {
                             if (!g_tts_session) return -1;
                             return stelnet_session_set_speaker_name(g_tts_session, name.c_str());
                         }));

    // Returns the list of preset speaker names for the active backend
    // (orpheus today). Empty array if the backend has no preset speakers.
    emscripten::function("ttsSpeakers",
                         emscripten::optional_override([]() -> emscripten::val {
                             emscripten::val out = emscripten::val::array();
                             if (!g_tts_session) return out;
                             int n = stelnet_session_n_speakers(g_tts_session);
                             for (int i = 0; i < n; i++) {
                                 const char* name = stelnet_session_get_speaker_name(g_tts_session, i);
                                 if (name) out.call<void>("push", std::string(name));
                             }
                             return out;
                         }));

    // qwen3-tts VoiceDesign — natural-language voice description.
    emscripten::function("ttsSetInstruct",
                         emscripten::optional_override([](const std::string& instruct) {
                             if (!g_tts_session) return -1;
                             return stelnet_session_set_instruct(g_tts_session, instruct.c_str());
                         }));

    // qwen3-tts variant detection (returns false also when the active
    // backend isn't qwen3-tts).
    emscripten::function("ttsIsCustomVoice",
                         emscripten::optional_override([]() -> bool {
                             return g_tts_session && stelnet_session_is_custom_voice(g_tts_session) != 0;
                         }));

    emscripten::function("ttsIsVoiceDesign",
                         emscripten::optional_override([]() -> bool {
                             return g_tts_session && stelnet_session_is_voice_design(g_tts_session) != 0;
                         }));

    // Returns a Float32Array of 24 kHz mono PCM. Empty array on failure.
    emscripten::function("ttsSynthesize",
                         emscripten::optional_override([](const std::string& text) -> emscripten::val {
                             if (!g_tts_session) return emscripten::val::array();
                             int n = 0;
                             float* pcm = stelnet_session_synthesize(g_tts_session, text.c_str(), &n);
                             if (!pcm || n <= 0) {
                                 if (pcm) stelnet_pcm_free(pcm);
                                 return emscripten::val::array();
                             }
                             emscripten::val out = emscripten::val::global("Float32Array").new_(n);
                             emscripten::val memoryView = emscripten::val(emscripten::typed_memory_view(n, pcm));
                             out.call<void>("set", memoryView);
                             stelnet_pcm_free(pcm);
                             return out;
                         }));

    // Mirrors python stelnet.kokoro_resolve_for_lang() — returns
    // {modelPath, voicePath, voiceName, backboneSwapped}.
    emscripten::function("kokoroResolveForLang",
                         emscripten::optional_override([](const std::string& model_path,
                                                          const std::string& lang) -> emscripten::val {
                             char out_model[1024]  = {0};
                             char out_voice[1024]  = {0};
                             char out_picked[64]   = {0};

                             int rc = stelnet_kokoro_resolve_model_for_lang_abi(
                                 model_path.c_str(), lang.c_str(), out_model, sizeof(out_model));
                             bool swapped = (rc == 0);
                             std::string resolved = (out_model[0] != 0) ? std::string(out_model) : model_path;

                             std::string vp, vn;
                             rc = stelnet_kokoro_resolve_fallback_voice_abi(
                                 model_path.c_str(), lang.c_str(),
                                 out_voice, sizeof(out_voice),
                                 out_picked, sizeof(out_picked));
                             if (rc == 0) {
                                 vp = out_voice;
                                 vn = out_picked;
                             }

                             emscripten::val r = emscripten::val::object();
                             r.set("modelPath",       resolved);
                             r.set("voicePath",       vp);
                             r.set("voiceName",       vn);
                             r.set("backboneSwapped", swapped);
                             return r;
                         }));

    // -------------------------------------------------------------------
    // Full C-ABI parity wrappers (PLAN #59)
    // -------------------------------------------------------------------

    // --- Session setters ---
    emscripten::function("sessionSetSourceLanguage", emscripten::optional_override([](const std::string& lang) {
        return g_tts_session ? stelnet_session_set_source_language(g_tts_session, lang.c_str()) : -1;
    }));
    emscripten::function("sessionSetTargetLanguage", emscripten::optional_override([](const std::string& lang) {
        return g_tts_session ? stelnet_session_set_target_language(g_tts_session, lang.c_str()) : -1;
    }));
    emscripten::function("sessionSetPunctuation", emscripten::optional_override([](bool enable) {
        return g_tts_session ? stelnet_session_set_punctuation(g_tts_session, enable ? 1 : 0) : -1;
    }));
    emscripten::function("sessionSetTranslate", emscripten::optional_override([](bool enable) {
        return g_tts_session ? stelnet_session_set_translate(g_tts_session, enable ? 1 : 0) : -1;
    }));
    emscripten::function("sessionSetTemperature", emscripten::optional_override([](float temp, int seed) {
        return g_tts_session ? stelnet_session_set_temperature(g_tts_session, temp, (unsigned long long)seed) : -1;
    }));
    emscripten::function("sessionSetTtsSeed", emscripten::optional_override([](int seed) {
        return g_tts_session ? stelnet_session_set_tts_seed(g_tts_session, (unsigned long long)seed) : -1;
    }));
    emscripten::function("sessionSetTtsSteps", emscripten::optional_override([](int steps) {
        return g_tts_session ? stelnet_session_set_tts_steps(g_tts_session, steps) : -1;
    }));
    emscripten::function("sessionSetMaxNewTokens", emscripten::optional_override([](int n) {
        return g_tts_session ? stelnet_session_set_max_new_tokens(g_tts_session, n) : -1;
    }));
    emscripten::function("sessionSetFrequencyPenalty", emscripten::optional_override([](float p) {
        return g_tts_session ? stelnet_session_set_frequency_penalty(g_tts_session, p) : -1;
    }));
    emscripten::function("sessionSetTopP", emscripten::optional_override([](float p) {
        return g_tts_session ? stelnet_session_set_top_p(g_tts_session, p) : -1;
    }));
    emscripten::function("sessionSetMinP", emscripten::optional_override([](float p) {
        return g_tts_session ? stelnet_session_set_min_p(g_tts_session, p) : -1;
    }));
    emscripten::function("sessionSetRepetitionPenalty", emscripten::optional_override([](float r) {
        return g_tts_session ? stelnet_session_set_repetition_penalty(g_tts_session, r) : -1;
    }));
    emscripten::function("sessionSetCfgWeight", emscripten::optional_override([](float w) {
        return g_tts_session ? stelnet_session_set_cfg_weight(g_tts_session, w) : -1;
    }));
    emscripten::function("sessionSetExaggeration", emscripten::optional_override([](float e) {
        return g_tts_session ? stelnet_session_set_exaggeration(g_tts_session, e) : -1;
    }));
    emscripten::function("sessionSetMaxSpeechTokens", emscripten::optional_override([](int n) {
        return g_tts_session ? stelnet_session_set_max_speech_tokens(g_tts_session, n) : -1;
    }));
    emscripten::function("sessionSetLengthScale", emscripten::optional_override([](float s) {
        return g_tts_session ? stelnet_session_set_length_scale(g_tts_session, s) : -1;
    }));
    emscripten::function("sessionSetBestOf", emscripten::optional_override([](int n) {
        return g_tts_session ? stelnet_session_set_best_of(g_tts_session, n) : -1;
    }));
    emscripten::function("sessionSetBeamSize", emscripten::optional_override([](int n) {
        return g_tts_session ? stelnet_session_set_beam_size(g_tts_session, n) : -1;
    }));
    emscripten::function("sessionSetGrammarText", emscripten::optional_override(
        [](const std::string& text, const std::string& root, float penalty) {
            return g_tts_session ? stelnet_session_set_grammar_text(
                g_tts_session, text.c_str(), root.c_str(), penalty) : -1;
        }));
    emscripten::function("sessionSetFallbackThresholds", emscripten::optional_override(
        [](float entropy, float logprob, float noSpeech, float tempInc) {
            return g_tts_session ? stelnet_session_set_fallback_thresholds(
                g_tts_session, entropy, logprob, noSpeech, tempInc) : -1;
        }));
    emscripten::function("sessionSetAltN", emscripten::optional_override([](int n) {
        return g_tts_session ? stelnet_session_set_alt_n(g_tts_session, n) : -1;
    }));
    emscripten::function("sessionSetWhisperDecodeExtras", emscripten::optional_override(
        [](bool suppressNst, const std::string& regex, bool carryPrompt) {
            return g_tts_session ? stelnet_session_set_whisper_decode_extras(
                g_tts_session, suppressNst ? 1 : 0, regex.c_str(), carryPrompt ? 1 : 0) : -1;
        }));
    emscripten::function("sessionSetAsk", emscripten::optional_override([](const std::string& prompt) {
        return g_tts_session ? stelnet_session_set_ask(g_tts_session, prompt.c_str()) : -1;
    }));

    // --- Session ASR ---
    emscripten::function("sessionTranscribe", emscripten::optional_override(
        [](const emscripten::val& audio, const std::string& lang) -> emscripten::val {
            if (!g_tts_session) return emscripten::val::array();
            const int n = audio["length"].as<int>();
            std::vector<float> pcm(n);
            emscripten::val heap = emscripten::val::module_property("HEAPU8");
            emscripten::val memory = heap["buffer"];
            emscripten::val mv = audio["constructor"].new_(memory, reinterpret_cast<uintptr_t>(pcm.data()), n);
            mv.call<void>("set", audio);

            stelnet_session_result* res;
            if (!lang.empty()) {
                res = stelnet_session_transcribe_lang(g_tts_session, pcm.data(), n, lang.c_str());
            } else {
                res = stelnet_session_transcribe(g_tts_session, pcm.data(), n);
            }
            if (!res) return emscripten::val::array();

            int ns = stelnet_session_result_n_segments(res);
            emscripten::val out = emscripten::val::array();
            for (int i = 0; i < ns; i++) {
                emscripten::val seg = emscripten::val::object();
                const char* t = stelnet_session_result_segment_text(res, i);
                seg.set("text", std::string(t ? t : ""));
                seg.set("t0", stelnet_session_result_segment_t0(res, i) / 100.0);
                seg.set("t1", stelnet_session_result_segment_t1(res, i) / 100.0);
                int nw = stelnet_session_result_n_words(res, i);
                emscripten::val words = emscripten::val::array();
                for (int j = 0; j < nw; j++) {
                    emscripten::val w = emscripten::val::object();
                    const char* wt = stelnet_session_result_word_text(res, i, j);
                    w.set("text", std::string(wt ? wt : ""));
                    w.set("t0", stelnet_session_result_word_t0(res, i, j) / 100.0);
                    w.set("t1", stelnet_session_result_word_t1(res, i, j) / 100.0);
                    w.set("p", (double)stelnet_session_result_word_p(res, i, j));
                    words.call<void>("push", w);
                }
                seg.set("words", words);
                out.call<void>("push", seg);
            }
            stelnet_session_result_free(res);
            return out;
        }));

    // --- Session translate ---
    emscripten::function("sessionTranslateText", emscripten::optional_override(
        [](const std::string& text, const std::string& src, const std::string& tgt, int maxTokens) -> std::string {
            if (!g_tts_session) return "";
            char* res = stelnet_session_translate_text(g_tts_session, text.c_str(),
                                                        src.c_str(), tgt.c_str(), maxTokens);
            if (!res) return "";
            std::string out(res);
            stelnet_session_translate_text_free(res);
            return out;
        }));

    // --- Available backends ---
    emscripten::function("availableBackends", emscripten::optional_override([]() -> std::string {
        char buf[1024] = {0};
        stelnet_session_available_backends(buf, sizeof(buf));
        return std::string(buf);
    }));

    // --- Detect backend from GGUF ---
    emscripten::function("detectBackendFromGguf", emscripten::optional_override(
        [](const std::string& path) -> std::string {
            char out[128] = {0};
            int rc = stelnet_detect_backend_from_gguf(path.c_str(), out, sizeof(out));
            return rc == 0 ? std::string(out) : "";
        }));

    // --- LCS dedup ---
    emscripten::function("lcsDedup", emscripten::optional_override(
        [](const emscripten::val& prev, const emscripten::val& curr, int minLen) -> int {
            int pn = prev["length"].as<int>();
            int cn = curr["length"].as<int>();
            std::vector<int> pvec(pn), cvec(cn);
            for (int i = 0; i < pn; i++) pvec[i] = prev[i].as<int>();
            for (int i = 0; i < cn; i++) cvec[i] = curr[i].as<int>();
            return stelnet_lcs_dedup_prefix_count(pvec.data(), pn, cvec.data(), cn, minLen);
        }));

    // --- Kokoro lang helpers ---
    emscripten::function("kokoroLangIsGerman", emscripten::optional_override(
        [](const std::string& lang) -> bool {
            return stelnet_kokoro_lang_is_german_abi(lang.c_str()) != 0;
        }));
    emscripten::function("kokoroLangHasNativeVoice", emscripten::optional_override(
        [](const std::string& lang) -> bool {
            return stelnet_kokoro_lang_has_native_voice_abi(lang.c_str()) != 0;
        }));

    // --- Text-LID ---
    emscripten::function("textDetectLanguage", emscripten::optional_override(
        [](const std::string& text, const std::string& modelPath, int nThreads) -> emscripten::val {
            char label[64] = {0};
            float conf = 0.0f;
            int rc = stelnet_text_detect_language(text.c_str(), modelPath.c_str(),
                                                   nThreads, label, 64, &conf);
            emscripten::val r = emscripten::val::object();
            r.set("rc", rc);
            r.set("lang", std::string(label));
            r.set("confidence", (double)conf);
            return r;
        }));

    // --- Registry ---
    emscripten::function("registryListBackends", emscripten::optional_override([]() -> std::string {
        char buf[8192] = {0};
        stelnet_registry_list_backends_abi(buf, sizeof(buf));
        return std::string(buf);
    }));

    // --- Punctuation ---
    emscripten::function("puncInit", emscripten::optional_override(
        [](const std::string& modelPath) -> int {
            void* h = stelnet_punc_init(modelPath.c_str());
            return h ? (int)(uintptr_t)h : 0;
        }));
    emscripten::function("puncProcess", emscripten::optional_override(
        [](int handle, const std::string& text) -> std::string {
            if (!handle) return text;
            const char* r = stelnet_punc_process((void*)(uintptr_t)handle, text.c_str());
            if (!r) return text;
            std::string out(r);
            stelnet_punc_free_text(r);
            return out;
        }));
    emscripten::function("puncFree", emscripten::optional_override([](int handle) {
        if (handle) stelnet_punc_free((void*)(uintptr_t)handle);
    }));
}
