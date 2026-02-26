# Copyright (c) 2026 Alan Blevins
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.12)
project(scene_rdl2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# nanobind requires Python 3.8+; Python 3.13 is used here.
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Threads REQUIRED)

# Read MoonRay install root from the environment (set by MoonRay's setup.sh / Rez).
if(NOT DEFINED ENV{REZ_MOONRAY_ROOT})
    message(FATAL_ERROR
        "REZ_MOONRAY_ROOT is not set.\n"
        "Source MoonRay's setup.sh (or activate the Rez environment) before running cmake.")
endif()
set(MOONRAY_ROOT    "$ENV{REZ_MOONRAY_ROOT}")
get_filename_component(MOONRAY_INSTALLS_DIR "${MOONRAY_ROOT}" DIRECTORY)
set(MOONRAY_INCLUDE "${MOONRAY_ROOT}/include")
set(MOONRAY_DEP_INC "${MOONRAY_INSTALLS_DIR}/include")   # boost, tbb, lua
set(MOONRAY_LIB     "${MOONRAY_ROOT}/lib")

# nanobind is a git submodule / sibling clone at this path.
set(NANOBIND_INCLUDE "/Users/alan/Projects/nanobind/include")

include_directories(
    ${MOONRAY_INCLUDE}
    ${MOONRAY_DEP_INC}
    ${NANOBIND_INCLUDE}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Build as a Python extension module (.so / .dylib).
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

# Standard flags for a Python extension module.
# NB_COMPACT_ASSERTIONS reduces nanobind binary size.
target_compile_options(scene_rdl2 PRIVATE
    -fvisibility=hidden
    -fPIC
    -DNB_COMPACT_ASSERTIONS
    # Old Boost (shipped with MoonRay) uses std::unary_function, removed in C++17.
    # This libc++ compat flag re-enables it on Apple platforms.
    -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
)

# Locate the MoonRay scene_rdl2 shared library without hard-coding the extension
# (.dylib on macOS, .so on Linux).
find_library(MOONRAY_SCENE_RDL2_LIB
    NAMES scene_rdl2
    PATHS "${MOONRAY_LIB}"
    NO_DEFAULT_PATH
    REQUIRED
)

# nanobind is header-only for our usage (we compile its core directly).
# Link the nanobind combined-source object.
add_library(nanobind_core OBJECT
    /Users/alan/Projects/nanobind/src/nb_combined.cpp
)
target_include_directories(nanobind_core PRIVATE
    ${NANOBIND_INCLUDE}
    /Users/alan/Projects/nanobind/ext/robin_map/include
    ${Python3_INCLUDE_DIRS}
)
target_compile_options(nanobind_core PRIVATE
    -fvisibility=hidden
    -fPIC
    -std=c++17
    -DNB_COMPACT_ASSERTIONS
)

# Link against scene_rdl2 and threads; do NOT link libpython (Python extension
# modules must leave Python symbols unresolved so the host interpreter provides them).
target_link_libraries(scene_rdl2 PRIVATE
    Threads::Threads
    ${MOONRAY_SCENE_RDL2_LIB}
    $<TARGET_OBJECTS:nanobind_core>
)
# -undefined dynamic_lookup is Apple ld only; Linux resolves Python symbols at
# runtime via the embedding interpreter without any extra flag.
if(APPLE)
    target_link_libraries(scene_rdl2 PRIVATE "-undefined dynamic_lookup")
endif()

# Install the module
install(TARGETS scene_rdl2 DESTINATION .)
