// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for SceneVariables.

#include "bindings.h"

void bind_scene_variables(py::module_& m)
{
    py::class_<rdl2::SceneVariables, rdl2::SceneObject,
               std::unique_ptr<rdl2::SceneVariables, py::nodelete>>(m, "SceneVariables")
        .def("getRezedWidth",  &rdl2::SceneVariables::getRezedWidth)
        .def("getRezedHeight", &rdl2::SceneVariables::getRezedHeight)
        .def("getMachineId",   &rdl2::SceneVariables::getMachineId)
        .def("getNumMachines", &rdl2::SceneVariables::getNumMachines)
        .def("getLayer",  &rdl2::SceneVariables::getLayer,
             py::return_value_policy::reference)
        .def("getCamera", &rdl2::SceneVariables::getCamera,
             py::return_value_policy::reference)
        .def("getExrHeaderAttributes", &rdl2::SceneVariables::getExrHeaderAttributes,
             py::return_value_policy::reference)
        .def("getTmpDir", &rdl2::SceneVariables::getTmpDir);
}
