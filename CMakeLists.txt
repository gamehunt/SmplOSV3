cmake_minimum_required(VERSION 3.20)

project(SmplOSV3-exo VERSION 1.0)
include(ExternalProject)

ExternalProject_Add(loader 
    PREFIX     ${CMAKE_SOURCE_DIR}/src/loader
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/loader
    BINARY_DIR ${CMAKE_BINARY_DIR}/loader
    TMP_DIR    ${CMAKE_BINARY_DIR}/loader-tmp
    STAMP_DIR  ${CMAKE_BINARY_DIR}/loader-stamp
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
    TMP_DIR    ${CMAKE_BINARY_DIR}/libk-tmp
    STAMP_DIR  ${CMAKE_BINARY_DIR}/libk-stamp
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
    TMP_DIR    ${CMAKE_BINARY_DIR}/libc-tmp
    STAMP_DIR  ${CMAKE_BINARY_DIR}/libc-stamp
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
    TMP_DIR    ${CMAKE_BINARY_DIR}/kernel-tmp
    STAMP_DIR  ${CMAKE_BINARY_DIR}/kernel-stamp
)

execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/kernel
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/kernel
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/kernel --target install
)

