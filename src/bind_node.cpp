// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for Node, Camera, and Geometry (the Node sub-hierarchy).

#include "bindings.h"

void bind_node(py::module_& m)
{
    // -----------------------------------------------------------------------
    // Node (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Node, rdl2::SceneObject,
               std::unique_ptr<rdl2::Node, py::nodelete>>(m, "Node")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Node* {
            auto* r = obj->asA<rdl2::Node>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Node");
            return r;
        }), py::arg("scene_object"))
        .def("getNodeXform", [](const rdl2::Node& self) {
            return self.get(rdl2::Node::sNodeXformKey);
        }, "Returns the node transform matrix (Mat4d).")
        .def("setNodeXform", [](rdl2::Node& self, const rdl2::Mat4d& xform) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.set(rdl2::Node::sNodeXformKey, xform);
        }, py::arg("xform"), "Sets the node transform matrix.");

    // -----------------------------------------------------------------------
    // Camera (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Camera, rdl2::Node,
               std::unique_ptr<rdl2::Camera, py::nodelete>>(m, "Camera")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Camera* {
            auto* r = obj->asA<rdl2::Camera>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Camera");
            return r;
        }), py::arg("scene_object"))
        .def("getMediumMaterial", &rdl2::Camera::getMediumMaterial,
             py::return_value_policy::reference)
        .def("getMediumGeometry", &rdl2::Camera::getMediumGeometry,
             py::return_value_policy::reference)
        .def("getNear", [](const rdl2::Camera& self) {
            return self.get(rdl2::Camera::sNearKey);
        })
        .def("getFar",  [](const rdl2::Camera& self) {
            return self.get(rdl2::Camera::sFarKey);
        })
        .def("setNear", [](rdl2::Camera& self, float near) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setNear(near);
        }, py::arg("near"))
        .def("setFar", [](rdl2::Camera& self, float far) {
            rdl2::SceneObject::UpdateGuard guard(&self);
            self.setFar(far);
        }, py::arg("far"));

    // -----------------------------------------------------------------------
    // Geometry (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Geometry, rdl2::Node,
               std::unique_ptr<rdl2::Geometry, py::nodelete>>(m, "Geometry")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Geometry* {
            auto* r = obj->asA<rdl2::Geometry>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Geometry");
            return r;
        }), py::arg("scene_object"))
        .def("isStatic",           &rdl2::Geometry::isStatic)
        .def("getSideType",        &rdl2::Geometry::getSideType)
        .def("getReverseNormals",  &rdl2::Geometry::getReverseNormals)
        .def("getRayEpsilon",      &rdl2::Geometry::getRayEpsilon)
        .def("getShadowRayEpsilon",&rdl2::Geometry::getShadowRayEpsilon)
        .def("getShadowReceiverLabel",    &rdl2::Geometry::getShadowReceiverLabel,
             py::return_value_policy::reference)
        .def("getShadowExclusionMappings",&rdl2::Geometry::getShadowExclusionMappings,
             py::return_value_policy::reference)
        .def("getVisibilityMask",  &rdl2::Geometry::getVisibilityMask);

    py::enum_<rdl2::Geometry::SideType>(m, "GeometrySideType")
        .value("TWO_SIDED",          rdl2::Geometry::TWO_SIDED)
        .value("SINGLE_SIDED",       rdl2::Geometry::SINGLE_SIDED)
        .value("MESH_DEFAULT_SIDED", rdl2::Geometry::MESH_DEFAULT_SIDED)
        .export_values();

    // -----------------------------------------------------------------------
    // EnvMap (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::EnvMap, rdl2::Node,
               std::unique_ptr<rdl2::EnvMap, py::nodelete>>(m, "EnvMap")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::EnvMap* {
            auto* r = obj->asA<rdl2::EnvMap>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to EnvMap");
            return r;
        }), py::arg("scene_object"));

    // -----------------------------------------------------------------------
    // Joint (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Joint, rdl2::Node,
               std::unique_ptr<rdl2::Joint, py::nodelete>>(m, "Joint")
        .def(py::init([](rdl2::SceneObject* obj) -> rdl2::Joint* {
            auto* r = obj->asA<rdl2::Joint>();
            if (!r) throw py::type_error(
                "cannot cast '" + obj->getSceneClass().getName() + "' to Joint");
            return r;
        }), py::arg("scene_object"));
}
