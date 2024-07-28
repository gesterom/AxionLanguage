cmake_minimum_required(VERSION 3.10)

project(Examples)

# Specify the source file written in your custom language
set(MYLANG_SOURCE example1.dog)

# Custom command to invoke the compiler on the source file
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example1.exe
    COMMAND ${CMAKE_BINARY_DIR}/core/cerberus ${CMAKE_CURRENT_SOURCE_DIR}/${MYLANG_SOURCE} -o ${CMAKE_CURRENT_BINARY_DIR}/example1.exe
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MYLANG_SOURCE} ${CMAKE_BINARY_DIR}/core/cerberus
    COMMENT "Compiling example1.mylang using cerberus"
)

# Custom target to run the compiled program
add_custom_target(run_example1
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/example1.exe
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example1.exe
    COMMENT "Running the compiled example1 program"
)