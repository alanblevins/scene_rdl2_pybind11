# scene_rdl2_pybind11

Python bindings for the scene_rdl2 library using pybind11.

## Overview

This project provides Python bindings for the scene_rdl2 library, which is part of the DreamWorks MoonRay rendering system. The bindings allow Python scripts to interact with RDL scene data structures, including SceneContext, SceneObjects, and SceneClasses.

## Requirements

To build this project, you need:

1. **C++17 compatible compiler** (clang or gcc)
2. **CMake 3.12+**
3. **Python 3.x** with development headers
4. **pybind11** (installed via package manager or pip)
5. **scene_rdl2 library** installed in `/Applications/MoonRay/installs/openmoonray/include/`

The scene_rdl2 library has additional dependencies that must be installed separately:
- log4cplus
- tbb (Threading Building Blocks)
- boost
- other system libraries

## Building

To build the project:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```python
import scene_rdl2_pybind11 as rdl2

# Create a scene context
context = rdl2.SceneContext()

# Create a scene class
scene_class = context.createSceneClass("Teapot")

# Create a scene object
scene_object = context.createSceneObject("Teapot", "/test/teapot")

# Retrieve an object
retrieved_object = context.getSceneObject("/test/teapot")
print(f"Object name: {retrieved_object.getName()}")
```

## Supported Features

- SceneContext creation and management
- SceneClass creation and retrieval
- SceneObject creation and retrieval
- Basic object information access

## Troubleshooting

If you encounter build errors, ensure that:
1. The scene_rdl2 library is properly installed in the expected location
2. All required dependencies are installed
3. The correct compiler and CMake versions are being used

## License

This project is licensed under the Apache 2.0 license.