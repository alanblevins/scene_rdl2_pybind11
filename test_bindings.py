#!/usr/bin/env python3
"""
Simple test script to verify scene_rdl2 bindings work correctly.
"""

import sys
import os

# Add the build directory to the path so we can import the module
build_dir = os.path.join(os.path.dirname(__file__), 'build')
if os.path.exists(build_dir):
    sys.path.insert(0, build_dir)

try:
    import scene_rdl2 as rdl2
    print("Successfully imported scene_rdl2 module")

    # Test creating a SceneContext
    context = rdl2.SceneContext()
    print("Created SceneContext successfully")

    # Test creating a scene class
    scene_class = context.createSceneClass("Teapot")
    print("Created scene class successfully")

    # Test creating a scene object
    scene_object = context.createSceneObject("Teapot", "/test/teapot")
    print("Created scene object successfully")

    # Test getting the scene object
    retrieved_object = context.getSceneObject("/test/teapot")
    print("Retrieved scene object successfully")

    # Test object name
    print(f"Object name: {retrieved_object.getName()}")

    print("All tests passed!")

except ImportError as e:
    print(f"Failed to import module: {e}")
    print("Make sure you have built the project first with 'cmake .. && make'")
    sys.exit(1)
except Exception as e:
    print(f"Error during testing: {e}")
    sys.exit(1)