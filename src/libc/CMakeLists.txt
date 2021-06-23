cmake_minimum_required(VERSION 3.20)

if(${BUILD_TYPE} MATCHES "libc")
    MESSAGE(STATUS "Starting libc build")
elseif(${BUILD_TYPE} MATCHES "libk")
    MESSAGE(STATUS "Starting libk build")
else()
    MESSAGE(FATAL_ERROR "Invalid build_type specified: ${BUILD_TYPE}")
endif()
if(${BUILD_TYPE} MATCHES "libk")
    set(CMAKE_C_FLAGS "-m64 -O0 -ffreestanding -z max-page-size=0x1000 -mno-red-zone -mno-mmx -std=gnu99 -mno-sse2 -Wall -Wextra")
    set(CMAKE_C_COMPILER "/usr/bin/x86_64-elf-gcc")
    set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-elf")
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
endif()
enable_language(ASM_NASM)
enable_language(C)
include_directories(include ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
project(SmplOSV3-exo-libc VERSION 1.0)
FILE(GLOB C_SRCS src/*.c src/**/*.c)
FILE(GLOB ASM_SRC src/*.asm)
if(${BUILD_TYPE} MATCHES "libk")
    add_link_options(-ffreestanding -nostdlib -lgcc)
    add_definitions(-DK_SMPLOS_LIBK)
    ADD_LIBRARY(k STATIC ${C_SRCS})
    INSTALL(
    TARGETS   k
    DESTINATION ${CMAKE_SOURCE_DIR}/../../lib
    )
endif()
INSTALL(
    DIRECTORY   ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_SOURCE_DIR}/../../include
    FILES_MATCHING PATTERN "*.h*")