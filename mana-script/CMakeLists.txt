cmake_minimum_required(VERSION 3.10)
project(Manascript VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/transpiler.cpp
    src/error.cpp
    src/symbol_table.cpp
    src/token.cpp
)

# Main executable
add_executable(manascript ${SOURCES})

# Add tests
enable_testing()
add_subdirectory(tests)

# Install
install(TARGETS manascript DESTINATION bin)