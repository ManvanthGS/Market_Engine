cmake_minimum_required(VERSION 3.20)
project(market_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories and source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/*.hpp)

add_library(market_engine STATIC
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_include_directories(market_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
add_subdirectory(benchmarks)
add_subdirectory(examples)
