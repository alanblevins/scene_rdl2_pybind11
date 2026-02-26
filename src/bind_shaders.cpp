// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for the shader class hierarchy:
//   Shader -> RootShader -> Material, Displacement, VolumeShader
//   Shader -> Map, NormalMap

#include "bindings.h"

// Helper macro for the downcasting __new__/__init__ pattern (same as bind_node.cpp).
#define DEF_DOWNCAST_CTOR(CLS, NM)                                          \
    .def_static("__new__",                                                  \
        [](py::handle type, rdl2::SceneObject* obj) -> py::object {        \
            auto* r = obj->asA<rdl2::CLS>();                                \
            if (!r) throw py::type_error(                                   \
                ("cannot cast '" + obj->getSceneClass().getName() +         \
                 "' to " NM).c_str());                                      \
            return py::inst_reference(type, r);                             \
        }, py::arg("type"), py::arg("scene_object"))                        \
    .def("__init__", [](py::object, rdl2::SceneObject*) {},                 \
         py::arg("scene_object"))

void bind_shaders(py::module_& m)
{
    // These classes don't expose additional Python methods beyond what they
    // inherit from SceneObject; the bindings exist for type identification,
    // safe downcasting, and constructor-based casting from SceneObject.

    py::class_<rdl2::Shader, rdl2::SceneObject>(m, "Shader")
        DEF_DOWNCAST_CTOR(Shader, "Shader");

    py::class_<rdl2::RootShader, rdl2::Shader>(m, "RootShader")
        DEF_DOWNCAST_CTOR(RootShader, "RootShader");

    py::class_<rdl2::Material, rdl2::RootShader>(m, "Material")
        DEF_DOWNCAST_CTOR(Material, "Material");

    py::class_<rdl2::Displacement, rdl2::RootShader>(m, "Displacement")
        DEF_DOWNCAST_CTOR(Displacement, "Displacement");

    py::class_<rdl2::VolumeShader, rdl2::RootShader>(m, "VolumeShader")
        DEF_DOWNCAST_CTOR(VolumeShader, "VolumeShader");

    py::class_<rdl2::Map, rdl2::Shader>(m, "Map")
        DEF_DOWNCAST_CTOR(Map, "Map");

    py::class_<rdl2::NormalMap, rdl2::Shader>(m, "NormalMap")
        DEF_DOWNCAST_CTOR(NormalMap, "NormalMap");
}

#undef DEF_DOWNCAST_CTOR
