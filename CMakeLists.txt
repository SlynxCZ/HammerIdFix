cmake_minimum_required(VERSION 3.18)
project(FUNPLAY-CS2 LANGUAGES C CXX ASM)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(makefiles/shared.cmake)
include(makefiles/protobuf.cmake)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/dyncall)
add_subdirectory(vendor/funchook)

add_subdirectory(HammerIdFix)
add_subdirectory(DemoVoiceFix)
