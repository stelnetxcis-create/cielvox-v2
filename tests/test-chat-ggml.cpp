// test-chat-ggml.cpp — end-to-end smoke for the stelnet_chat_* C ABI.
//
// Gated on STELNET_CHAT_TEST_MODEL — a path to a small GGUF chat model
// (e.g. harrier-270m-q4_k.gguf, qwen2.5-0.5b-instruct, smollm2-360m).
// When unset the test is reported as SKIPPED so unrelated builds stay
// green without a model on disk.
//
// Verifies in one pass:
//   • stelnet_chat_open with default params returns a session
//   • stelnet_chat_n_ctx / _template_name return non-trivial values
//   • stelnet_chat_generate returns non-empty UTF-8 (one-shot path)
//   • stelnet_chat_generate_stream fires on_token at least once and
//     the concatenated chunks equal the one-shot output for the same
//     seed (regression guard against streaming drift)
//   • stelnet_chat_reset clears history without crashing

#include <catch2/catch_test_macros.hpp>

#include "stelnet_chat.h"

#include <cstdlib>
#include <cstring>
#include <string>

namespace {

const char* test_model_path() {
    return std::getenv("STELNET_CHAT_TEST_MODEL");
}

void on_token_appender(const char* chunk, void* user) {
    auto* out = static_cast<std::string*>(user);
    out->append(chunk);
}

} // namespace

TEST_CASE("stelnet_chat one-shot generate", "[chat][gguf]") {
    const char* model = test_model_path();
    if (!model) {
        SKIP("STELNET_CHAT_TEST_MODEL not set; skipping chat smoke");
    }

    stelnet_chat_open_params op;
    stelnet_chat_open_params_default(&op);
    op.n_gpu_layers = -1;
    op.n_ctx = 1024;

    stelnet_chat_error err{};
    stelnet_chat_session_t s = stelnet_chat_open(model, &op, &err);
    REQUIRE(s != nullptr);
    REQUIRE(err.code == 0);

    REQUIRE(stelnet_chat_n_ctx(s) > 0);
    const char* tmpl = stelnet_chat_template_name(s);
    REQUIRE(tmpl != nullptr);
    REQUIRE(std::strlen(tmpl) > 0);

    stelnet_chat_generate_params gp;
    stelnet_chat_generate_params_default(&gp);
    gp.max_tokens = 16;
    gp.temperature = 0.0f; // greedy → reproducible across one-shot + stream
    gp.seed = 1;

    stelnet_chat_message messages[] = {
        {"system", "You are a terse assistant. Answer in one word."},
        {"user", "Say hello."},
    };

    char* out = stelnet_chat_generate(s, messages, 2, &gp, &err);
    REQUIRE(out != nullptr);
    REQUIRE(err.code == 0);
    REQUIRE(std::strlen(out) > 0);
    const std::string one_shot = out;
    stelnet_chat_string_free(out);

    // Streaming path with the same seed + greedy must reproduce one-shot.
    REQUIRE(stelnet_chat_reset(s, &err) == 0);
    std::string streamed;
    int32_t rc = stelnet_chat_generate_stream(s, messages, 2, &gp, on_token_appender, &streamed, &err);
    REQUIRE(rc == 0);
    REQUIRE(err.code == 0);
    REQUIRE_FALSE(streamed.empty());
    REQUIRE(streamed == one_shot);

    stelnet_chat_close(s);
}
