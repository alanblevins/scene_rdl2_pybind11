// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for the shader class hierarchy:
//   Shader -> RootShader -> Material, Displacement, VolumeShader
//   Shader -> Map, NormalMap

#include "bindings.h"

void bind_shaders(py::module_& m)
{
    // These classes don't expose additional Python methods beyond what they
    // inherit from SceneObject; the bindings exist for type identification
    // and safe downcasting.
    py::class_<rdl2::Shader,      rdl2::SceneObject>(m, "Shader");
    py::class_<rdl2::RootShader,  rdl2::Shader>     (m, "RootShader");
    py::class_<rdl2::Material,    rdl2::RootShader>  (m, "Material");
    py::class_<rdl2::Displacement,rdl2::RootShader>  (m, "Displacement");
    py::class_<rdl2::VolumeShader,rdl2::RootShader>  (m, "VolumeShader");
    py::class_<rdl2::Map,         rdl2::Shader>      (m, "Map");
    py::class_<rdl2::NormalMap,   rdl2::Shader>      (m, "NormalMap");
}
