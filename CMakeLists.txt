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
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/loader
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
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/kernel
)