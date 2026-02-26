// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for Node, Camera, and Geometry (the Node sub-hierarchy).

#include "bindings.h"

// ---------------------------------------------------------------------------
// Macro: define a downcasting constructor for SceneObject-derived types.
//
// In nanobind, holder types do not exist.  The pybind11 pattern of returning
// a raw T* from py::init() is replaced by overriding __new__ to produce a
// non-owning reference (inst_reference: destruct=false, cpp_delete=false),
// then providing a no-op __init__ so Python does not complain about extra args.
// ---------------------------------------------------------------------------
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

void bind_node(py::module_& m)
{
    // -----------------------------------------------------------------------
    // Node (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Node, rdl2::SceneObject>(m, "Node")
        DEF_DOWNCAST_CTOR(Node, "Node")
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
    py::class_<rdl2::Camera, rdl2::Node>(m, "Camera")
        DEF_DOWNCAST_CTOR(Camera, "Camera")
        .def("getMediumMaterial", &rdl2::Camera::getMediumMaterial,
             py::rv_policy::reference)
        .def("getMediumGeometry", &rdl2::Camera::getMediumGeometry,
             py::rv_policy::reference)
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
    py::class_<rdl2::Geometry, rdl2::Node>(m, "Geometry")
        DEF_DOWNCAST_CTOR(Geometry, "Geometry")
        .def("isStatic",           &rdl2::Geometry::isStatic)
        .def("getSideType",        &rdl2::Geometry::getSideType)
        .def("getReverseNormals",  &rdl2::Geometry::getReverseNormals)
        .def("getRayEpsilon",      &rdl2::Geometry::getRayEpsilon)
        .def("getShadowRayEpsilon",&rdl2::Geometry::getShadowRayEpsilon)
        .def("getShadowReceiverLabel",    &rdl2::Geometry::getShadowReceiverLabel,
             py::rv_policy::reference)
        .def("getShadowExclusionMappings",&rdl2::Geometry::getShadowExclusionMappings,
             py::rv_policy::reference)
        .def("getVisibilityMask",  &rdl2::Geometry::getVisibilityMask);

    py::enum_<rdl2::Geometry::SideType>(m, "GeometrySideType")
        .value("TWO_SIDED",          rdl2::Geometry::TWO_SIDED)
        .value("SINGLE_SIDED",       rdl2::Geometry::SINGLE_SIDED)
        .value("MESH_DEFAULT_SIDED", rdl2::Geometry::MESH_DEFAULT_SIDED)
        .export_values();

    // -----------------------------------------------------------------------
    // EnvMap (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::EnvMap, rdl2::Node>(m, "EnvMap")
        DEF_DOWNCAST_CTOR(EnvMap, "EnvMap");

    // -----------------------------------------------------------------------
    // Joint (inherits Node)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Joint, rdl2::Node>(m, "Joint")
        DEF_DOWNCAST_CTOR(Joint, "Joint");
}

#undef DEF_DOWNCAST_CTOR
