// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <scene_rdl2/scene/rdl2/SceneContext.h>
#include <scene_rdl2/scene/rdl2/SceneObject.h>
#include <scene_rdl2/scene/rdl2/SceneClass.h>

namespace py = pybind11;
namespace rdl2 = scene_rdl2::rdl2;

// Forward declarations
class PySceneContext;
class PySceneObject;
class PySceneClass;

void init_scene_context(py::module_& m);
void init_scene_object(py::module_& m);
void init_scene_class(py::module_& m);