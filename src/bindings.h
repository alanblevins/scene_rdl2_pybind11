// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Shared header for all scene_rdl2 Python binding translation units.
// Included by module.cpp and every bind_*.cpp file.

#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/map.h>

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
#include <scene_rdl2/scene/rdl2/BinaryReader.h>
#include <scene_rdl2/scene/rdl2/BinaryWriter.h>
#include <scene_rdl2/scene/rdl2/DisplayFilter.h>
#include <scene_rdl2/scene/rdl2/EnvMap.h>
#include <scene_rdl2/scene/rdl2/Joint.h>
#include <scene_rdl2/scene/rdl2/Metadata.h>
#include <scene_rdl2/scene/rdl2/TraceSet.h>

namespace py  = nanobind;
namespace rdl2 = scene_rdl2::rdl2;

// ---------------------------------------------------------------------------
// std::is_move_constructible specialisations
//
// SceneObject has a user-declared (private, undefined) copy constructor which
// suppresses the implicit move constructor.  Compiler-generated move ctors for
// derived types (e.g. TraceSet::TraceSet(TraceSet&&)) therefore fall back to
// SceneObject's copy ctor, whose symbol is not exported from libscene_rdl2.dylib.
// Specialising std::is_move_constructible to false for all rdl2 types prevents
// nanobind from emitting move handlers that would instantiate that chain.
// (Technically a specialisation in namespace std for a user type is undefined
// behaviour per the standard, but all major compilers accept it and it has the
// desired effect here.)
// ---------------------------------------------------------------------------
#define NB_MARK_NON_MOVABLE(T)                                              \
    namespace std {                                                          \
        template <> struct is_move_constructible<T> : false_type {};        \
    }

NB_MARK_NON_MOVABLE(rdl2::SceneObject)
NB_MARK_NON_MOVABLE(rdl2::SceneVariables)
NB_MARK_NON_MOVABLE(rdl2::SceneContext)
NB_MARK_NON_MOVABLE(rdl2::SceneClass)
NB_MARK_NON_MOVABLE(rdl2::Attribute)
NB_MARK_NON_MOVABLE(rdl2::Node)
NB_MARK_NON_MOVABLE(rdl2::Camera)
NB_MARK_NON_MOVABLE(rdl2::Geometry)
NB_MARK_NON_MOVABLE(rdl2::EnvMap)
NB_MARK_NON_MOVABLE(rdl2::Joint)
NB_MARK_NON_MOVABLE(rdl2::Light)
NB_MARK_NON_MOVABLE(rdl2::Shader)
NB_MARK_NON_MOVABLE(rdl2::RootShader)
NB_MARK_NON_MOVABLE(rdl2::Material)
NB_MARK_NON_MOVABLE(rdl2::Displacement)
NB_MARK_NON_MOVABLE(rdl2::VolumeShader)
NB_MARK_NON_MOVABLE(rdl2::Map)
NB_MARK_NON_MOVABLE(rdl2::NormalMap)
NB_MARK_NON_MOVABLE(rdl2::GeometrySet)
NB_MARK_NON_MOVABLE(rdl2::LightSet)
NB_MARK_NON_MOVABLE(rdl2::LightFilter)
NB_MARK_NON_MOVABLE(rdl2::LightFilterSet)
NB_MARK_NON_MOVABLE(rdl2::ShadowSet)
NB_MARK_NON_MOVABLE(rdl2::ShadowReceiverSet)
NB_MARK_NON_MOVABLE(rdl2::DisplayFilter)
NB_MARK_NON_MOVABLE(rdl2::Metadata)
NB_MARK_NON_MOVABLE(rdl2::TraceSet)
NB_MARK_NON_MOVABLE(rdl2::UserData)
NB_MARK_NON_MOVABLE(rdl2::Layer)
NB_MARK_NON_MOVABLE(rdl2::RenderOutput)

#undef NB_MARK_NON_MOVABLE

// ---------------------------------------------------------------------------
// nanobind::detail::is_copy_constructible specialisations
//
// These types have copy constructors in the C++ class definitions but the
// symbols are not exported from libscene_rdl2.  Specialising nanobind's
// trait (analogous to pybind11's MARK_NON_COPYABLE) prevents nanobind from
// generating a copy handler that would reference the unexported symbol.
// ---------------------------------------------------------------------------
#define NB_MARK_NON_COPYABLE(T)                                         \
    namespace nanobind { namespace detail {                             \
        template <> struct is_copy_constructible<T> : std::false_type {}; \
    }}

NB_MARK_NON_COPYABLE(rdl2::SceneObject)
NB_MARK_NON_COPYABLE(rdl2::SceneVariables)
NB_MARK_NON_COPYABLE(rdl2::SceneContext)
NB_MARK_NON_COPYABLE(rdl2::SceneClass)
NB_MARK_NON_COPYABLE(rdl2::Attribute)
NB_MARK_NON_COPYABLE(rdl2::Node)
NB_MARK_NON_COPYABLE(rdl2::Camera)
NB_MARK_NON_COPYABLE(rdl2::Geometry)
NB_MARK_NON_COPYABLE(rdl2::EnvMap)
NB_MARK_NON_COPYABLE(rdl2::Joint)
NB_MARK_NON_COPYABLE(rdl2::Light)
NB_MARK_NON_COPYABLE(rdl2::Shader)
NB_MARK_NON_COPYABLE(rdl2::RootShader)
NB_MARK_NON_COPYABLE(rdl2::Material)
NB_MARK_NON_COPYABLE(rdl2::Displacement)
NB_MARK_NON_COPYABLE(rdl2::VolumeShader)
NB_MARK_NON_COPYABLE(rdl2::Map)
NB_MARK_NON_COPYABLE(rdl2::NormalMap)
NB_MARK_NON_COPYABLE(rdl2::GeometrySet)
NB_MARK_NON_COPYABLE(rdl2::LightSet)
NB_MARK_NON_COPYABLE(rdl2::LightFilter)
NB_MARK_NON_COPYABLE(rdl2::LightFilterSet)
NB_MARK_NON_COPYABLE(rdl2::ShadowSet)
NB_MARK_NON_COPYABLE(rdl2::ShadowReceiverSet)
NB_MARK_NON_COPYABLE(rdl2::DisplayFilter)
NB_MARK_NON_COPYABLE(rdl2::Metadata)
NB_MARK_NON_COPYABLE(rdl2::TraceSet)
NB_MARK_NON_COPYABLE(rdl2::UserData)
NB_MARK_NON_COPYABLE(rdl2::Layer)
NB_MARK_NON_COPYABLE(rdl2::RenderOutput)

#undef NB_MARK_NON_COPYABLE

// ---------------------------------------------------------------------------
// Per-class binding functions — implemented in bind_*.cpp, called from
// NB_MODULE in module.cpp.  Must be called in the order listed so that
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
