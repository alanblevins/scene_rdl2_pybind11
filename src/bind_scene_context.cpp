// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for SceneContext.

#include "bindings.h"

static std::vector<rdl2::SceneObject*> getAllSceneObjects(rdl2::SceneContext& ctx)
{
    std::vector<rdl2::SceneObject*> result;
    for (auto it = ctx.beginSceneObject(); it != ctx.endSceneObject(); ++it)
        result.push_back(it->second);
    return result;
}

static std::vector<const rdl2::SceneClass*> getAllSceneClasses(const rdl2::SceneContext& ctx)
{
    std::vector<const rdl2::SceneClass*> result;
    for (auto it = ctx.beginSceneClass(); it != ctx.endSceneClass(); ++it)
        result.push_back(it->second);
    return result;
}

void bind_scene_context(py::module_& m)
{
    // py::nodelete prevents pybind11 from calling ~SceneContext(), which aborts
    // outside the full MoonRay pipeline. Memory is reclaimed by the OS on exit.
    py::class_<rdl2::SceneContext, std::unique_ptr<rdl2::SceneContext, py::nodelete>>(m, "SceneContext")
        .def(py::init<>())
        // DSO path
        .def("getDsoPath",  &rdl2::SceneContext::getDsoPath)
        .def("setDsoPath",  &rdl2::SceneContext::setDsoPath)
        // Proxy mode
        .def("getProxyModeEnabled", &rdl2::SceneContext::getProxyModeEnabled)
        .def("setProxyModeEnabled", &rdl2::SceneContext::setProxyModeEnabled)
        // Scene variables
        .def("getSceneVariables",
             (const rdl2::SceneVariables& (rdl2::SceneContext::*)() const)
             &rdl2::SceneContext::getSceneVariables,
             py::return_value_policy::reference)
        .def("getSceneVariables",
             (rdl2::SceneVariables& (rdl2::SceneContext::*)())
             &rdl2::SceneContext::getSceneVariables,
             py::return_value_policy::reference)
        // Scene classes
        .def("getSceneClass",
             (const rdl2::SceneClass* (rdl2::SceneContext::*)(const std::string&) const)
             &rdl2::SceneContext::getSceneClass,
             py::return_value_policy::reference)
        .def("sceneClassExists",  &rdl2::SceneContext::sceneClassExists)
        .def("createSceneClass",  &rdl2::SceneContext::createSceneClass,
             py::return_value_policy::reference)
        .def("getAllSceneClasses", &getAllSceneClasses,
             py::return_value_policy::reference,
             "Returns a list of all SceneClass objects in the context.")
        // Scene objects
        .def("getSceneObject",
             (const rdl2::SceneObject* (rdl2::SceneContext::*)(const std::string&) const)
             &rdl2::SceneContext::getSceneObject,
             py::return_value_policy::reference)
        .def("getSceneObject",
             (rdl2::SceneObject* (rdl2::SceneContext::*)(const std::string&))
             &rdl2::SceneContext::getSceneObject,
             py::return_value_policy::reference)
        .def("sceneObjectExists", &rdl2::SceneContext::sceneObjectExists)
        .def("createSceneObject", &rdl2::SceneContext::createSceneObject,
             py::return_value_policy::reference)
        .def("getAllSceneObjects", &getAllSceneObjects,
             py::return_value_policy::reference,
             "Returns a list of all SceneObject instances in the context.")
        // Cameras
        .def("getPrimaryCamera", &rdl2::SceneContext::getPrimaryCamera,
             py::return_value_policy::reference)
        .def("getCameras",       &rdl2::SceneContext::getCameras,
             py::return_value_policy::reference)
        .def("getActiveCameras", &rdl2::SceneContext::getActiveCameras,
             py::return_value_policy::reference)
        .def("getDicingCamera",  &rdl2::SceneContext::getDicingCamera,
             py::return_value_policy::reference)
        // Transforms
        .def("getRender2World", &rdl2::SceneContext::getRender2World,
             py::return_value_policy::reference)
        .def("setRender2World", &rdl2::SceneContext::setRender2World)
        // Checkpoint / resume
        .def("getCheckpointActive",  &rdl2::SceneContext::getCheckpointActive)
        .def("getResumableOutput",   &rdl2::SceneContext::getResumableOutput)
        .def("getResumeRender",      &rdl2::SceneContext::getResumeRender)
        // Commit / load
        .def("commitAllChanges",    &rdl2::SceneContext::commitAllChanges)
        .def("loadAllSceneClasses", &rdl2::SceneContext::loadAllSceneClasses)
        // DSO counts
        .def("getDsoCounts", &rdl2::SceneContext::getDsoCounts);
}
