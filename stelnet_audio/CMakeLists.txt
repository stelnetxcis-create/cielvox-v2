# stelnet_audio — shared C++ audio-encoder library.
#
# Used by Stelnet (qwen3_asr.cpp) and CrispEmbed (BidirLM-Omni audio path)
# to share the Whisper-shape Conv-stem + Transformer-encoder forward pass.
#
# Linked into CrispEmbed via add_subdirectory("../Stelnet/stelnet_audio") from
# CrispEmbed's top-level CMakeLists.txt.

cmake_minimum_required(VERSION 3.14)

# stelnet_audio links against the consumer's "core" target — which provides
# core_gguf::* and core_mel::* symbols. Stelnet's stelnet-core and
# CrispEmbed's crispembed-core both supply these from identical sources;
# we pick whichever the parent build defines so consumers don't end up
# with duplicate symbols.
if(TARGET stelnet-core)
    set(_stelnet_audio_core_target stelnet-core)
    set(_stelnet_audio_core_inc    ${CMAKE_CURRENT_SOURCE_DIR}/../src)
elseif(TARGET crispembed-core)
    set(_stelnet_audio_core_target crispembed-core)
    # CrispEmbed exposes core/* via crispembed-core's PUBLIC include dirs;
    # nothing extra needed here.
    set(_stelnet_audio_core_inc "")
else()
    message(STATUS "stelnet_audio: no core target (stelnet-core or crispembed-core) — skipping")
    return()
endif()

add_library(stelnet_audio STATIC
    src/audio_tower.cpp
    include/stelnet_audio.h
)

target_include_directories(stelnet_audio
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
if(_stelnet_audio_core_inc)
    target_include_directories(stelnet_audio PRIVATE ${_stelnet_audio_core_inc})
endif()

target_link_libraries(stelnet_audio
    PUBLIC  ${_stelnet_audio_core_target} ggml ggml-base
)

target_compile_features(stelnet_audio PUBLIC cxx_std_17)
set_target_properties(stelnet_audio PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(NOT MSVC)
    target_compile_options(stelnet_audio PRIVATE -O3)
endif()

if(GGML_METAL)
    target_compile_definitions(stelnet_audio PRIVATE GGML_USE_METAL)
    target_link_libraries(stelnet_audio PUBLIC ggml-metal)
endif()
if(GGML_CUDA)
    target_compile_definitions(stelnet_audio PRIVATE GGML_USE_CUDA)
    target_link_libraries(stelnet_audio PUBLIC ggml-cuda)
endif()
