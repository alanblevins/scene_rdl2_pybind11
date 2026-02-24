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
    py::class_<rdl2::Node, rdl2::SceneObject>(m, "Node")
        .def("getNodeXform", [](const rdl2::Node& self) {
            return self.get(rdl2::Node::sNodeXformKey);
        }, "Returns the node transform matrix (Mat4d).")
        .def("setNodeXform", [](rdl2::Node& self, const rdl2::Mat4d& xform) {
            self.set(rdl2::Node::sNodeXformKey, xform);
        }, py::arg("xform"), "Sets the node transform matrix.");

    // -----------------------------------------------------------------------
    // Camera (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Camera, rdl2::Node>(m, "Camera")
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
        .def("setNear", &rdl2::Camera::setNear, py::arg("near"))
        .def("setFar",  &rdl2::Camera::setFar,  py::arg("far"));

    // -----------------------------------------------------------------------
    // Geometry (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Geometry, rdl2::Node>(m, "Geometry")
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
    py::class_<rdl2::EnvMap, rdl2::Node>(m, "EnvMap");

    // -----------------------------------------------------------------------
    // Joint (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Joint, rdl2::Node>(m, "Joint");
}
