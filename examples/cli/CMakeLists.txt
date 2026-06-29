#
# stelnet — unified ASR CLI (formerly stelnet).
#
# Drives all model backends in src/ (stelnet, parakeet, canary, cohere,
# qwen3, voxtral, voxtral4b, granite) from a single binary via the
# --backend flag. When --backend is omitted or set to "whisper" the
# historical stelnet code path is used unchanged.
#
# The CMake target is named `stelnet-cli`, but the shipped binary stays
# `stelnet` via OUTPUT_NAME for user-facing compatibility.

set(TARGET stelnet-cli)

add_executable(${TARGET}
    cli.cpp
    stelnet_backend.cpp
    stelnet_backend_stelnet.cpp
    stelnet_backend_nemotron.cpp
    stelnet_backend_parakeet.cpp
    stelnet_backend_canary.cpp
    stelnet_backend_lfm2_audio.cpp
    stelnet_backend_mini_omni2.cpp
    stelnet_backend_cohere.cpp
    stelnet_backend_granite.cpp
    stelnet_backend_granite_nle.cpp
    stelnet_backend_voxtral.cpp
    stelnet_backend_voxtral4b.cpp
    stelnet_backend_cielvox_asr.cpp
    stelnet_backend_fastconformer_ctc.cpp
    stelnet_backend_wav2vec2.cpp
    stelnet_backend_vibevoice.cpp
    stelnet_backend_kugelaudio.cpp
    stelnet_backend_cielvox2_tts.cpp
    stelnet_backend_orpheus.cpp
    stelnet_backend_chatterbox.cpp
    stelnet_backend_tada.cpp
    stelnet_backend_indextts.cpp
    stelnet_backend_f5_tts.cpp
    stelnet_backend_pocket_tts.cpp
    stelnet_backend_csm.cpp
    stelnet_backend_parler.cpp
    stelnet_backend_fastpitch.cpp
    stelnet_backend_bark.cpp
    stelnet_backend_dia.cpp
    stelnet_backend_voxcpm2_tts.cpp
    stelnet_backend_cosyvoice3.cpp
    stelnet_backend_kokoro.cpp
    stelnet_backend_piper.cpp
    stelnet_backend_melotts.cpp
    stelnet_backend_m2m100.cpp
    stelnet_backend_t5.cpp
    stelnet_backend_glm_asr.cpp
    stelnet_backend_kyutai_stt.cpp
    stelnet_backend_firered_asr.cpp
    stelnet_backend_moonshine.cpp
    stelnet_backend_moonshine_streaming.cpp
    stelnet_backend_gemma4_e2b.cpp
    stelnet_backend_omniasr.cpp
    stelnet_backend_mimo_asr.cpp
    stelnet_backend_moss_audio.cpp
    stelnet_backend_funasr.cpp
    stelnet_backend_paraformer.cpp
    stelnet_backend_sensevoice.cpp
    stelnet_backend_speecht5.cpp
    stelnet_backend_zonos.cpp
    stelnet_aligner_cli.cpp
    stelnet_vad_cli.cpp
    stelnet_lid_cli.cpp
    stelnet_diarize_cli.cpp
    stelnet_output.cpp
    stelnet_model_mgr_cli.cpp
    stelnet_diagnostics.cpp
    stelnet_run.cpp
    stelnet_server.cpp
    stelnet_tts_chunking.cpp
    # Real-time WebSocket ASR streaming, shared with the standalone server
    # (examples/server). Wired into `stelnet --server` via --ws-port.
    ${CMAKE_CURRENT_SOURCE_DIR}/../server/ws_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../server/realtime_server.cpp
    # Wyoming protocol TCP server (issue #172) — Home Assistant Assist STT+TTS.
    wyoming.cpp
)

# ---------------------------------------------------------------------------
# Build-info macros surfaced to stelnet_diagnostics.cpp. Together they
# answer "what exact bits am I running?" — important for bug reports
# from prebuilt Docker tags where the user has no source tree handy.
#
# `git-vars.cmake` populates GIT_SHA1 / GIT_DATE / GIT_COMMIT_SUBJECT.
# Re-include defensively in case a downstream consumer of this file
# imports it without the top-level CMakeLists having run yet.
# ---------------------------------------------------------------------------
if (NOT DEFINED GIT_SHA1)
    include(${CMAKE_SOURCE_DIR}/cmake/git-vars.cmake)
endif()
string(TIMESTAMP STELNET_BUILD_DATE_STR "%Y-%m-%dT%H:%M:%SZ" UTC)
target_compile_definitions(${TARGET} PRIVATE
    STELNET_VERSION_STR="${PROJECT_VERSION}"
    STELNET_GIT_SHA1="${GIT_SHA1}"
    STELNET_GIT_DATE="${GIT_DATE}"
    STELNET_GIT_SUBJECT="${GIT_COMMIT_SUBJECT}"
    STELNET_BUILD_DATE="${STELNET_BUILD_DATE_STR}"
    STELNET_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
)
if (GGML_CUDA)
    # CMAKE_CUDA_ARCHITECTURES is a CMake list ("75-real;86-real;..."). When
    # fed into target_compile_definitions, CMake's command-line generator
    # treats list items as SEPARATE arguments — the cl.exe line ends up as
    # `-DSTELNET_CUDA_ARCHS="75-real -D86-real -D120-virtual"` which has
    # an unbalanced quote and then leftover `-D120-virtual"` confuses
    # link.exe into trying to open random tokens as .obj files
    # ("LNK1181: cannot open input file 'GIT_DATE.obj'"). Convert the
    # separator to "," before embedding so it remains a single token.
    string(REPLACE ";" "," STELNET_CUDA_ARCHS_STR "${CMAKE_CUDA_ARCHITECTURES}")
    target_compile_definitions(${TARGET} PRIVATE
        STELNET_HAVE_CUDA=1
        STELNET_CUDA_VERSION="${CUDAToolkit_VERSION}"
        STELNET_CUDA_ARCHS="${STELNET_CUDA_ARCHS_STR}"
    )
endif()
if (GGML_METAL)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_METAL=1)
endif()
if (GGML_VULKAN)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_VULKAN=1)
endif()
if (GGML_HIP)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_HIP=1)
endif()
if (GGML_SYCL)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_SYCL=1)
endif()
if (GGML_OPENCL)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_OPENCL=1)
endif()
if (GGML_BLAS)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_BLAS=1)
endif()
if (GGML_MUSA)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_MUSA=1)
endif()

# ---------------------------------------------------------------------------
# stelnet-diff-lib — shared ground-truth diff harness (companion to
# tools/dump_reference.py).
#
# Exposed as a small static library so both the `stelnet-diff` binary
# and the older differential-test drivers under
# examples/{qwen3,voxtral}-test-*/ can link it without re-compiling the
# same sources. The library has no per-model dependencies — it only
# needs core_gguf from stelnet-core and the ggml CPU backend for its
# tensor loader.
# ---------------------------------------------------------------------------
add_library(stelnet-diff-lib STATIC
    stelnet_diff.cpp
    stelnet_diff.h
)
target_include_directories(stelnet-diff-lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(stelnet-diff-lib PUBLIC
    stelnet-core
    ggml
)
set_target_properties(stelnet-diff-lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# stelnet-diff — the CLI wrapper binary. Separate target so downstream
# users who don't care about differential testing don't pay the extra
# link time.
# ---------------------------------------------------------------------------
add_executable(stelnet-diff
    stelnet_diff_main.cpp
)
target_link_libraries(stelnet-diff PRIVATE
    common
    stelnet-lib
    parakeet
    canary
    cohere
    granite_speech
    granite_nle
    voxtral
    voxtral4b
    cielvox2_asr
    cielvox2_tts
    orpheus
    kokoro
    chatterbox
    tada-tts
    indextts
    m2m100
    gemma4_e2b
    mimo_tokenizer
    mimo_asr
    moss_audio
    voxcpm2_tts
    funasr
    paraformer
    sensevoice
    cosyvoice3_tts
    csm-tts
    parler-tts
    mini-omni2
    stelnet-diff-lib
    ${FFMPEG_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
install(TARGETS stelnet-diff RUNTIME BUNDLE DESTINATION bin)

# ---------------------------------------------------------------------------
# stelnet-lid — standalone text language identification.
# Auto-routes between fastText (GlotLID-V3, LID-176) and Google CLD3 by
# peeking the GGUF's general.architecture key at load time. One binary
# for any text-LID GGUF.
#
# Link order: text-lid-dispatch BEFORE stelnet-lib because the dispatch
# static library has unresolved references to stelnet_registry_lookup
# / stelnet_cache::ensure_cached_file (in stelnet_model_registry.cpp
# / stelnet_cache.cpp inside the main `stelnet-lib` library), and MinGW's
# GNU ld processes static libraries strictly left-to-right — listing
# the consumer first lets ld pick up the missing definitions when it
# subsequently scans `stelnet-lib`. Without this, msys2 UCRT64 build.yml
# fails with `undefined reference to stelnet_registry_lookup` etc.
# (Windows MSVC and Linux gcc are forgiving here; only MinGW UCRT64
# enforces order strictly enough to surface the bug.)
#
# Windows networking libs match stelnet-cli's set: winhttp for
# auto-download (text-lid GGUFs come from HF over HTTPS), ws2_32 +
# crypt32 because httplib.h's #pragma comment line auto-links them on
# MSVC but MinGW silently ignores those pragmas.
# ---------------------------------------------------------------------------
add_executable(stelnet-lid stelnet_lid_main.cpp)
if(TARGET stelnet_lid)
    target_link_libraries(stelnet-lid PRIVATE stelnet_lid stelnet-lib)
if(TARGET text-lid-dispatch)
    target_link_libraries(stelnet-lid PRIVATE text-lid-dispatch)
endif()
else()
    target_link_libraries(stelnet-lid PRIVATE text-lid-dispatch stelnet-lib)
endif()
target_link_libraries(stelnet-lid PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    $<$<BOOL:${WIN32}>:winhttp>
    $<$<BOOL:${WIN32}>:ws2_32>
    $<$<BOOL:${WIN32}>:crypt32>
)
install(TARGETS stelnet-lid RUNTIME BUNDLE DESTINATION bin)

# ---------------------------------------------------------------------------
# mimo-tokenizer-smoke — finiteness/magnitude smoke test for the MiMo
# tokenizer encoder forward path. Used during PLAN #51 step 2 development
# while the upstream safetensors download is in flight.
# ---------------------------------------------------------------------------
add_executable(mimo-tokenizer-smoke mimo_tokenizer_smoke.cpp)
target_link_libraries(mimo-tokenizer-smoke PRIVATE
    common
    stelnet-lib
    mimo_tokenizer
    ${FFMPEG_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME stelnet)

# httplib.h is in the server example directory
target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/examples/server)

include(DefaultTargetOptions)

target_link_libraries(${TARGET} PRIVATE
    common
    stelnet-lib
    parakeet
    canary
    cohere
    granite_speech
    granite_nle
    voxtral
    voxtral4b
    cielvox2_asr
    cielvox2_tts
    orpheus
    chatterbox
    tada-tts
    indextts
    kokoro
    piper-tts
    speecht5-tts
    fastpitch-tts
    bark-tts
    dia-tts
    pocket-tts
    csm-tts
    voxcpm2_tts
    m2m100
    t5_translate
    canary_ctc
    wav2vec2-ggml
    vibevoice
    kugelaudio
    silero-lid
    $<$<TARGET_EXISTS:text-lid-dispatch>:text-lid-dispatch>
    pyannote-seg
    funasr
    paraformer
    sensevoice
    moss_audio
    ${FFMPEG_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    $<$<BOOL:${WIN32}>:winhttp>
    # Winsock + crypto for httplib.h (transitively pulled in by
    # stelnet_server.cpp). MSVC auto-links these via #pragma comment
    # but MinGW (msys2 UCRT64 / CLANG64) ignores those pragmas, so the
    # link fails with __imp_freeaddrinfo / WSASocketW / etc. undefined.
    # Listing them unconditionally is harmless on MSVC.
    $<$<BOOL:${WIN32}>:ws2_32>
    $<$<BOOL:${WIN32}>:crypt32>
    $<$<BOOL:${WIN32}>:bcrypt>
    $<$<BOOL:${WIN32}>:iphlpapi>
    $<$<BOOL:${WIN32}>:shell32>
    $<$<BOOL:${STELNET_CURL}>:CURL::libcurl>
)

if (STELNET_CURL)
    target_compile_definitions(${TARGET} PRIVATE STELNET_USE_LIBCURL)
endif()

# Optional outetts backend (source files may not be present in all checkouts)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stelnet_backend_outetts.cpp")
    target_sources(${TARGET} PRIVATE stelnet_backend_outetts.cpp)
    target_link_libraries(${TARGET} PRIVATE outetts)
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_OUTETTS=1)
endif()

# Optional parler-tts backend
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stelnet_backend_parler.cpp")
    target_link_libraries(${TARGET} PRIVATE parler-tts)
endif()

# Optional MP3 output (libmp3lame)
find_library(LAME_LIBRARY NAMES mp3lame)
find_path(LAME_INCLUDE_DIR lame/lame.h)
if (LAME_LIBRARY AND LAME_INCLUDE_DIR)
    target_link_libraries(${TARGET} PRIVATE ${LAME_LIBRARY})
    target_include_directories(${TARGET} PRIVATE ${LAME_INCLUDE_DIR})
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_LAME=1)
    message(STATUS "MP3 output enabled (libmp3lame)")
else()
    message(STATUS "MP3 output disabled (libmp3lame-dev not found)")
endif()

# Optional Opus output (libopus + libogg for OGG container)
find_library(OPUS_LIBRARY NAMES opus)
find_path(OPUS_INCLUDE_DIR opus/opus.h)
if (OPUS_LIBRARY AND OPUS_INCLUDE_DIR)
    target_link_libraries(${TARGET} PRIVATE ${OPUS_LIBRARY})
    target_include_directories(${TARGET} PRIVATE ${OPUS_INCLUDE_DIR})
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_OPUS=1)
    message(STATUS "Opus output enabled (libopus)")
else()
    message(STATUS "Opus output disabled (libopus-dev not found)")
endif()

# Optional C2PA Content Credentials signing (c2pa-c library)
find_library(C2PA_LIBRARY NAMES c2pa)
find_path(C2PA_INCLUDE_DIR NAMES c2pa.h c2pa/c2pa.h)
if (C2PA_LIBRARY AND C2PA_INCLUDE_DIR)
    target_link_libraries(${TARGET} PRIVATE ${C2PA_LIBRARY})
    target_include_directories(${TARGET} PRIVATE ${C2PA_INCLUDE_DIR})
    target_compile_definitions(${TARGET} PRIVATE STELNET_HAVE_C2PA=1)
    message(STATUS "C2PA signing enabled (c2pa-c)")
else()
    message(STATUS "C2PA signing disabled (c2pa-c not found; install from github.com/contentauth/c2pa-c)")
endif()

install(TARGETS ${TARGET} RUNTIME BUNDLE DESTINATION bin)

# ---------------------------------------------------------------------------
# stelnet-chat — minimal reference CLI for the stelnet_chat_* C ABI.
#
# Pure stdin → stdout chat loop. Streaming by default (one-shot when
# stdin is a pipe). Demonstrates the smallest downstream-binding shape
# and doubles as a smoke test for the chat surface against any GGUF
# chat model on disk. No SDL2 dep — text-only.
# ---------------------------------------------------------------------------
add_executable(stelnet-chat
    stelnet_chat_main.cpp
)
target_include_directories(stelnet-chat PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(stelnet-chat PRIVATE
    stelnet-lib
    ${CMAKE_THREAD_LIBS_INIT}
)
include(DefaultTargetOptions)
install(TARGETS stelnet-chat RUNTIME BUNDLE DESTINATION bin)
