# stelnet_truecase — shared C++ truecasing library.
#
# Three backends: statistical, CRF, BiLSTM.
# Used by Stelnet (post-ASR German truecasing) and CrispEmbed (post-OCR).

cmake_minimum_required(VERSION 3.14)

if(TARGET stelnet-core)
    set(_stelnet_tc_core_target stelnet-core)
    set(_stelnet_tc_core_inc    ${CMAKE_CURRENT_SOURCE_DIR}/../src)
elseif(TARGET crispembed-core)
    set(_stelnet_tc_core_target crispembed-core)
    set(_stelnet_tc_core_inc "")
else()
    message(STATUS "stelnet_truecase: no core target — skipping")
    return()
endif()

add_library(stelnet_truecase STATIC
    src/truecaser.cpp
    src/truecaser_crf.cpp
    src/truecaser_lstm.cpp
)

target_include_directories(stelnet_truecase
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)
if(_stelnet_tc_core_inc)
    target_include_directories(stelnet_truecase PRIVATE ${_stelnet_tc_core_inc})
endif()

target_link_libraries(stelnet_truecase
    PUBLIC  ${_stelnet_tc_core_target}
)

target_compile_features(stelnet_truecase PUBLIC cxx_std_17)
set_target_properties(stelnet_truecase PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(NOT MSVC)
    target_compile_options(stelnet_truecase PRIVATE -O3)
endif()
