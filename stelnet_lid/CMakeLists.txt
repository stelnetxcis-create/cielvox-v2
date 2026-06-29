# stelnet_lid — shared C++ text-based language identification library.
#
# Two backends: lid-fasttext (GlotLID/LID-176) and lid-cld3 (Google CLD3).
# Auto-detects from GGUF `general.architecture` key.
#
# Used by Stelnet (post-ASR LID) and CrispEmbed (OCR language routing).

cmake_minimum_required(VERSION 3.14)

if(TARGET stelnet-core)
    set(_stelnet_lid_core_target stelnet-core)
    set(_stelnet_lid_core_inc    ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    set(_stelnet_lid_defs        STELNET_BUILD)
elseif(TARGET crispembed-core)
    set(_stelnet_lid_core_target crispembed-core)
    set(_stelnet_lid_core_inc "")
    set(_stelnet_lid_defs "")
else()
    message(STATUS "stelnet_lid: no core target — skipping")
    return()
endif()

add_library(stelnet_lid STATIC
    src/lid_fasttext.cpp
    src/lid_cld3.cpp
    src/text_lid_dispatch.cpp
)

target_include_directories(stelnet_lid
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)
if(_stelnet_lid_core_inc)
    target_include_directories(stelnet_lid PRIVATE ${_stelnet_lid_core_inc})
endif()
if(_stelnet_lid_defs)
    target_compile_definitions(stelnet_lid PRIVATE ${_stelnet_lid_defs})
endif()

target_link_libraries(stelnet_lid
    PUBLIC  ${_stelnet_lid_core_target} ggml ggml-base
)

target_compile_features(stelnet_lid PUBLIC cxx_std_17)
set_target_properties(stelnet_lid PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(lid-fasttext ALIAS stelnet_lid)

if(NOT MSVC)
    target_compile_options(stelnet_lid PRIVATE -O3)
endif()
