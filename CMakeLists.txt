# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.12)
project(scene_rdl2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use Python 3.13 – pybind11 2.13.6 (MoonRay's bundled version) supports it.
# Python 3.14 removed private APIs that pybind11 2.13.6 relied on.
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Threads REQUIRED)

# Read MoonRay install root from the environment (set by MoonRay's setup.sh / Rez).
# MoonRay ships pybind11 2.13.6 alongside its own headers; we use that exclusively
# rather than any separately installed pybind11.
if(NOT DEFINED ENV{REZ_MOONRAY_ROOT})
    message(FATAL_ERROR
        "REZ_MOONRAY_ROOT is not set.\n"
        "Source MoonRay's setup.sh (or activate the Rez environment) before running cmake.")
endif()
set(MOONRAY_ROOT    "$ENV{REZ_MOONRAY_ROOT}")
get_filename_component(MOONRAY_INSTALLS_DIR "${MOONRAY_ROOT}" DIRECTORY)
set(MOONRAY_INCLUDE "${MOONRAY_ROOT}/include")
set(MOONRAY_DEP_INC "${MOONRAY_INSTALLS_DIR}/include")   # boost, tbb, lua, pybind11
set(MOONRAY_LIB     "${MOONRAY_ROOT}/lib")

include_directories(
    ${MOONRAY_INCLUDE}
    ${MOONRAY_DEP_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Build as a Python extension module (.so / .dylib).
# We replicate what pybind11_add_module() does, minus the homebrew dependency.
add_library(scene_rdl2 MODULE
    src/module.cpp
    src/bind_math.cpp
    src/bind_types.cpp
    src/bind_attribute.cpp
    src/bind_scene_object.cpp
    src/bind_scene_variables.cpp
    src/bind_node.cpp
    src/bind_light.cpp
    src/bind_shaders.cpp
    src/bind_sets.cpp
    src/bind_layer.cpp
    src/bind_render_output.cpp
    src/bind_scene_context.cpp
    src/bind_io.cpp
)

# Python headers
target_include_directories(scene_rdl2 PRIVATE ${Python3_INCLUDE_DIRS})

# Extension module naming convention: scene_rdl2.cpython-XY-darwin.so
set_target_properties(scene_rdl2 PROPERTIES
    PREFIX ""
    SUFFIX ".${Python3_SOABI}.so"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    BUILD_RPATH "${MOONRAY_LIB};${MOONRAY_INSTALLS_DIR}/lib"
    INSTALL_RPATH "${MOONRAY_LIB};${MOONRAY_INSTALLS_DIR}/lib"
)

# Standard flags for a Python extension module
target_compile_options(scene_rdl2 PRIVATE
    -fvisibility=hidden
    -fPIC
)

# Locate the MoonRay scene_rdl2 shared library without hard-coding the extension
# (.dylib on macOS, .so on Linux).
find_library(MOONRAY_SCENE_RDL2_LIB
    NAMES scene_rdl2
    PATHS "${MOONRAY_LIB}"
    NO_DEFAULT_PATH
    REQUIRED
)

# Link against scene_rdl2 and threads; do NOT link libpython (Python extension
# modules must leave Python symbols unresolved so the host interpreter provides them).
target_link_libraries(scene_rdl2 PRIVATE
    Threads::Threads
    ${MOONRAY_SCENE_RDL2_LIB}
)
# -undefined dynamic_lookup is Apple ld only; Linux resolves Python symbols at
# runtime via the embedding interpreter without any extra flag.
if(APPLE)
    target_link_libraries(scene_rdl2 PRIVATE "-undefined dynamic_lookup")
endif()

# Install the module
install(TARGETS scene_rdl2 DESTINATION .)
