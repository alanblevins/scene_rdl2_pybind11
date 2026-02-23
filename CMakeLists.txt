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

# MoonRay ships pybind11 2.13.6 at /Applications/MoonRay/installs/include.
# We use that exclusively (option 4) rather than the homebrew pybind11 3.0.1.
set(MOONRAY_INCLUDE  /Applications/MoonRay/installs/openmoonray/include)
set(MOONRAY_DEP_INC  /Applications/MoonRay/installs/include)   # boost, tbb, lua, pybind11
set(MOONRAY_LIB      /Applications/MoonRay/installs/openmoonray/lib)

include_directories(
    ${MOONRAY_INCLUDE}
    ${MOONRAY_DEP_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Build as a Python extension module (.so / .dylib).
# We replicate what pybind11_add_module() does, minus the homebrew dependency.
add_library(scene_rdl2 MODULE src/module.cpp)

# Python headers
target_include_directories(scene_rdl2 PRIVATE ${Python3_INCLUDE_DIRS})

# Extension module naming convention: scene_rdl2.cpython-XY-darwin.so
set_target_properties(scene_rdl2 PROPERTIES
    PREFIX ""
    SUFFIX ".${Python3_SOABI}.so"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/scene_rdl2
    BUILD_RPATH "${MOONRAY_LIB};/Applications/MoonRay/installs/lib"
    INSTALL_RPATH "${MOONRAY_LIB};/Applications/MoonRay/installs/lib"
)

# Standard flags for a Python extension module
target_compile_options(scene_rdl2 PRIVATE
    -fvisibility=hidden
    -fPIC
)

# Link against scene_rdl2 and threads; do NOT link libpython (Python extension
# modules must leave Python symbols unresolved so the host interpreter provides them).
target_link_libraries(scene_rdl2 PRIVATE
    Threads::Threads
    ${MOONRAY_LIB}/libscene_rdl2.dylib
    "-undefined dynamic_lookup"
)

# Install the module
install(TARGETS scene_rdl2 DESTINATION .)
