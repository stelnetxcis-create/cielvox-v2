// stelnet_backend.cpp — backend factory, auto-detection, and helpers.

#include "stelnet_backend.h"
#include "whisper_params.h"

// Forward declarations of per-backend constructors. Each is implemented in
// its own stelnet_backend_X.cpp file and compiled only if the backend's
// library is linked in.
std::unique_ptr<StelnetBackend> stelnet_make_whisper_backend();
std::unique_ptr<StelnetBackend> stelnet_make_nemotron_backend();
std::unique_ptr<StelnetBackend> stelnet_make_parakeet_backend();
std::unique_ptr<StelnetBackend> stelnet_make_canary_backend();
std::unique_ptr<StelnetBackend> stelnet_make_lfm2_audio_backend();
std::unique_ptr<StelnetBackend> stelnet_make_mini_omni2_backend();
std::unique_ptr<StelnetBackend> stelnet_make_cohere_backend();
std::unique_ptr<StelnetBackend> stelnet_make_granite_backend();
std::unique_ptr<StelnetBackend> stelnet_make_granite_nle_backend();
std::unique_ptr<StelnetBackend> stelnet_make_voxtral_backend();
std::unique_ptr<StelnetBackend> stelnet_make_voxtral4b_backend();
std::unique_ptr<StelnetBackend> stelnet_make_cielvox_asr_backend();
std::unique_ptr<StelnetBackend> stelnet_make_fastconformer_ctc_backend();
std::unique_ptr<StelnetBackend> stelnet_make_wav2vec2_backend();
std::unique_ptr<StelnetBackend> stelnet_make_vibevoice_backend();
std::unique_ptr<StelnetBackend> stelnet_make_vibevoice_tts_backend();
std::unique_ptr<StelnetBackend> stelnet_make_vibevoice_1p5b_backend();
std::unique_ptr<StelnetBackend> stelnet_make_kugelaudio_backend();
std::unique_ptr<StelnetBackend> stelnet_make_cielvox2_backend();
std::unique_ptr<StelnetBackend> stelnet_make_cielvox2_base_backend();
std::unique_ptr<StelnetBackend> stelnet_make_orpheus_backend();
std::unique_ptr<StelnetBackend> stelnet_make_chatterbox_backend();
std::unique_ptr<StelnetBackend> stelnet_make_tada_backend();
std::unique_ptr<StelnetBackend> stelnet_make_indextts_backend();
std::unique_ptr<StelnetBackend> stelnet_make_m2m100_backend();
std::unique_ptr<StelnetBackend> stelnet_make_t5_backend();
std::unique_ptr<StelnetBackend> stelnet_make_kokoro_backend();
std::unique_ptr<StelnetBackend> stelnet_make_glm_asr_backend();
std::unique_ptr<StelnetBackend> stelnet_make_kyutai_stt_backend();
std::unique_ptr<StelnetBackend> stelnet_make_firered_asr_backend();
std::unique_ptr<StelnetBackend> stelnet_make_moonshine_backend();
std::unique_ptr<StelnetBackend> stelnet_make_moonshine_streaming_backend();
std::unique_ptr<StelnetBackend> stelnet_make_gemma4_e2b_backend();
std::unique_ptr<StelnetBackend> stelnet_make_omniasr_backend();
std::unique_ptr<StelnetBackend> stelnet_make_mimo_asr_backend();
std::unique_ptr<StelnetBackend> stelnet_make_moss_audio_backend();
std::unique_ptr<StelnetBackend> stelnet_make_funasr_backend();
std::unique_ptr<StelnetBackend> stelnet_make_paraformer_backend();
std::unique_ptr<StelnetBackend> stelnet_make_sensevoice_backend();
std::unique_ptr<StelnetBackend> stelnet_make_voxcpm2_tts_backend();
std::unique_ptr<StelnetBackend> stelnet_make_cosyvoice3_tts_backend();
std::unique_ptr<StelnetBackend> stelnet_make_piper_backend();
std::unique_ptr<StelnetBackend> stelnet_make_melotts_backend();
#ifdef STELNET_HAVE_OUTETTS
std::unique_ptr<StelnetBackend> stelnet_make_outetts_backend();
#endif
std::unique_ptr<StelnetBackend> stelnet_make_zonos_backend();
std::unique_ptr<StelnetBackend> stelnet_make_f5_tts_backend();
std::unique_ptr<StelnetBackend> stelnet_make_bark_backend();
std::unique_ptr<StelnetBackend> stelnet_make_pocket_tts_backend();
std::unique_ptr<StelnetBackend> stelnet_make_speecht5_backend();
std::unique_ptr<StelnetBackend> stelnet_make_dia_backend();
std::unique_ptr<StelnetBackend> stelnet_make_parler_tts_backend();
std::unique_ptr<StelnetBackend> stelnet_make_fastpitch_backend();
// csm-tts (§135): sesame/csm-1b — Llama backbone + depth decoder + Mimi codec.
std::unique_ptr<StelnetBackend> stelnet_make_csm_tts_backend();

#include "ggml.h"
#include "gguf.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Factory
// ---------------------------------------------------------------------------

std::unique_ptr<StelnetBackend> stelnet_create_backend(const std::string& name) {
    if (name == "whisper")
        return stelnet_make_whisper_backend();
    if (name == "nemotron" || name == "nemotron-streaming" || name == "nemotron-3.5" || name == "nemotron-asr" ||
        name == "nemotron-speech-streaming")
        return stelnet_make_nemotron_backend();
    if (name == "parakeet")
        return stelnet_make_parakeet_backend();
    if (name == "canary")
        return stelnet_make_canary_backend();
    if (name == "lfm2-audio")
        return stelnet_make_lfm2_audio_backend();
    if (name == "mini-omni2" || name == "mini_omni2" || name == "miniomni2")
        return stelnet_make_mini_omni2_backend();
    if (name == "cohere")
        return stelnet_make_cohere_backend();
    if (name == "granite" || name == "granite-4.1" || name == "granite-4.1-plus")
        return stelnet_make_granite_backend();
    if (name == "granite-4.1-nar" || name == "granite-nar")
        return stelnet_make_granite_nle_backend();
    if (name == "voxtral")
        return stelnet_make_voxtral_backend();
    if (name == "voxtral4b")
        return stelnet_make_voxtral4b_backend();
    if (name == "cielvox-asr" || name == "cielvox-asr-1.7b" || name == "qwen3" || name == "qwen3-1.7b" || name == "qwen3_1.7b" || name == "qwen3_17b" || name == "mega-asr" ||
        name == "mega_asr" || name == "megaasr")
        return stelnet_make_cielvox_asr_backend();
    if (name == "fastconformer-ctc")
        return stelnet_make_fastconformer_ctc_backend();
    if (name == "wav2vec2" || name == "hubert" || name == "data2vec")
        return stelnet_make_wav2vec2_backend();
    if (name == "vibevoice")
        return stelnet_make_vibevoice_backend();
    if (name == "vibevoice-tts")
        return stelnet_make_vibevoice_tts_backend();
    if (name == "kugelaudio" || name == "kugelaudio-tts" || name == "kugelaudio-0-open")
        return stelnet_make_kugelaudio_backend();
    if (name == "vibevoice-1.5b" || name == "vibevoice-tts-1.5b" || name == "vibevoice-tts-base")
        return stelnet_make_vibevoice_1p5b_backend();
    if (name == "cielvox2" || name == "cielvox2" || name == "cielvox2" || name == "cielvox2-1.7b-stelnetvoice" ||
        name == "qwen3-tts-1.7b")
        return stelnet_make_cielvox2_base_backend();
    if (name == "cielvox2-stelnetvoicepreset" || name == "qwen3tts-customvoice" || name == "qwen3-tts-cv" ||
        name == "cielvox2-1.7b-stelnetvoicepreset" || name == "qwen3-tts-1.7b-cv" || name == "cielvox2-1.7b-stelnetvoicecreation" ||
        name == "qwen3-tts-voicedesign" || name == "qwen3-tts-vd")
        return stelnet_make_cielvox2_backend();
    if (name == "orpheus" || name == "orpheus-tts" || name == "orpheus3b" || name == "kartoffel-orpheus" ||
        name == "kartoffel_orpheus" || name == "kartoffel-orpheus-de-natural" ||
        name == "kartoffel-orpheus-de-synthetic" || name == "kartoffel-orpheus-natural" ||
        name == "kartoffel-orpheus-synthetic" || name == "lex-au-orpheus-de" || name == "lex-au-orpheus")
        return stelnet_make_orpheus_backend();
    if (name == "chatterbox" || name == "chatterbox-tts" || name == "chatterbox-base" || name == "chatterbox-turbo" ||
        name == "chatterbox_turbo" || name == "kartoffelbox" || name == "kartoffelbox-turbo" ||
        name == "kartoffelbox_turbo" || name == "lahgtna" || name == "lahgtna-chatterbox" ||
        name == "lahgtna-chatterbox-v1")
        return stelnet_make_chatterbox_backend();
    if (name == "tada" || name == "tada-tts" || name == "tada-3b" || name == "tada-3b-ml")
        return stelnet_make_tada_backend();
    if (name == "indextts" || name == "indextts-1.5" || name == "indextts1.5" || name == "index-tts")
        return stelnet_make_indextts_backend();
    if (name == "kokoro" || name == "styletts2" || name == "styletts2-ljspeech" || name == "kokoro-tts")
        return stelnet_make_kokoro_backend();
    if (name == "piper" || name == "piper-tts" || name == "piper-vits")
        return stelnet_make_piper_backend();
    if (name == "melotts" || name == "melo-tts" || name == "melo")
        return stelnet_make_melotts_backend();
#ifdef STELNET_HAVE_OUTETTS
    if (name == "outetts" || name == "outetts-tts" || name == "oute-tts" || name == "outetts-0.3-1b")
        return stelnet_make_outetts_backend();
#endif
    if (name == "f5-tts" || name == "f5_tts" || name == "f5tts" || name == "f5")
        return stelnet_make_f5_tts_backend();
    if (name == "pocket-tts" || name == "pocket_tts" || name == "pockettts" || name == "pocket")
        return stelnet_make_pocket_tts_backend();
    if (name == "fastpitch" || name == "fastpitch-tts" || name == "fastpitch_tts")
        return stelnet_make_fastpitch_backend();
    if (name == "voxcpm2-tts" || name == "voxcpm2" || name == "voxcpm" || name == "voxcpm2_tts")
        return stelnet_make_voxcpm2_tts_backend();
    if (name == "cosyvoice3" || name == "cosyvoice3-tts" || name == "cosyvoice3_tts" || name == "cv3" ||
        name == "cv3-tts")
        return stelnet_make_cosyvoice3_tts_backend();
    if (name == "m2m100" || name == "m2m-100" || name == "translate" || name == "m2m100-wmt21" || name == "wmt21" ||
        name == "m2m100-1.2b")
        return stelnet_make_m2m100_backend();
    if (name == "madlad" || name == "madlad400" || name == "madlad-400" || name == "t5" || name == "t5-translate")
        return stelnet_make_t5_backend();
    if (name == "glm-asr" || name == "glmasr" || name == "glm" || name == "glm_asr")
        return stelnet_make_glm_asr_backend();
    if (name == "kyutai-stt" || name == "kyutai" || name == "moshi-stt")
        return stelnet_make_kyutai_stt_backend();
    if (name == "firered-asr" || name == "firered")
        return stelnet_make_firered_asr_backend();
    if (name == "moonshine-streaming")
        return stelnet_make_moonshine_streaming_backend();
    if (name == "gemma4-e2b" || name == "gemma4e2b" || name == "gemma4")
        return stelnet_make_gemma4_e2b_backend();
    if (name == "moonshine" || name == "moonshine-de" || name == "moonshine-tiny-de")
        return stelnet_make_moonshine_backend();
    if (name.rfind("omniasr", 0) == 0)
        return stelnet_make_omniasr_backend();
    if (name == "mimo-asr" || name == "mimo_asr" || name == "mimoasr")
        return stelnet_make_mimo_asr_backend();
    if (name == "moss-audio" || name == "moss_audio" || name == "mossaudio")
        return stelnet_make_moss_audio_backend();
    if (name == "funasr" || name == "fun-asr" || name == "fun-asr-nano" || name == "fun-asr-mlt-nano")
        return stelnet_make_funasr_backend();
    if (name == "paraformer" || name == "paraformer-zh" || name == "paraformer-en")
        return stelnet_make_paraformer_backend();
    if (name == "sensevoice" || name == "sensevoice-small" || name == "sense-voice")
        return stelnet_make_sensevoice_backend();
    if (name == "bark" || name == "bark-tts" || name == "bark-small" || name == "bark-large")
        return stelnet_make_bark_backend();
    if (name == "speecht5" || name == "speecht5-tts" || name == "speecht5_tts")
        return stelnet_make_speecht5_backend();
    if (name == "dia" || name == "dia-tts" || name == "dia-1.6b" || name == "dia_tts")
        return stelnet_make_dia_backend();
    if (name == "parler-tts" || name == "parler_tts" || name == "parler" || name == "parlertts")
        return stelnet_make_parler_tts_backend();
    if (name == "zonos" || name == "zonos-tts" || name == "zonos_tts")
        return stelnet_make_zonos_backend();
    if (name == "csm" || name == "csm-tts" || name == "csm_tts" || name == "sesame" || name == "sesame-csm")
        return stelnet_make_csm_tts_backend();

    fprintf(stderr, "stelnet: error: unknown backend '%s'\n", name.c_str());
    return nullptr;
}

std::vector<std::string> stelnet_list_backends() {
    return {
        "whisper",
        "nemotron",
        "parakeet",
        "canary",
        "lfm2-audio",
        "mini-omni2",
        "cohere",
        "granite",
        "granite-4.1",
        "granite-4.1-plus",
        "granite-4.1-nar",
        "voxtral",
        "voxtral4b",
        "cielvox-asr",
        "cielvox-asr-1.7b",
        "mega-asr",
        "fastconformer-ctc",
        "wav2vec2",
        "hubert",
        "data2vec",
        "vibevoice",
        "kugelaudio",
        "cielvox2",
        "vibevoice-1.5b",
        "cielvox2-stelnetvoicepreset",
        "cielvox2-1.7b-stelnetvoice",
        "cielvox2-1.7b-stelnetvoicepreset",
        "cielvox2-1.7b-stelnetvoicecreation",
        "orpheus",
        "lex-au-orpheus-de",
        "kartoffel-orpheus-de-natural",
        "kartoffel-orpheus-de-synthetic",
        "chatterbox",
        "chatterbox-turbo",
        "kartoffelbox-turbo",
        "lahgtna-chatterbox",
        "indextts",
        "f5-tts",
        "pocket-tts",
        "fastpitch",
        "kokoro",
        "melotts",
        "piper",
        "outetts",
        "voxcpm2-tts",
        "cosyvoice3-tts",
        "m2m100",
        "m2m100-wmt21",
        "madlad",
        "glm-asr",
        "kyutai-stt",
        "firered-asr",
        "moonshine",
        "moonshine-streaming",
        "gemma4-e2b",
        "omniasr",
        "omniasr-300m",
        "omniasr-llm",
        "omniasr-llm-1b",
        "mimo-asr",
        "moss-audio",
        "funasr",
        "fun-asr-mlt-nano",
        "paraformer",
        "sensevoice",
        "bark",
        "bark-tts",
        "speecht5",
        "dia",
        "dia-tts",
        "parler-tts",
        "zonos",
        "zonos-tts",
        "csm",
        "csm-tts",
        "sesame",
    };
}

// ---------------------------------------------------------------------------
// Capability matrix for --list-backends
// ---------------------------------------------------------------------------

struct feature_col {
    const char* label;
    uint32_t flag;
};

static constexpr feature_col kFeatures[] = {
    {"ts-native", CAP_TIMESTAMPS_NATIVE},
    {"ts-ctc", CAP_TIMESTAMPS_CTC},
    {"word-ts", CAP_WORD_TIMESTAMPS},
    {"tok-conf", CAP_TOKEN_CONFIDENCE},
    {"lang-detect", CAP_LANGUAGE_DETECT},
    {"translate", CAP_TRANSLATE},
    {"diarize", CAP_DIARIZE},
    {"grammar", CAP_GRAMMAR},
    {"temperature", CAP_TEMPERATURE},
    {"beam", CAP_BEAM_SEARCH},
    {"flash", CAP_FLASH_ATTN},
    {"punctuation", CAP_PUNCTUATION_TOGGLE},
    {"punctuation-native", CAP_PUNCTUATION_NATIVE},
    {"src/tgt lang", CAP_SRC_TGT_LANGUAGE},
    {"auto-dl", CAP_AUTO_DOWNLOAD},
    {"tts", CAP_TTS},
    {"s2s", CAP_S2S},
    {"voice-clone", CAP_VOICE_CLONING},
    {"streaming", CAP_STREAMING},
};

void stelnet_print_backend_matrix() {
    const auto backends = stelnet_list_backends();

    // Column widths
    size_t name_w = 8;
    for (const auto& b : backends)
        if (b.size() > name_w)
            name_w = b.size();

    // Header
    printf("stelnet backends (%zu):\n\n", backends.size());
    printf("  %-*s", (int)name_w, "backend");
    for (const auto& f : kFeatures)
        printf(" %-12s", f.label);
    printf("\n  ");
    for (size_t i = 0; i < name_w; i++)
        printf("-");
    for (size_t i = 0; i < sizeof(kFeatures) / sizeof(kFeatures[0]); i++) {
        printf(" ------------");
    }
    printf("\n");

    // Each row: instantiate the backend just to read its capability bitmask.
    for (const auto& name : backends) {
        uint32_t caps = 0;
        auto be = stelnet_create_backend(name);
        if (be)
            caps = be->capabilities();
        // backend destroyed when unique_ptr goes out of scope
        printf("  %-*s", (int)name_w, name.c_str());
        for (const auto& f : kFeatures) {
            printf(" %-12s", (caps & f.flag) ? "   Y" : "    -");
        }
        printf("\n");
    }
    printf("\nUse --backend NAME to force a specific backend. When omitted, the\n");
    printf("backend is auto-detected from GGUF metadata or the filename.\n");
    printf("\n");
    printf("Language detection: backends that don't advertise lang-detect\n");
    printf("natively (cohere, canary, granite, voxtral, voxtral4b) can still\n");
    printf("accept `-l auto` via the LID pre-step. Pick the provider with\n");
    printf("--lid-backend whisper|silero (whisper-tiny is the default).\n");
}

// Stable cap-name slugs for JSON output. These map to the kFeatures
// table above but use long, kebab-cased names that are nicer for
// machine consumers (test-all-backends.py, etc.) than the table
// labels which are tuned for terminal-column-width.
struct cap_slug {
    const char* slug;
    uint32_t flag;
};
static constexpr cap_slug kCapSlugs[] = {
    {"timestamps-native", CAP_TIMESTAMPS_NATIVE},
    {"timestamps-ctc", CAP_TIMESTAMPS_CTC},
    {"word-timestamps", CAP_WORD_TIMESTAMPS},
    {"token-confidence", CAP_TOKEN_CONFIDENCE},
    {"language-detect", CAP_LANGUAGE_DETECT},
    {"translate", CAP_TRANSLATE},
    {"diarize", CAP_DIARIZE},
    {"grammar", CAP_GRAMMAR},
    {"temperature", CAP_TEMPERATURE},
    {"beam-search", CAP_BEAM_SEARCH},
    {"flash-attn", CAP_FLASH_ATTN},
    {"punctuation-toggle", CAP_PUNCTUATION_TOGGLE},
    {"punctuation-native", CAP_PUNCTUATION_NATIVE},
    {"src-tgt-language", CAP_SRC_TGT_LANGUAGE},
    {"auto-download", CAP_AUTO_DOWNLOAD},
    {"parallel-processors", CAP_PARALLEL_PROCESSORS},
    {"vad-internal", CAP_VAD_INTERNAL},
    {"tts", CAP_TTS},
    {"s2s", CAP_S2S},
    {"voice-cloning", CAP_VOICE_CLONING},
    {"streaming", CAP_STREAMING},
};

void stelnet_print_backend_matrix_json() {
    const auto backends = stelnet_list_backends();

    printf("{\n  \"backends\": [\n");
    bool first_backend = true;
    for (const auto& name : backends) {
        uint32_t caps = 0;
        auto be = stelnet_create_backend(name);
        if (be)
            caps = be->capabilities();
        // backend destroyed when unique_ptr goes out of scope

        if (!first_backend)
            printf(",\n");
        first_backend = false;
        printf("    {\n");
        printf("      \"name\": \"%s\",\n", name.c_str());
        printf("      \"caps_bitmask\": %u,\n", caps);
        printf("      \"caps\": [");
        bool first_cap = true;
        for (const auto& c : kCapSlugs) {
            if (caps & c.flag) {
                if (!first_cap)
                    printf(", ");
                first_cap = false;
                printf("\"%s\"", c.slug);
            }
        }
        printf("]\n");
        printf("    }");
    }
    printf("\n  ]\n}\n");
}

// ---------------------------------------------------------------------------
// GGUF auto-detection
// ---------------------------------------------------------------------------

// Read the "general.architecture" key from a GGUF file and map it to a
// backend name. Uses gguf_init_from_file() — which lives in ggml — so this
// is cheap: only the metadata is parsed, not the weight tensors.
//
// Mappings are based on the value that each model's converter writes into
// the GGUF file. When a converter doesn't write this key we fall back to
// filename heuristics.
std::string stelnet_detect_backend_from_gguf(const std::string& model_path) {
    if (model_path.empty())
        return "";

    // ---- Pass 1: filename heuristics ----
    //
    // Try filename matching first. This avoids two problems:
    //   1. Whisper's legacy ggml-*.bin files are not GGUF; calling
    //      gguf_init_from_file() on them prints a confusing stderr warning.
    //   2. It's a fast path that covers nearly every real-world case
    //      (users consistently name their models after the architecture).
    // Extract filename (after last / or \) for matching — avoid false
    // positives from directory names (e.g. "test_cohere/" matching "cohere").
    std::string fname = model_path;
    auto sep = fname.find_last_of("/\\");
    if (sep != std::string::npos)
        fname = fname.substr(sep + 1);
    auto contains_ci = [&](const char* needle) {
        std::string lo;
        lo.reserve(fname.size());
        for (char c : fname)
            lo += (char)std::tolower((unsigned char)c);
        return lo.find(needle) != std::string::npos;
    };

    if (contains_ci("voxtral") && contains_ci("4b"))
        return "voxtral4b";
    if (contains_ci("voxtral"))
        return "voxtral";
    // Distinguish parakeet-CTC standalones (parakeet-ctc-0.6b /
    // parakeet-ctc-1.1b — same FastConformer encoder + CTC head as the
    // stt_en_fastconformer_ctc family) from parakeet-TDT (transducer)
    // and the parakeet-tdt_ctc-*-ja hybrid, which are routed via the
    // `parakeet` backend's TDT path. The "tdt" guard keeps the JA
    // hybrid (parakeet-tdt_ctc-0.6b-ja) on the parakeet route even
    // though its filename also contains "ctc".
    if (contains_ci("parakeet") && contains_ci("ctc") && !contains_ci("tdt"))
        return "fastconformer-ctc";
    if (contains_ci("parakeet"))
        return "parakeet";
    // Check "fastconformer-ctc" / "stt_en_fc_ctc" style filenames before
    // the broader "canary" match so users who drop a NeMo standalone
    // model next to a canary aligner pick the right backend.
    if (contains_ci("fastconformer") && contains_ci("ctc"))
        return "fastconformer-ctc";
    if (contains_ci("omniasr"))
        return "omniasr";
    if (contains_ci("wav2vec2"))
        return "wav2vec2";
    if (contains_ci("hubert"))
        return "wav2vec2";
    if (contains_ci("data2vec"))
        return "wav2vec2";
    if (contains_ci("vibevoice"))
        return "vibevoice";
    if (contains_ci("kugelaudio"))
        return "kugelaudio";
    if (contains_ci("fireredpunc"))
        return "fireredpunc";
    if (contains_ci("canary"))
        return "canary";
    if (contains_ci("lfm2-audio") || contains_ci("lfm2_audio"))
        return "lfm2-audio";
    if (contains_ci("mini-omni2") || contains_ci("mini_omni2") || contains_ci("miniomni2"))
        return "mini-omni2";
    if (contains_ci("cohere"))
        return "cohere";
    if (contains_ci("mega-asr") || contains_ci("mega_asr") || contains_ci("megaasr"))
        return "mega-asr";
    if (contains_ci("cielvox-asr") || (contains_ci("qwen3") && contains_ci("asr")))
        return "cielvox-asr";
    if (contains_ci("cielvox-asr") && contains_ci("tts"))
        return "cielvox2";
    if (contains_ci("orpheus") || contains_ci("kartoffel-orpheus") || contains_ci("kartoffel_orpheus"))
        return "orpheus";
    if (contains_ci("outetts") || contains_ci("oute-tts") || contains_ci("oute_tts"))
        return "outetts";
    if (contains_ci("indextts"))
        return "indextts";
    if (contains_ci("f5-tts") || contains_ci("f5tts") || contains_ci("F5TTS"))
        return "f5-tts";
    if (contains_ci("pocket-tts") || contains_ci("pocket_tts") || contains_ci("pockettts"))
        return "pocket-tts";
    if (contains_ci("fastpitch"))
        return "fastpitch";
    if (contains_ci("melotts") || contains_ci("melo-tts") || contains_ci("melo_tts"))
        return "melotts";
    if (contains_ci("piper") && !contains_ci("piper-phonemize"))
        return "piper";
    if (contains_ci("chatterbox") || contains_ci("kartoffelbox") || contains_ci("lahgtna"))
        return "chatterbox";
    if (contains_ci("tada"))
        return "tada";
    if (contains_ci("m2m100") || (contains_ci("m2m") && contains_ci("100")) || contains_ci("wmt21"))
        return "m2m100";
    if (contains_ci("madlad"))
        return "madlad";
    if (contains_ci("kokoro"))
        return "kokoro";
    if (contains_ci("styletts") && contains_ci("ljspeech"))
        return "kokoro";
    if (contains_ci("voxcpm2") || contains_ci("voxcpm"))
        return "voxcpm2-tts";
    if (contains_ci("cosyvoice3") || contains_ci("cosyvoice-3") || contains_ci("cv3"))
        return "cosyvoice3-tts";
    if (contains_ci("granite") && contains_ci("nar"))
        return "granite-4.1-nar";
    if (contains_ci("granite") && contains_ci("speech"))
        return "granite";
    if (contains_ci("glm") && contains_ci("asr"))
        return "glm-asr";
    if (contains_ci("kyutai") && contains_ci("stt"))
        return "kyutai-stt";
    if (contains_ci("moshi") && contains_ci("stt"))
        return "kyutai-stt";
    if (contains_ci("firered") && (contains_ci("asr") || contains_ci("lid")))
        return "firered-asr";
    if (contains_ci("gemma") && (contains_ci("e2b") || contains_ci("4-e2b")))
        return "gemma4-e2b";
    if (contains_ci("moonshine") && contains_ci("streaming"))
        return "moonshine-streaming";
    if (contains_ci("moonshine"))
        return "moonshine";
    if (contains_ci("fun-asr") || contains_ci("funasr") || contains_ci("fun_asr"))
        return "funasr";
    if (contains_ci("sensevoice") || contains_ci("sense-voice") || contains_ci("sense_voice"))
        return "sensevoice";
    if (contains_ci("speecht5"))
        return "speecht5";
    if (contains_ci("bark"))
        return "bark";
    if (contains_ci("dia") && contains_ci("1.6b"))
        return "dia";
    if (contains_ci("dia-tts") || contains_ci("dia_tts"))
        return "dia";
    if (contains_ci("csm") || contains_ci("sesame"))
        return "csm";
    if (contains_ci("parler") && contains_ci("tts"))
        return "parler-tts";
    if (contains_ci("zonos"))
        return "zonos";
    if (contains_ci("ggml-") && contains_ci(".bin"))
        return "whisper";

    // ---- Pass 2: GGUF metadata ----
    //
    // Only reached when the filename didn't clearly identify a backend.
    // Reads just the "general.architecture" key; no weight tensors.
    struct gguf_init_params gip = {/*.no_alloc=*/true, /*.ctx=*/nullptr};
    gguf_context* gctx = gguf_init_from_file(model_path.c_str(), gip);
    if (!gctx)
        return "";

    std::string result;
    const int key = gguf_find_key(gctx, "general.architecture");
    if (key >= 0) {
        const char* arch = gguf_get_val_str(gctx, key);
        if (arch) {
            const std::string a = arch;
            if (a == "whisper")
                result = "whisper";
            else if (a == "nemotron" || a == "nemotron-asr" || a == "nemotron-streaming")
                result = "nemotron";
            else if (a == "parakeet")
                result = "parakeet";
            else if (a == "parakeet-tdt" || a == "parakeet-ja" || a == "parakeet_ja")
                result = "parakeet";
            else if (a == "canary")
                result = "canary";
            else if (a == "lfm2-audio")
                result = "lfm2-audio";
            else if (a == "mini-omni2")
                result = "mini-omni2";
            else if (a == "canary-ctc")
                result = "canary";
            else if (a == "cohere")
                result = "cohere";
            else if (a == "cohere-transcribe")
                result = "cohere";
            else if (a == "cielvox-asr" || a == "qwen3-asr" || a == "cielvox2_asr" || a == "qwen3asr")
                result = "cielvox-asr";
            else if (a == "cielvox2" || a == "cielvox2" || a == "cielvox2")
                result = "cielvox2";
            else if (a == "orpheus")
                result = "orpheus";
            else if (a == "kokoro" || a == "styletts2" || a == "styletts2-ljspeech")
                result = "kokoro";
            else if (a == "voxcpm2" || a == "voxcpm2_tts" || a == "voxcpm2-tts")
                result = "voxcpm2-tts";
            else if (a == "cosyvoice3" || a == "cosyvoice3-tts" || a == "cosyvoice3_tts" || a == "cosyvoice3-llm")
                result = "cosyvoice3-tts";
            else if (a == "fastpitch" || a == "fastpitch-tts" || a == "fastpitch_tts")
                result = "fastpitch";
            else if (a == "piper" || a == "piper-tts" || a == "piper_tts" || a == "vits")
                result = "piper";
            else if (a == "melotts" || a == "melo-tts" || a == "melo_tts" || a == "vits2")
                result = "melotts";
            else if (a == "f5-tts" || a == "f5_tts" || a == "f5tts")
                result = "f5-tts";
            else if (a == "chatterbox" || a == "chatterbox_turbo" || a == "kartoffelbox")
                result = "chatterbox";
            else if (a == "tada" || a == "tada-tts" || a == "tada-3b-ml")
                result = "tada";
            else if (a == "m2m100" || a == "m2m_100")
                result = "m2m100";
            else if (a == "voxtral")
                result = "voxtral";
            else if (a == "voxtral4b" || a == "voxtral-4b" || a == "voxtral_4b")
                result = "voxtral4b";
            else if (a == "granite-speech" || a == "granite_speech" || a == "granitespeech")
                result = "granite";
            else if (a == "granite-nle" || a == "granite_nle" || a == "granitenle")
                result = "granite-4.1-nar";
            else if (a == "wav2vec2" || a == "wav2vec2-ctc")
                result = "wav2vec2";
            else if (a == "vibevoice" || a == "vibevoice-asr" || a == "vibevoice_asr")
                result = "vibevoice";
            else if (a == "kugelaudio" || a == "kugelaudio-tts" || a == "kugelaudio_tts")
                result = "kugelaudio";
            else if (a == "fastconformer-ctc" || a == "stt-fastconformer-ctc" || a == "stt_fastconformer_ctc")
                result = "fastconformer-ctc";
            else if (a == "glmasr" || a == "glm-asr" || a == "glm_asr")
                result = "glm-asr";
            else if (a == "kyutai-stt" || a == "kyutai_stt" || a == "kyutaistt")
                result = "kyutai-stt";
            else if (a == "firered-asr" || a == "firered_asr" || a == "firereadasr" || a == "firered-lid" ||
                     a == "firered_lid")
                result = "firered-asr";
            else if (a == "moonshine_streaming")
                return "moonshine-streaming";
            else if (a == "gemma4e2b" || a == "gemma4_e2b")
                return "gemma4-e2b";
            else if (a == "moonshine" || a == "moonshine-tiny" || a == "moonshine-base")
                result = "moonshine";
            else if (a == "omniasr-ctc" || a == "omniasr_ctc" || a == "omniasr" || a == "omniasr-300m" ||
                     a == "omniasr-llm" || a == "omniasr_llm")
                result = "omniasr";
            else if (a == "hubert" || a == "hubert-ctc")
                result = "wav2vec2";
            else if (a == "data2vec" || a == "data2vec-audio" || a == "data2vec_audio")
                result = "wav2vec2";
            else if (a == "fireredpunc")
                result = "fireredpunc";
            else if (a == "mimo_asr" || a == "mimo-asr")
                result = "mimo-asr";
            else if (a == "moss_audio" || a == "moss-audio")
                result = "moss-audio";
            else if (a == "funasr" || a == "fun_asr" || a == "fun-asr")
                result = "funasr";
            else if (a == "paraformer")
                result = "paraformer";
            else if (a == "sensevoice" || a == "sense_voice" || a == "sense-voice" || a == "sensevoicesmall")
                result = "sensevoice";
            else if (a == "indextts" || a == "indextts-1.5" || a == "indextts_1_5")
                result = "indextts";
            else if (a == "outetts" || a == "oute-tts" || a == "oute_tts")
                result = "outetts";
            else if (a == "pocket-tts" || a == "pocket_tts" || a == "pockettts")
                result = "pocket-tts";
            else if (a == "speecht5-tts" || a == "speecht5_tts" || a == "speecht5")
                result = "speecht5";
            else if (a == "bark" || a == "bark-tts" || a == "bark_tts")
                result = "bark";
            else if (a == "dia" || a == "dia-tts" || a == "dia_tts")
                result = "dia";
            else if (a == "csm" || a == "csm-tts" || a == "csm_tts")
                result = "csm";
            else if (a == "parler-tts" || a == "parler_tts" || a == "parlertts")
                result = "parler-tts";
            else if (a == "zonos" || a == "zonos-tts" || a == "zonos_tts")
                result = "zonos";
        }
    }
    gguf_free(gctx);
    return result;
}
