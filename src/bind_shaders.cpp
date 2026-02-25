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
    // inherit from SceneObject; the bindings exist for type identification,
    // safe downcasting, and constructor-based casting from SceneObject.

    py::class_<rdl2::Shader, rdl2::SceneObject,
               std::unique_ptr<rdl2::Shader, py::nodelete>>(m, "Shader")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Shader* {
            auto* r = obj->asA<rdl2::Shader>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Shader");
            return r;
        }), py::arg("scene_object"));

    py::class_<rdl2::RootShader, rdl2::Shader,
               std::unique_ptr<rdl2::RootShader, py::nodelete>>(m, "RootShader")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::RootShader* {
            auto* r = obj->asA<rdl2::RootShader>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to RootShader");
            return r;
        }), py::arg("scene_object"));

    py::class_<rdl2::Material, rdl2::RootShader,
               std::unique_ptr<rdl2::Material, py::nodelete>>(m, "Material")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Material* {
            auto* r = obj->asA<rdl2::Material>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Material");
            return r;
        }), py::arg("scene_object"));

    py::class_<rdl2::Displacement, rdl2::RootShader,
               std::unique_ptr<rdl2::Displacement, py::nodelete>>(m, "Displacement")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Displacement* {
            auto* r = obj->asA<rdl2::Displacement>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Displacement");
            return r;
        }), py::arg("scene_object"));

    py::class_<rdl2::VolumeShader, rdl2::RootShader,
               std::unique_ptr<rdl2::VolumeShader, py::nodelete>>(m, "VolumeShader")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::VolumeShader* {
            auto* r = obj->asA<rdl2::VolumeShader>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to VolumeShader");
            return r;
        }), py::arg("scene_object"));

    py::class_<rdl2::Map, rdl2::Shader,
               std::unique_ptr<rdl2::Map, py::nodelete>>(m, "Map")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Map* {
            auto* r = obj->asA<rdl2::Map>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Map");
            return r;
        }), py::arg("scene_object"));

    py::class_<rdl2::NormalMap, rdl2::Shader,
               std::unique_ptr<rdl2::NormalMap, py::nodelete>>(m, "NormalMap")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::NormalMap* {
            auto* r = obj->asA<rdl2::NormalMap>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to NormalMap");
            return r;
        }), py::arg("scene_object"));
}
