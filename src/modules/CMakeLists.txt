cmake_minimum_required(VERSION 3.20)

MESSAGE(STATUS "Building modules...")

project(SmplOSV3-exo-modules VERSION 1.0)
include(ExternalProject)

ExternalProject_Add(test
    PREFIX     ${CMAKE_SOURCE_DIR}/src/test
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/test
    BINARY_DIR ${CMAKE_BINARY_DIR}/test
    TMP_DIR    ${CMAKE_BINARY_DIR}/test
    STAMP_DIR  ${CMAKE_BINARY_DIR}/test
)

execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
)

execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/test --target install
)

ExternalProject_Add(ata
    PREFIX     ${CMAKE_SOURCE_DIR}/src/ata
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/ata
    BINARY_DIR ${CMAKE_BINARY_DIR}/ata
    TMP_DIR    ${CMAKE_BINARY_DIR}/ata
    STAMP_DIR  ${CMAKE_BINARY_DIR}/ata
)

execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/ata
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ata
)

execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/ata --target install
)