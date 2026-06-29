// test-registry.cpp — unit tests for stelnet_model_registry.
//
// Verifies registry lookup, backend listing, and filename-based reverse
// lookup. No network, no models — pure in-memory registry queries.

#include <catch2/catch_test_macros.hpp>

#include "stelnet_model_registry.h"

#include <cstring>
#include <string>

TEST_CASE("registry: lookup known backend returns valid entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("whisper", e);
    REQUIRE(found);
    REQUIRE(std::string(e.filename).find("ggml") != std::string::npos);
    REQUIRE(std::string(e.url).find("huggingface") != std::string::npos);
}

TEST_CASE("registry: lookup unknown backend returns false", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("nonexistent-backend-xyz", e);
    REQUIRE_FALSE(found);
}

TEST_CASE("registry: parakeet entry has correct filename", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("parakeet", e);
    REQUIRE(found);
    REQUIRE(std::string(e.filename).find("parakeet") != std::string::npos);
}

TEST_CASE("registry: mimo-asr has entry (added in #63)", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("mimo-asr", e);
    REQUIRE(found);
    REQUIRE(std::string(e.filename).find("mimo-asr") != std::string::npos);
}

TEST_CASE("registry: omniasr has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("omniasr", e);
    REQUIRE(found);
}

TEST_CASE("registry: omniasr-300m has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("omniasr-300m", e);
    REQUIRE(found);
    REQUIRE(std::string(e.filename).find("300m") != std::string::npos);
}

TEST_CASE("registry: omniasr-llm has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("omniasr-llm", e);
    REQUIRE(found);
}

TEST_CASE("registry: omniasr-llm-1b has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("omniasr-llm-1b", e);
    REQUIRE(found);
    REQUIRE(std::string(e.filename).find("1b") != std::string::npos);
}

TEST_CASE("registry: granite-4.1 has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("granite-4.1", e);
    REQUIRE(found);
    REQUIRE(std::string(e.filename).find("granite") != std::string::npos);
}

TEST_CASE("registry: gemma4-e2b has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("gemma4-e2b", e);
    REQUIRE(found);
}

TEST_CASE("registry: vibevoice has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("vibevoice", e);
    REQUIRE(found);
}

TEST_CASE("registry: wav2vec2 aligner aliases resolve", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("wav2vec2-aligner", e));
    REQUIRE(e.backend == "wav2vec2-aligner");
    REQUIRE(e.filename.find("wav2vec2") != std::string::npos);

    REQUIRE(stelnet_registry_lookup("wav2vec2-aligner-en", e));
    REQUIRE(e.filename == "wav2vec2-xlsr-en-q4_k.gguf");

    REQUIRE(stelnet_registry_lookup("wav2vec2-aligner-de", e));
    REQUIRE(e.filename.find("german") != std::string::npos);

    for (const auto& [alias, filename_part] : {
             std::pair{"wav2vec2-aligner-fr", "french"},
             std::pair{"wav2vec2-aligner-es", "spanish"},
             std::pair{"wav2vec2-aligner-it", "italian"},
             std::pair{"wav2vec2-aligner-ja", "japanese"},
             std::pair{"wav2vec2-aligner-zh", "chinese-zh-cn"},
             std::pair{"wav2vec2-aligner-nl", "dutch"},
             std::pair{"wav2vec2-aligner-uk", "uk-with-small-lm"},
             std::pair{"wav2vec2-aligner-pt", "portuguese"},
             std::pair{"wav2vec2-aligner-ar", "arabic"},
             std::pair{"wav2vec2-aligner-cs", "cs-250"},
         }) {
        REQUIRE(stelnet_registry_lookup(alias, e));
        REQUIRE(std::string(e.filename).find(filename_part) != std::string::npos);
        REQUIRE(e.backend == alias);
    }
}

TEST_CASE("registry: preferred quant rewrites primary filename", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("chatterbox", e, "q4_k");
    REQUIRE(found);
    REQUIRE(e.filename == "chatterbox-t3-q4_k.gguf");
}

TEST_CASE("registry: companion quant can be resolved independently", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("chatterbox", e, "q4_k");
    REQUIRE(found);
    REQUIRE(e.companion_filename == "chatterbox-s3gen-q4_k.gguf");
}

TEST_CASE("registry: non-quantized companion remains unchanged", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup("cielvox2-tts", e, "q4_k");
    REQUIRE(found);
    REQUIRE(e.companion_filename == "cielvox2-tts-tokenizer-12hz.gguf");
}

TEST_CASE("registry: companion filename lookup resolves the companion entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup_by_filename("cielvox2-tts-tokenizer-12hz.gguf", e);
    REQUIRE(found);
    REQUIRE(e.backend == "cielvox2-tts");
    REQUIRE(e.filename == "cielvox2-tts-tokenizer-12hz.gguf");
    REQUIRE(e.url.find("cielvox2-tts-tokenizer-12hz-GGUF") != std::string::npos);
}

TEST_CASE("registry: quantized companion filename lookup preserves the requested quant", "[unit][registry]") {
    StelnetRegistryEntry e;
    bool found = stelnet_registry_lookup_by_filename("cielvox2-tts-tokenizer-12hz-q8_0.gguf", e);
    REQUIRE(found);
    REQUIRE(e.backend == "cielvox2-tts");
    REQUIRE(e.filename == "cielvox2-tts-tokenizer-12hz-q8_0.gguf");
    REQUIRE(e.url.find("cielvox2-tts-tokenizer-12hz-q8_0.gguf") != std::string::npos);
}

// ── companion_approx_size (#146 / #148) ──────────────────────────────

TEST_CASE("registry: companion_approx_size populated for mimo-asr", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("mimo-asr", e));
    REQUIRE(!e.companion_filename.empty());
    REQUIRE(!e.companion_approx_size.empty());
    REQUIRE(e.companion_approx_size != e.approx_size); // tokenizer != LM size
    REQUIRE(e.companion_approx_size.find("MB") != std::string::npos);
}

TEST_CASE("registry: companion_approx_size populated for cielvox2-tts", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("cielvox2-tts", e));
    REQUIRE(!e.companion_approx_size.empty());
    REQUIRE(e.companion_approx_size != e.approx_size);
}

TEST_CASE("registry: companion_approx_size populated for orpheus", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("orpheus", e));
    REQUIRE(!e.companion_approx_size.empty());
    REQUIRE(e.companion_approx_size != e.approx_size);
}

TEST_CASE("registry: companion_approx_size populated for chatterbox", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("chatterbox", e));
    REQUIRE(!e.companion_approx_size.empty());
    REQUIRE(e.companion_approx_size != e.approx_size);
}

TEST_CASE("registry: chatterbox family keeps multilingual and finetunes separate", "[unit][registry]") {
    StelnetRegistryEntry e;

    REQUIRE(stelnet_registry_lookup("chatterbox", e));
    REQUIRE(e.filename == "chatterbox-t3-q8_0.gguf");
    REQUIRE(e.companion_filename == "chatterbox-s3gen-q8_0.gguf");

    REQUIRE(stelnet_registry_lookup("kartoffelbox-turbo", e));
    REQUIRE(e.filename.find("kartoffelbox-turbo-t3") != std::string::npos);
    REQUIRE(e.companion_filename == "chatterbox-turbo-s3gen-f16.gguf");

    REQUIRE(stelnet_registry_lookup("lahgtna-chatterbox", e));
    REQUIRE(e.filename == "chatterbox-t3-f16.gguf");
    REQUIRE(e.companion_filename == "chatterbox-s3gen-q8_0.gguf");
}

TEST_CASE("registry: companion_approx_size empty for backends without companion", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("whisper", e));
    REQUIRE(e.companion_filename.empty());
    REQUIRE(e.companion_approx_size.empty());
}

TEST_CASE("registry: companion filename lookup uses companion size, not LM size (#146)", "[unit][registry]") {
    StelnetRegistryEntry e;
    // Look up mimo-tokenizer by filename — should get the tokenizer's
    // size (~395 MB), not the LM's size (~4.2 GB).
    REQUIRE(stelnet_registry_lookup_by_filename("mimo-tokenizer-q4_k.gguf", e));
    REQUIRE(e.approx_size.find("MB") != std::string::npos);
    REQUIRE(e.approx_size.find("GB") == std::string::npos); // must NOT be the LM's 4.2 GB
}

TEST_CASE("registry: cielvox2-tts tokenizer filename lookup uses companion size (#146)", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup_by_filename("cielvox2-tts-tokenizer-12hz.gguf", e));
    // The tokenizer is ~60 MB, the LM is ~986 MB. The size should
    // reflect the tokenizer, not the LM.
    REQUIRE(e.approx_size.find("60") != std::string::npos);
}

TEST_CASE("registry: all entries with companions have companion_approx_size set", "[unit][registry]") {
    const int n = stelnet_registry_count();
    for (int i = 0; i < n; ++i) {
        StelnetRegistryEntry e;
        if (!stelnet_registry_get_at(i, e)) continue;
        if (!e.companion_filename.empty()) {
            REQUIRE(!e.companion_approx_size.empty());
        }
    }
}

// ── #152: fill() robustness — no entry may crash on lookup ──────────

TEST_CASE("registry: every entry fills without crash (#152)", "[unit][registry]") {
    // The SIGSEGV in #152 was heap corruption, but this test ensures
    // fill() handles every Entry in the static table without tripping
    // on NULL fields or malformed strings.
    const int n = stelnet_registry_count();
    REQUIRE(n > 0);
    for (int i = 0; i < n; ++i) {
        StelnetRegistryEntry e;
        bool ok = stelnet_registry_get_at(i, e);
        REQUIRE(ok);
        REQUIRE(!e.backend.empty());
        REQUIRE(!e.filename.empty());
        REQUIRE(!e.url.empty());
        REQUIRE(!e.approx_size.empty());
        // companion fields: either both set or both empty
        REQUIRE((e.companion_filename.empty() == e.companion_url.empty()));
    }
}

TEST_CASE("registry: fill with preferred quant does not crash on any entry (#152)", "[unit][registry]") {
    const int n = stelnet_registry_count();
    for (int i = 0; i < n; ++i) {
        StelnetRegistryEntry e;
        // Exercise the quant-rewriting path for every entry
        stelnet_registry_get_at(i, e);
        StelnetRegistryEntry eq;
        stelnet_registry_lookup(e.backend, eq, "q4_k");
        REQUIRE(!eq.backend.empty());
        REQUIRE(!eq.filename.empty());
    }
}

// ── v0.7.0 new backends: verify they exist in the registry ──────────

TEST_CASE("registry: zonos has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("zonos", e));
    REQUIRE(e.filename.find("zonos") != std::string::npos);
}

TEST_CASE("registry: kugelaudio has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("kugelaudio", e));
}

TEST_CASE("registry: melotts has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("melotts", e));
}

TEST_CASE("registry: cosyvoice3-tts has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("cosyvoice3-tts", e));
}

// The CLI / kaggle benchmark pass the short `--backend cosyvoice3` alias to
// `-m auto`; the registry must resolve it to the canonical `cosyvoice3-tts`
// entry via the `-tts`-suffix fallback (else `-m auto` fails instantly with
// "no default model registered" — full-backend-sweep cosyvoice3 FAIL).
TEST_CASE("registry: cosyvoice3 short alias resolves via -tts fallback", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("cosyvoice3", e));
    REQUIRE(e.filename == "cosyvoice3-llm-q4_k.gguf");
}

TEST_CASE("registry: dia has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("dia", e));
}

TEST_CASE("registry: f5-tts has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("f5-tts", e));
}

TEST_CASE("registry: bark has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("bark", e));
}

TEST_CASE("registry: piper has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("piper", e));
}

TEST_CASE("registry: csm (sesame) has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("csm", e));
}

TEST_CASE("registry: pocket-tts has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("pocket-tts", e));
}

TEST_CASE("registry: speecht5 has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("speecht5", e));
}

TEST_CASE("registry: fastpitch has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("fastpitch", e));
}

TEST_CASE("registry: parler-tts has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("parler-tts", e));
}

TEST_CASE("registry: voxcpm2-tts has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("voxcpm2-tts", e));
}

TEST_CASE("registry: moss-audio has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("moss-audio", e));
}

TEST_CASE("registry: sensevoice has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("sensevoice", e));
}

TEST_CASE("registry: paraformer has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("paraformer", e));
}

TEST_CASE("registry: outetts has entry", "[unit][registry]") {
    StelnetRegistryEntry e;
    REQUIRE(stelnet_registry_lookup("outetts", e));
}
