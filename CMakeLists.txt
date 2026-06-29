cmake_minimum_required(VERSION 3.10...3.29)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION_CONTENT)
string(STRIP "${PROJECT_VERSION_CONTENT}" PROJECT_VERSION_CONTENT)

project(stelnet VERSION ${PROJECT_VERSION_CONTENT} LANGUAGES C CXX)

# src/CMakeLists.txt promotes stelnet_audio.cpp and stelnet_mic.cpp to
# LANGUAGE OBJCXX on Apple so miniaudio's CoreAudio backend can include
# <AVFoundation/AVFoundation.h>. CMake refuses to honor that property
# unless the language is enabled at project scope.
if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
endif()

include(CheckIncludeFileCXX)

set(SOVERSION 1)

#set(CMAKE_WARN_DEPRECATED YES)
set(CMAKE_WARN_UNUSED_CLI YES)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT XCODE AND NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Add path to modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(STELNET_STANDALONE ON)

    include(git-vars)

    # configure project version
    configure_file(${CMAKE_SOURCE_DIR}/bindings/javascript/package-tmpl.json ${CMAKE_SOURCE_DIR}/bindings/javascript/package.json @ONLY)
else()
    set(STELNET_STANDALONE OFF)
endif()

# Always populate GIT_SHA1 / GIT_DATE / GIT_COMMIT_SUBJECT — the CLI
# build-info banner uses them whether or not we're the top-level build.
if (NOT DEFINED GIT_SHA1)
    include(git-vars)
endif()

if (EMSCRIPTEN)
    set(BUILD_SHARED_LIBS_DEFAULT OFF)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    option(STELNET_WASM            "stelnet: build WASM/JS target (Emscripten)"              ON)
    option(STELNET_WASM_SINGLE_FILE "stelnet: embed WASM inside the generated stelnet.js"   ON)

    # Multithreaded WASM: -pthread enables SharedArrayBuffer-based threading
    # in the browser. The hosting page MUST set COOP/COEP headers:
    #   Cross-Origin-Opener-Policy: same-origin
    #   Cross-Origin-Embedder-Policy: require-corp
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s TOTAL_STACK=5242880")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s TOTAL_STACK=5242880")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
else()
    if (MINGW)
        set(BUILD_SHARED_LIBS_DEFAULT OFF)
    else()
        set(BUILD_SHARED_LIBS_DEFAULT ON)
    endif()
endif()

option(BUILD_SHARED_LIBS "build shared libraries" ${BUILD_SHARED_LIBS_DEFAULT})

#
# option list
#

# debug
option(STELNET_ALL_WARNINGS           "stelnet: enable all compiler warnings"                   ON)
option(STELNET_ALL_WARNINGS_3RD_PARTY "stelnet: enable all compiler warnings in 3rd party libs" OFF)

# build
option(STELNET_FATAL_WARNINGS  "stelnet: enable -Werror flag"               OFF)
option(STELNET_USE_SYSTEM_GGML "stelnet: use system-installed GGML library" OFF)

# sanitizers
option(STELNET_SANITIZE_THREAD    "stelnet: enable thread sanitizer"    OFF)
option(STELNET_SANITIZE_ADDRESS   "stelnet: enable address sanitizer"   OFF)
option(STELNET_SANITIZE_UNDEFINED "stelnet: enable undefined sanitizer" OFF)

# extra artifacts
option(STELNET_BUILD_TESTS    "stelnet: build tests"          ${STELNET_STANDALONE})
option(STELNET_BUILD_EXAMPLES "stelnet: build examples"       ${STELNET_STANDALONE})
option(STELNET_BUILD_SERVER   "stelnet: build server example" ${STELNET_STANDALONE})

# 3rd party libs
option(STELNET_CURL "stelnet: use libcurl to download model from an URL" OFF)
option(STELNET_SDL2 "stelnet: support for libSDL2" OFF)

if (STELNET_CURL)
    find_package(CURL REQUIRED)
    message(STATUS "libcurl found: ${CURL_VERSION_STRING} (${CURL_LIBRARIES})")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    option(STELNET_FFMPEG "stelnet: support building and linking with ffmpeg libs (avcodec, swresample, ...)" OFF)
endif()

option(STELNET_COREML                "stelnet: enable Core ML framework"  OFF)
option(STELNET_COREML_ALLOW_FALLBACK "stelnet: allow non-CoreML fallback" OFF)
option(STELNET_OPENVINO              "stelnet: support for OpenVINO"      OFF)

# ---------------------------------------------------------------------------
# Cohere Transcribe options
# ---------------------------------------------------------------------------
# COHERE_MKL: route both ggml's GEMM and the Cohere mel filterbank through
# Intel MKL. This typically gives a 1.5-2× speedup of the F16 / quantised
# matmul paths on x86_64 (Cascade Lake / Ice Lake / Zen4) by using MLAS-class
# INT8/F32 kernels with VNNI. Default OFF — turning it on adds a ~200 MB MKL
# runtime dependency. Recommended only for x86_64 server deployments.
option(COHERE_MKL "cohere: link Intel MKL for ggml GEMM + mel filterbank" OFF)

# Required for relocatable CMake package
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build-info.cmake)

# Bump tensor name limit for models with long hierarchical names (e.g.
# Pocket TTS Mimi decoder transformer: 71 chars). Must be set before
# add_subdirectory(ggml) so the definition propagates to all targets.
add_compile_definitions(GGML_MAX_NAME=128)

# MSVC's <cmath> doesn't expose M_PI unless _USE_MATH_DEFINES is set.
# NOMINMAX prevents <windows.h> from defining min/max macros that conflict
# with std::min/std::max (#152). Both are defined globally so all targets
# (src, tests, examples) get them on Windows.
if (WIN32)
    add_compile_definitions(_USE_MATH_DEFINES NOMINMAX)
endif()

# Wire COHERE_MKL into ggml's BLAS backend before add_subdirectory(ggml).
# This must happen here so the values are visible to the ggml subproject.
if (COHERE_MKL)
    set(GGML_BLAS        ON  CACHE BOOL "ggml: use BLAS"               FORCE)
    set(GGML_BLAS_VENDOR "Intel10_64lp" CACHE STRING "ggml: BLAS vendor" FORCE)
    # Hint our own find_package(BLAS) call (in src/CMakeLists.txt) toward MKL too.
    set(BLA_VENDOR       "Intel10_64lp")
    message(STATUS "COHERE_MKL=ON: enabling GGML_BLAS with vendor Intel10_64lp")
endif()

# override ggml options
set(GGML_SANITIZE_THREAD    ${STELNET_SANITIZE_THREAD})
set(GGML_SANITIZE_ADDRESS   ${STELNET_SANITIZE_ADDRESS})
set(GGML_SANITIZE_UNDEFINED ${STELNET_SANITIZE_UNDEFINED})
set(GGML_ALL_WARNINGS       ${STELNET_ALL_WARNINGS})
set(GGML_FATAL_WARNINGS     ${STELNET_FATAL_WARNINGS})

# transition helpers
function (stelnet_option_depr TYPE OLD NEW)
    if (${OLD})
        message(${TYPE} "${OLD} is deprecated and will be removed in the future.\nUse ${NEW} instead\n")
        set(${NEW} ON)
    endif()
endfunction()

stelnet_option_depr(FATAL_ERROR STELNET_CUBLAS              GGML_CUDA)
stelnet_option_depr(WARNING     STELNET_CUDA                GGML_CUDA)
stelnet_option_depr(WARNING     STELNET_KOMPUTE             GGML_KOMPUTE)
stelnet_option_depr(WARNING     STELNET_METAL               GGML_METAL)
stelnet_option_depr(WARNING     STELNET_METAL_EMBED_LIBRARY GGML_METAL_EMBED_LIBRARY)
stelnet_option_depr(WARNING     STELNET_NATIVE              GGML_NATIVE)
stelnet_option_depr(WARNING     STELNET_OPENMP              GGML_OPENMP)
stelnet_option_depr(WARNING     STELNET_RPC                 GGML_RPC)
stelnet_option_depr(WARNING     STELNET_SYCL                GGML_SYCL)
stelnet_option_depr(WARNING     STELNET_SYCL_F16            GGML_SYCL_F16)
stelnet_option_depr(WARNING     STELNET_CCACHE              GGML_CCACHE)

if (GGML_CUDA AND NOT MSVC)
    #GGML_CUDA enabled, add the necessary compile options -Wno-deprecated-gpu-targets
    add_compile_options(-Wno-deprecated-gpu-targets)
endif()

#
# build the library
#

if (NOT TARGET ggml)
    if (STELNET_USE_SYSTEM_GGML)
        find_package(ggml REQUIRED)
        if (NOT ggml_FOUND)
            message(FATAL_ERROR "System-installed GGML library not found.")
        endif()
        add_library(ggml ALIAS ggml::ggml)
    else()
        add_subdirectory(ggml)
        if(WIN32)
            # The following adds a _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR macro and is a workaround for
            # the Windows C++ standard library which does not support constexpr mutexes.
            # From the release notes://github.com/microsoft/STL/wiki/Changelog
            #  Disable constexpr mutex constructor on Windows
            #  Fixed mutex's constructor to be constexpr. #3824 #4000 #4339
            #  Note: Programs that aren't following the documented restrictions on binary compatibility may encounter
            #  null dereferences in mutex machinery. You must follow this rule:
            #  When you mix binaries built by different supported versions of the toolset, the Redistributable version
            #  must be at least as new as the latest toolset used by any app component.
            #  You can define _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR as an escape hatch.
            #
            # Specifically to stelnet this would cause a crash when using the Java bindings.
            # resulting in a Invalid memory access error.
            target_compile_definitions(ggml-base PRIVATE _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
        endif()
    endif()
    # ... otherwise assume ggml is added by a parent CMakeLists.txt
endif()
add_subdirectory(src)

# ---------------------------------------------------------------------------
# WASM / JS bindings (Emscripten only, all backends)
# ---------------------------------------------------------------------------
if(EMSCRIPTEN AND STELNET_WASM)
    add_subdirectory(bindings/javascript)
endif()

#
# install
#

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(STELNET_BUILD_NUMBER        ${BUILD_NUMBER})
set(STELNET_BUILD_COMMIT        ${BUILD_COMMIT})
set(STELNET_INSTALL_VERSION     ${CMAKE_PROJECT_VERSION})

set(STELNET_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Location of header  files")
set(STELNET_LIB_INSTALL_DIR     ${CMAKE_INSTALL_LIBDIR}     CACHE PATH "Location of library files")
set(STELNET_BIN_INSTALL_DIR     ${CMAKE_INSTALL_BINDIR}     CACHE PATH "Location of binary  files")

get_directory_property(STELNET_TRANSIENT_DEFINES COMPILE_DEFINITIONS)

set_target_properties(stelnet-lib PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/stelnet.h)
install(TARGETS stelnet-lib LIBRARY PUBLIC_HEADER)

target_compile_definitions(stelnet-lib PRIVATE
    STELNET_VERSION="${PROJECT_VERSION}"
)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stelnet-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/stelnet-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stelnet
    PATH_VARS
    STELNET_INCLUDE_INSTALL_DIR
    STELNET_LIB_INSTALL_DIR
    STELNET_BIN_INSTALL_DIR )

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/stelnet-version.cmake
    VERSION ${STELNET_INSTALL_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stelnet-config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/stelnet-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stelnet)

configure_file(cmake/stelnet.pc.in
        "${CMAKE_CURRENT_BINARY_DIR}/stelnet.pc"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/stelnet.pc"
        DESTINATION lib/pkgconfig)

#
# programs, examples and tests
#

add_subdirectory(stelnet_audio)

if (STELNET_BUILD_TESTS AND NOT CMAKE_JS_VERSION)
    include(CTest)
    add_subdirectory(tests)
endif ()

if (STELNET_BUILD_EXAMPLES)
    add_subdirectory(examples)
else()
    # The Ruby/Python/Rust/Dart bindings need the `common` static
    # library (audio I/O helpers etc.) but NOT the example executables
    # (cli, server) and not Catch2. examples/CMakeLists.txt has a
    # `if (STELNET_BUILD_COMMON_ONLY)` early-return that builds just
    # the common+common-stelnet libraries when STELNET_BUILD_EXAMPLES
    # is OFF. Always include it — the early-return is cheap when no
    # binding is consuming it.
    set(STELNET_BUILD_COMMON_ONLY ON)
    add_subdirectory(examples)
endif()

if (MSVC)
    set(MSVC_WARNING_FLAGS
        /wd4101  # Unreferenced local variable
        /wd4005  # Macro redefinition
        /wd4065  # switch statement contains 'default' but no 'case' labels
        /wd4267  # Conversion from 'size_t' to a smaller type, possible loss of data
        /wd4244  # Conversion from one type to another type, possible loss of ata
        /wd4805  # Unsafe mix of type
        /wd4305  # Truncation from 'type1' to 'type2' (often double to float)
        /wd4996  # Function or variable may be unsafe/deprecated
    )
    function(disable_msvc_warnings target_name)
        if(TARGET ${target_name})
            target_compile_options(${target_name} PRIVATE ${MSVC_WARNING_FLAGS})
        endif()
    endfunction()

    if (STELNET_BUILD_EXAMPLES)
        disable_msvc_warnings(stelnet-lib)
        disable_msvc_warnings(common)
        disable_msvc_warnings(common-sdl)
        disable_msvc_warnings(lsp)
        disable_msvc_warnings(wchess-core)
        disable_msvc_warnings(stelnet-command)
        disable_msvc_warnings(stelnet-cli)
        disable_msvc_warnings(stelnet-server)
        disable_msvc_warnings(stelnet-stream)
        disable_msvc_warnings(stelnet-talk-llama)
        disable_msvc_warnings(stelnet-bench)
        disable_msvc_warnings(quantize)
        disable_msvc_warnings(vad-speech-segments)
    endif()
endif()
