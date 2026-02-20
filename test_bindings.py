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

DSO_PATH = "/Applications/MoonRay/installs/openmoonray/rdl2dso"

try:
    import scene_rdl2 as rdl2
    print("Successfully imported scene_rdl2 module")

    # Test creating a SceneContext (proxy mode so we don't need full MoonRay pipeline)
    context = rdl2.SceneContext()
    context.setProxyModeEnabled(True)
    context.setDsoPath(DSO_PATH)
    print("Created SceneContext successfully")

    # Test creating a scene class
    scene_class = context.createSceneClass("BoxGeometry")
    print("Created scene class successfully")

    # Test creating a scene object
    scene_object = context.createSceneObject("BoxGeometry", "/test/box")
    print("Created scene object successfully")

    # Test getting the scene object
    retrieved_object = context.getSceneObject("/test/box")
    print("Retrieved scene object successfully")

    # Test object name
    print(f"Object name: {retrieved_object.getName()}")

    # Test loading all DSOs via proxy mode
    print("\n--- DSO loading test ---")
    dso_context = rdl2.SceneContext()
    dso_context.setProxyModeEnabled(True)
    dso_context.setDsoPath(DSO_PATH)
    dso_context.loadAllSceneClasses()
    all_classes = dso_context.getAllSceneClasses()
    print(f"Scene classes loaded: {len(all_classes)}")

    print("\nAll tests passed!")

except ImportError as e:
    print(f"Failed to import module: {e}")
    print("Make sure you have built the project first with 'cmake .. && make'")
    sys.exit(1)
except Exception as e:
    print(f"Error during testing: {e}")
    sys.exit(1)
