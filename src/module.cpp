// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for the scene_rdl2 library — module entry point.
// All class/enum registrations live in the accompanying bind_*.cpp files.

#include "bindings.h"

NB_MODULE(scene_rdl2, m) {
    m.doc() = "Python bindings for the scene_rdl2 library";

    // Registration order matters: base classes must precede derived classes.
    bind_math(m);
    bind_types(m);
    bind_attribute(m);       // Attribute, SceneClass
    bind_scene_object(m);    // SceneObject, UpdateGuard
    bind_scene_variables(m); // SceneVariables
    bind_node(m);            // Node, Camera, Geometry
    bind_light(m);           // Light
    bind_shaders(m);         // Shader -> RootShader -> Material/Displacement/VolumeShader/Map/NormalMap
    bind_sets(m);            // GeometrySet, LightSet
    bind_layer(m);           // LayerAssignment, Layer
    bind_render_output(m);   // RenderOutput (+ nested enums)
    bind_scene_context(m);   // SceneContext
    bind_io(m);              // AsciiReader, AsciiWriter, free functions
}
