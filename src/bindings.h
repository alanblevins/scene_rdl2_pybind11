// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Shared header for all scene_rdl2 Python binding translation units.
// Included by module.cpp and every bind_*.cpp file.

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

// scene_rdl2 headers — order matters for forward declarations
#include <scene_rdl2/scene/rdl2/Types.h>
#include <scene_rdl2/scene/rdl2/Attribute.h>
#include <scene_rdl2/scene/rdl2/AttributeKey.h>
#include <scene_rdl2/scene/rdl2/SceneClass.h>
#include <scene_rdl2/scene/rdl2/SceneObject.h>
#include <scene_rdl2/scene/rdl2/SceneContext.h>
#include <scene_rdl2/scene/rdl2/SceneVariables.h>
#include <scene_rdl2/scene/rdl2/Node.h>
#include <scene_rdl2/scene/rdl2/Camera.h>
#include <scene_rdl2/scene/rdl2/Geometry.h>
#include <scene_rdl2/scene/rdl2/GeometrySet.h>
#include <scene_rdl2/scene/rdl2/Light.h>
#include <scene_rdl2/scene/rdl2/LightSet.h>
#include <scene_rdl2/scene/rdl2/LightFilter.h>
#include <scene_rdl2/scene/rdl2/LightFilterSet.h>
#include <scene_rdl2/scene/rdl2/ShadowSet.h>
#include <scene_rdl2/scene/rdl2/ShadowReceiverSet.h>
#include <scene_rdl2/scene/rdl2/Layer.h>
#include <scene_rdl2/scene/rdl2/Material.h>
#include <scene_rdl2/scene/rdl2/Displacement.h>
#include <scene_rdl2/scene/rdl2/VolumeShader.h>
#include <scene_rdl2/scene/rdl2/Map.h>
#include <scene_rdl2/scene/rdl2/NormalMap.h>
#include <scene_rdl2/scene/rdl2/RootShader.h>
#include <scene_rdl2/scene/rdl2/Shader.h>
#include <scene_rdl2/scene/rdl2/RenderOutput.h>
#include <scene_rdl2/scene/rdl2/UserData.h>
#include <scene_rdl2/scene/rdl2/AsciiReader.h>
#include <scene_rdl2/scene/rdl2/AsciiWriter.h>

// SceneObject and all its subclasses declare their copy constructors as
// private and leave them undefined (the C++03 non-copyable idiom, without
// = delete).  On some compilers/libc++ versions __is_constructible() returns
// true for these types anyway (it doesn't check access), which causes
// pybind11 to emit copy/move constructor wrappers that reference undefined
// symbols.  Specialise pybind11::detail traits here so no such code is
// generated in any translation unit that includes this header.
namespace pybind11 { namespace detail {
#define MARK_NON_COPYABLE(T) \
    template <> struct is_copy_constructible<T> : std::false_type {}; \
    template <> struct is_copy_assignable<T>     : std::false_type {}; \
    template <> struct is_move_constructible<T>  : std::false_type {};
MARK_NON_COPYABLE(scene_rdl2::rdl2::SceneObject)
MARK_NON_COPYABLE(scene_rdl2::rdl2::SceneVariables)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Node)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Camera)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Geometry)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Light)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Shader)
MARK_NON_COPYABLE(scene_rdl2::rdl2::RootShader)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Material)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Displacement)
MARK_NON_COPYABLE(scene_rdl2::rdl2::VolumeShader)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Map)
MARK_NON_COPYABLE(scene_rdl2::rdl2::NormalMap)
MARK_NON_COPYABLE(scene_rdl2::rdl2::GeometrySet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::LightSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::LightFilter)
MARK_NON_COPYABLE(scene_rdl2::rdl2::LightFilterSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::ShadowSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::ShadowReceiverSet)
MARK_NON_COPYABLE(scene_rdl2::rdl2::Layer)
MARK_NON_COPYABLE(scene_rdl2::rdl2::RenderOutput)
#undef MARK_NON_COPYABLE
}} // namespace pybind11::detail

namespace py  = pybind11;
namespace rdl2 = scene_rdl2::rdl2;

// ---------------------------------------------------------------------------
// Per-class binding functions — implemented in bind_*.cpp, called from
// PYBIND11_MODULE in module.cpp.  Must be called in the order listed so that
// base classes are registered before their derived classes.
// ---------------------------------------------------------------------------
void bind_math(py::module_& m);
void bind_types(py::module_& m);
void bind_attribute(py::module_& m);
void bind_scene_object(py::module_& m);
void bind_scene_variables(py::module_& m);
void bind_node(py::module_& m);
void bind_light(py::module_& m);
void bind_shaders(py::module_& m);
void bind_sets(py::module_& m);
void bind_layer(py::module_& m);
void bind_render_output(py::module_& m);
void bind_scene_context(py::module_& m);
void bind_io(py::module_& m);
