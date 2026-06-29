// test-cielvox2tts-params.cpp — unit tests for cielvox2_context_params defaults
// and null-guard coverage. No GGUF required.

#include <catch2/catch_test_macros.hpp>
#include "cielvox2_tts.h"

TEST_CASE("cielvox2_tts_params: default values are sensible", "[unit][cielvox2_tts]") {
    struct cielvox2_context_params p = cielvox2_context_default_params();

    REQUIRE(p.n_threads >= 1);
    REQUIRE(p.verbosity >= 0);
}

TEST_CASE("cielvox2_init_from_file: null path returns nullptr", "[unit][cielvox2_tts]") {
    struct cielvox2_context_params p = cielvox2_context_default_params();
    struct cielvox2_context* ctx = cielvox2_init_from_file(nullptr, p);
    REQUIRE(ctx == nullptr);
}

TEST_CASE("cielvox2_init_from_file: empty path returns nullptr", "[unit][cielvox2_tts]") {
    struct cielvox2_context_params p = cielvox2_context_default_params();
    struct cielvox2_context* ctx = cielvox2_init_from_file("", p);
    REQUIRE(ctx == nullptr);
}

TEST_CASE("cielvox2_free: NULL context is a no-op", "[unit][cielvox2_tts]") {
    cielvox2_free(nullptr);
    SUCCEED("cielvox2_free tolerated a NULL ctx.");
}
