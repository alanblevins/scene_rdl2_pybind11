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
        }, py::return_value_policy::reference,
        "Returns a list of Geometry SceneObjects in this set.")
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
        }, py::return_value_policy::reference,
        "Returns a list of Light SceneObjects in this set.")
        .def("add",      &rdl2::LightSet::add,      py::arg("light"))
        .def("remove",   &rdl2::LightSet::remove,   py::arg("light"))
        .def("contains", &rdl2::LightSet::contains, py::arg("light"))
        .def("clear",    &rdl2::LightSet::clear);

    // -----------------------------------------------------------------------
    // LightFilter (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LightFilter, rdl2::SceneObject>(m, "LightFilter")
        .def("isOn", &rdl2::LightFilter::isOn);

    // -----------------------------------------------------------------------
    // LightFilterSet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::LightFilterSet, rdl2::SceneObject>(m, "LightFilterSet")
        .def("getLightFilters", [](const rdl2::LightFilterSet& self) {
            const rdl2::SceneObjectVector& v = self.getLightFilters();
            return std::vector<rdl2::SceneObject*>(v.begin(), v.end());
        }, py::return_value_policy::reference,
        "Returns a list of LightFilter SceneObjects in this set.")
        .def("add",      &rdl2::LightFilterSet::add,      py::arg("light_filter"))
        .def("remove",   &rdl2::LightFilterSet::remove,   py::arg("light_filter"))
        .def("contains", &rdl2::LightFilterSet::contains, py::arg("light_filter"))
        .def("clear",    &rdl2::LightFilterSet::clear);

    // -----------------------------------------------------------------------
    // ShadowSet (inherits LightSet)
    // -----------------------------------------------------------------------
    py::class_<rdl2::ShadowSet, rdl2::LightSet>(m, "ShadowSet")
        .def("haveLightsChanged", &rdl2::ShadowSet::haveLightsChanged);

    // -----------------------------------------------------------------------
    // ShadowReceiverSet (inherits GeometrySet)
    // -----------------------------------------------------------------------
    py::class_<rdl2::ShadowReceiverSet, rdl2::GeometrySet>(m, "ShadowReceiverSet")
        .def("haveGeometriesChanged", &rdl2::ShadowReceiverSet::haveGeometriesChanged);
}
