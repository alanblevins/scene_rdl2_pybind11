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

    // -----------------------------------------------------------------------
    // DisplayFilter (inherits SceneObject)
    // getInputData / filterv use moonray-internal types and are not exposed.
    // -----------------------------------------------------------------------
    py::class_<rdl2::DisplayFilter, rdl2::SceneObject>(m, "DisplayFilter");

    // -----------------------------------------------------------------------
    // Metadata (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::Metadata, rdl2::SceneObject>(m, "Metadata")
        .def("setAttributes", [](rdl2::Metadata& self,
                                 const std::vector<std::string>& names,
                                 const std::vector<std::string>& types,
                                 const std::vector<std::string>& values) {
            rdl2::StringVector n(names), t(types), v(values);
            self.setAttributes(n, t, v);
        }, py::arg("names"), py::arg("types"), py::arg("values"),
        "Set EXR header metadata entries as parallel name/type/value lists.")
        .def("getAttributeNames",  [](const rdl2::Metadata& self) {
            return std::vector<std::string>(self.getAttributeNames().begin(),
                                           self.getAttributeNames().end());
        })
        .def("getAttributeTypes",  [](const rdl2::Metadata& self) {
            return std::vector<std::string>(self.getAttributeTypes().begin(),
                                           self.getAttributeTypes().end());
        })
        .def("getAttributeValues", [](const rdl2::Metadata& self) {
            return std::vector<std::string>(self.getAttributeValues().begin(),
                                           self.getAttributeValues().end());
        });

    // -----------------------------------------------------------------------
    // TraceSet (inherits SceneObject)
    // -----------------------------------------------------------------------
    py::class_<rdl2::TraceSet, rdl2::SceneObject>(m, "TraceSet")
        .def("getAssignmentCount", &rdl2::TraceSet::getAssignmentCount,
             "Returns the number of Geometry/Part assignments in this TraceSet.")
        .def("assign", &rdl2::TraceSet::assign,
             py::arg("geometry"), py::arg("part_name"),
             "Add a Geometry/Part pair and return its assignment ID.")
        .def("lookupGeomAndPart", [](const rdl2::TraceSet& self, int32_t assignmentId) {
            auto pair = self.lookupGeomAndPart(assignmentId);
            return py::make_tuple(
                py::cast(pair.first, py::return_value_policy::reference),
                std::string(pair.second));
        }, py::arg("assignment_id"),
        "Return (Geometry, part_name) for a given assignment ID.")
        .def("getAssignmentId", &rdl2::TraceSet::getAssignmentId,
             py::arg("geometry"), py::arg("part_name"),
             "Return the assignment ID for a Geometry/Part pair, or -1 if not found.")
        .def("contains", &rdl2::TraceSet::contains,
             py::arg("geometry"),
             "Return True if the given Geometry appears in this TraceSet.")
        .def("getAssignmentIds", [](const rdl2::TraceSet& self,
                                    const rdl2::Geometry* geometry) {
            std::vector<int32_t> ids;
            for (auto it = self.begin(geometry); it != self.end(geometry); ++it)
                ids.push_back(*it);
            return ids;
        }, py::arg("geometry"),
        "Return a list of all assignment IDs for the given Geometry.");
}
