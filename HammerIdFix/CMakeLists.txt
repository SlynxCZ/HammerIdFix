cmake_minimum_required(VERSION 3.18)
project(HammerIdFix C CXX ASM)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(GLOB_RECURSE SOURCE_FILES
        src/*.cpp
        src/*.h
        src/*.hpp
)

list(APPEND SOURCE_FILES
        ../vendor/hl2sdk-cs2/public/tier0/memoverride.cpp
        ../vendor/hl2sdk-cs2/tier1/generichash.cpp
        ../vendor/hl2sdk-cs2/tier1/keyvalues3.cpp
        ../vendor/hl2sdk-cs2/tier1/convar.cpp
        ../vendor/hl2sdk-cs2/entity2/entityidentity.cpp
        ../vendor/hl2sdk-cs2/entity2/entitysystem.cpp
        ../vendor/hl2sdk-cs2/entity2/entitykeyvalues.cpp
        ../vendor/hl2sdk-cs2/interfaces/interfaces.cpp
        ../vendor/metamod-source/core/sourcehook/sourcehook.cpp
        ../vendor/metamod-source/core/sourcehook/sourcehook_impl_chookidman.cpp
        ../vendor/metamod-source/core/sourcehook/sourcehook_impl_chookmaninfo.cpp
        ../vendor/metamod-source/core/sourcehook/sourcehook_impl_cvfnptr.cpp
        ../vendor/metamod-source/core/sourcehook/sourcehook_impl_cproto.cpp
        ../vendor/nlohmann/json.hpp
        src/cs2_sdk/interfaces/cs2_interfaces.cpp
        src/cs2_sdk/interfaces/cs2_interfaces.h
        src/cs2_sdk/interfaces/cgameresourceserviceserver.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${NATIVES_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cs2_sdk
)

if (LINUX)
    include(${CMAKE_SOURCE_DIR}/makefiles/linux.base.cmake)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            PREFIX ""
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/HammerIdFix/bin/linuxsteamrt64"
    )
elseif (WIN32)
    include(${CMAKE_SOURCE_DIR}/makefiles/windows.base.cmake)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            PREFIX ""
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/HammerIdFix/bin/win64"
    )
endif()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/configs ${CMAKE_BINARY_DIR}
)