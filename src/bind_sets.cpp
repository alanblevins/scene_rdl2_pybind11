// Copyright (c) 2026 Alan Blevins
// SPDX-License-Identifier: MIT
//
// Python bindings for scene object collection types:
//   GeometrySet, LightSet

#include "bindings.h"

void bind_sets(py::module_& m)
{
    // -----------------------------------------------------------------------
    // GeometrySet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::GeometrySet, rdl2::SceneObject>(m, "GeometrySet")
        .def("getGeometries", [](const rdl2::GeometrySet& self) {
            const rdl2::SceneObjectIndexable& idx = self.getGeometries();
            return std::vector<rdl2::SceneObject*>(idx.begin(), idx.end());
        }, "Returns a list of Geometry SceneObjects in this set.")
        .def("add",      &rdl2::GeometrySet::add,      py::arg("geometry"))
        .def("remove",   &rdl2::GeometrySet::remove,   py::arg("geometry"))
        .def("contains", &rdl2::GeometrySet::contains, py::arg("geometry"))
        .def("clear",    &rdl2::GeometrySet::clear)
        .def("isStatic", &rdl2::GeometrySet::isStatic)
        .def("haveGeometriesChanged", &rdl2::GeometrySet::haveGeometriesChanged);

    // -----------------------------------------------------------------------
    // LightSet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LightSet, rdl2::SceneObject>(m, "LightSet")
        .def("getLights", [](const rdl2::LightSet& self) {
            return self.getLights();
        }, "Returns a list of Light SceneObjects in this set.")
        .def("add",      &rdl2::LightSet::add,      py::arg("light"))
        .def("remove",   &rdl2::LightSet::remove,   py::arg("light"))
        .def("contains", &rdl2::LightSet::contains, py::arg("light"))
        .def("clear",    &rdl2::LightSet::clear);
}
