cmake_minimum_required(VERSION 3.10)

project(CerberusCompiler)

# Set the source files for the compiler
file(GLOB_RECURSE COMPILER_SOURCES
    src/*.cpp
    Preambles/*.cpp
)

file(GLOB_RECURSE COMPILER_HEADERS
    headers/*.h
    Preambles/*.h
)

include_directories(headers)
include_directories(Preambles)

# Create an executable target for the compiler
add_executable(cerberus ${COMPILER_SOURCES} ${COMPILER_HEADERS})