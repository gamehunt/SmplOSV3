cmake_minimum_required(VERSION 3.20)
MESSAGE(STATUS "Starting native subkernel build")
set(CMAKE_C_FLAGS "-m64 -O0 -ffreestanding -fno-pic -z max-page-size=0x1000 -mno-red-zone -mno-mmx -mno-sse -std=gnu99 -mno-sse2 -Wall -Wextra")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_C_COMPILER "/usr/bin/x86_64-elf-gcc")
set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-elf")
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
enable_language(ASM_NASM)
enable_language(C)
include_directories(include ${CMAKE_CURRENT_SOURCE_DIR}/../../../../sysroot/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../sysroot/lib)
project(SmplOSV3-exo-native-subkernel VERSION 1.0)
add_link_options(-ffreestanding -nostdlib -lgcc)
add_definitions(-DK_SMPLOS_NATIVE_SUBKERNEL)
FILE(GLOB C_SRCS src/*.c src/**/*.c)
FILE(GLOB INIT_C_SRCS init/*.c)
FILE(GLOB ASM_SRC src/*.asm src/**/*.asm)
ADD_EXECUTABLE(native_init.smpinit ${INIT_C_SRCS})
target_link_options(native_init.smpinit PUBLIC -T ${CMAKE_SOURCE_DIR}/init/link.ld)
ADD_LIBRARY(native.subkernel STATIC ${C_SRCS})
INSTALL(
    TARGETS   native.subkernel
    DESTINATION ${CMAKE_SOURCE_DIR}/../../../../sysroot/lib
)
INSTALL(
    DIRECTORY   ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_SOURCE_DIR}/../../../../sysroot/include/subkernels/native/
    FILES_MATCHING PATTERN "*.h*")
INSTALL(TARGETS native_init.smpinit DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../../initrd/)