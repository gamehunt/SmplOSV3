cmake_minimum_required(VERSION 3.20)

MESSAGE(STATUS "Building subkernels...")

project(SmplOSV3-exo-subkernels VERSION 1.0)
include(ExternalProject)

ExternalProject_Add(native
    PREFIX     ${CMAKE_SOURCE_DIR}/src/native
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/native
    BINARY_DIR ${CMAKE_BINARY_DIR}/native
    TMP_DIR    ${CMAKE_BINARY_DIR}/native
    STAMP_DIR  ${CMAKE_BINARY_DIR}/native
)

execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/native
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/native
)

execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/native --target install
)