set(TARGET stelnet-legacy-quantize)
add_executable(${TARGET} quantize.cpp)

include(DefaultTargetOptions)

target_link_libraries(${TARGET} PRIVATE common stelnet-lib ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS ${TARGET} RUNTIME BUNDLE DESTINATION bin)
