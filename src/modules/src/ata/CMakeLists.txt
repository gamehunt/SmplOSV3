cmake_minimum_required(VERSION 3.20)
MESSAGE(STATUS "Building module test")
set(CMAKE_C_FLAGS "-m64 -O0 -ffreestanding -z max-page-size=0x1000 -fno-pic -mno-red-zone -mno-mmx -mno-sse -std=gnu99 -mno-sse2 -Wall -Wextra")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_C_COMPILER "/usr/bin/x86_64-elf-gcc")
set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-elf")
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
enable_language(ASM_NASM)
enable_language(C)
include_directories(include ${CMAKE_CURRENT_SOURCE_DIR}/../../../../sysroot/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../sysroot/lib)
project(SmplOSV3-exo-module-test VERSION 1.0)
add_link_options(-r -ffreestanding -nostdlib -lgcc)
FILE(GLOB C_SRCS src/*.c src/**/*.c)
FILE(GLOB ASM_SRC src/*.asm src/**/*.asm)
ADD_EXECUTABLE(ata.smpm ${ASM_SRC} ${C_SRCS})
target_link_libraries(ata.smpm libk.a)
INSTALL(TARGETS ata.smpm DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../../initrd/)
INSTALL (
    DIRECTORY   ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_SOURCE_DIR}/../../../../sysroot/include/kernel
    FILES_MATCHING PATTERN "*.h*"
)
