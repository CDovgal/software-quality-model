cmake_minimum_required(VERSION 3.0)

project(bayeslib)
set(LIBRARY_NAME bayeslib)

include_directories(
	include
)

# Collect sources

file(
    GLOB
    source_files
    src/*.c*
)

file(
    GLOB
    include_files
    include/*.h*
)

# Assign sources to binaries

add_library(
    ${LIBRARY_NAME}
    SHARED
    ${source_files}
    ${include_files}
)

# Install

INSTALL(TARGETS ${LIBRARY_NAME}
    ARCHIVE
    DESTINATION lib
    COMPONENT Libraries
)

INSTALL(TARGETS ${LIBRARY_NAME}
    RUNTIME
    DESTINATION bin
    COMPONENT Libraries
)