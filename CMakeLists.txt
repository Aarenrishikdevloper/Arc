cmake_minimum_required(VERSION 3.28)
project(arc LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Default to Release-like flags if no build type given
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(WARN_FLAGS -Wall -Wextra -Wpedantic)

# Gather sources (equivalent of the Makefile's $(wildcard ...))
file(GLOB SRC_MAIN     CONFIGURE_DEPENDS src/*.c)
file(GLOB SRC_PREVIEW  CONFIGURE_DEPENDS src/preview/*.c)
file(GLOB SRC_HELPS    CONFIGURE_DEPENDS help/*.c)
set(SOURCES ${SRC_MAIN} ${SRC_PREVIEW} ${SRC_HELPS})

# Output to bin/ like the Makefile
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Release target: bin/arc ---
add_executable(arc ${SOURCES})
target_compile_options(arc PRIVATE ${WARN_FLAGS})

# --- Debug target: bin/arc_debug (with sanitizers) ---
add_executable(arc_debug ${SOURCES})
target_compile_options(arc_debug PRIVATE ${WARN_FLAGS} -g -fsanitize=address,undefined)
target_link_options(arc_debug PRIVATE -fsanitize=address,undefined)