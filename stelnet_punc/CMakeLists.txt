# stelnet_punc — shared C++ punctuation restoration library.
#
# Two backends: FireRedPunc (BERT, Chinese+multilingual) and
# PCS (XLM-R, punctuation + capitalization + segmentation).
#
# Used by Stelnet (post-ASR cleanup) and CrispEmbed (post-OCR cleanup).
# Linked into CrispEmbed via add_subdirectory("../Stelnet/stelnet_punc")
# from CrispEmbed's top-level CMakeLists.txt.

cmake_minimum_required(VERSION 3.14)

# stelnet_punc links against the consumer's "core" target — which provides
# core_gguf::* symbols. Stelnet's stelnet-core and CrispEmbed's
# crispembed-core both supply these from identical sources.
if(TARGET stelnet-core)
    set(_stelnet_punc_core_target stelnet-core)
    set(_stelnet_punc_core_inc    ${CMAKE_CURRENT_SOURCE_DIR}/../src)
elseif(TARGET crispembed-core)
    set(_stelnet_punc_core_target crispembed-core)
    set(_stelnet_punc_core_inc "")
else()
    message(STATUS "stelnet_punc: no core target (stelnet-core or crispembed-core) — skipping")
    return()
endif()

add_library(stelnet_punc STATIC
    src/fireredpunc.cpp
    src/pcs.cpp
    include/stelnet_punc.h
)

target_include_directories(stelnet_punc
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
if(_stelnet_punc_core_inc)
    target_include_directories(stelnet_punc PRIVATE ${_stelnet_punc_core_inc})
endif()

target_link_libraries(stelnet_punc
    PUBLIC  ${_stelnet_punc_core_target} ggml ggml-base
)

target_compile_features(stelnet_punc PUBLIC cxx_std_17)
set_target_properties(stelnet_punc PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(NOT MSVC)
    target_compile_options(stelnet_punc PRIVATE -O3)
endif()
