cmake_minimum_required(VERSION 3.20)

project(SmplOSV3-exo VERSION 1.0)
include(ExternalProject)

MESSAGE(STATUS "Cleaning...")

execute_process(
    COMMAND rm -rf build
    COMMAND rm -rf sysroot
    COMMAND rm -rf initrd
    COMMAND mkdir  initrd
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    ERROR_QUIET
)

ExternalProject_Add(loader 
    PREFIX     ${CMAKE_SOURCE_DIR}/src/loader
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/loader
    BINARY_DIR ${CMAKE_BINARY_DIR}/loader
    TMP_DIR    ${CMAKE_BINARY_DIR}/loader
    STAMP_DIR  ${CMAKE_BINARY_DIR}/loader
)

execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/loader
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/loader
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/loader --target install
)

ExternalProject_Add(libk
    PREFIX     ${CMAKE_SOURCE_DIR}/src/libc
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libc
    BINARY_DIR ${CMAKE_BINARY_DIR}/libk
    TMP_DIR    ${CMAKE_BINARY_DIR}/libk
    STAMP_DIR  ${CMAKE_BINARY_DIR}/libk
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE:STRING=libk ${CMAKE_SOURCE_DIR}/src/libc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libk
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/libk --target install
)

ExternalProject_Add(libc
    PREFIX     ${CMAKE_SOURCE_DIR}/src/libc
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libc
    BINARY_DIR ${CMAKE_BINARY_DIR}/libc
    TMP_DIR    ${CMAKE_BINARY_DIR}/libc
    STAMP_DIR  ${CMAKE_BINARY_DIR}/libc
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE:STRING=libc ${CMAKE_SOURCE_DIR}/src/libc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libc
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/libc --target install
)


ExternalProject_Add(kernel
    PREFIX     ${CMAKE_SOURCE_DIR}/src/kernel
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/kernel
    BINARY_DIR ${CMAKE_BINARY_DIR}/kernel
    TMP_DIR    ${CMAKE_BINARY_DIR}/kernel
    STAMP_DIR  ${CMAKE_BINARY_DIR}/kernel
)

execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/kernel
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/kernel
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/kernel --target install
)

ExternalProject_Add(modules
    PREFIX     ${CMAKE_SOURCE_DIR}/src/modules
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/modules
    BINARY_DIR ${CMAKE_BINARY_DIR}/modules
    TMP_DIR    ${CMAKE_BINARY_DIR}/modules
    STAMP_DIR  ${CMAKE_BINARY_DIR}/modules
)

execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/modules
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/modules
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/modules
)

MESSAGE(STATUS "Creating InitRD")

MESSAGE(STATUS "Cleaning...")
execute_process(
    COMMAND rm ramdisk.initrd
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    ERROR_QUIET
)

MESSAGE(STATUS "Generating...")
execute_process(
    COMMAND tar cvf ramdisk.initrd initrd/
    COMMAND cp  ramdisk.initrd     /mnt/smplos/boot
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

MESSAGE(STATUS "Updating GRUB config")

execute_process(
    COMMAND cp  grub.cfg  /mnt/smplos/boot/grub/grub.cfg
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

