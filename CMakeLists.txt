cmake_minimum_required(VERSION 3.16)
project(muvc)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.5.0
)

FetchContent_MakeAvailable(CLI11)

include_directories(inc)
include_directories(src/commands)

file(GLOB SOURCES
        src/*.cpp
        src/commands/*.cpp
)

add_executable(muvc ${SOURCES}
        src/commands/about.cpp
        src/commands/about.hpp
        src/commands/stage.cpp
        src/commands/stage.hpp
        src/commands/snap.cpp
        src/commands/snap.hpp
        src/utils/hash.cpp
        src/utils/hash.hpp
        src/commands/list.cpp
        src/commands/list.hpp)

target_link_libraries(muvc CLI11::CLI11)
