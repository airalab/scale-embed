cmake_minimum_required(VERSION 3.13)
project(scale_embed C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(SCALE_EMBED_BUILD_TESTS "Build scale-embed unit tests" ON)
option(SCALE_EMBED_BUILD_EXAMPLES "Build scale-embed examples" ON)
option(SCALE_EMBED_STRICT_WARNINGS "Enable strict host warnings" ON)

add_library(scale-embed
    src/writer.c
    src/reader.c
    src/primitive.c
    src/compact.c
    src/bytes.c
    src/option.c
    src/result.c
    src/variant.c
)

target_include_directories(scale-embed
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(SCALE_EMBED_STRICT_WARNINGS AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"))
    target_compile_options(scale-embed PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
    )
endif()

if(SCALE_EMBED_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SCALE_EMBED_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
