cmake_minimum_required(VERSION 3.10)
project(backbencher)

file(GLOB SOURCES "src/**/*.c")
file(GLOB SRC_FILES "src/*.c")
set(SOURCES ${SOURCES} ${SRC_FILES})

if(SOURCES)
    add_executable(backbencher ${SOURCES})
else()
    message(FATAL_ERROR "No .c files found in source")
endif()

target_compile_options(backbencher PRIVATE -Ofast)
