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

    // -----------------------------------------------------------------------
    // UserData (inherits SceneObject)
    // Typed key/value channels for passing primitive attributes through rdl2.
    // Bool, Int, String: single-timestep only.
    // Float, Color, Vec2f, Vec3f, Mat4f: support dual-timestep (blur).
    // -----------------------------------------------------------------------
    py::class_<rdl2::UserData, rdl2::SceneObject> ud(m, "UserData");

    py::enum_<rdl2::UserData::Rate>(ud, "Rate")
        .value("AUTO",         rdl2::UserData::AUTO)
        .value("CONSTANT",     rdl2::UserData::CONSTANT)
        .value("PART",         rdl2::UserData::PART)
        .value("UNIFORM",      rdl2::UserData::UNIFORM)
        .value("VERTEX",       rdl2::UserData::VERTEX)
        .value("VARYING",      rdl2::UserData::VARYING)
        .value("FACE_VARYING", rdl2::UserData::FACE_VARYING)
        .export_values();

    ud
        .def("setRate", [](rdl2::UserData& self, rdl2::UserData::Rate rate) {
            self.setRate(static_cast<int>(rate));
        }, py::arg("rate"))
        .def("getRate", [](const rdl2::UserData& self) {
            return static_cast<rdl2::UserData::Rate>(self.getRate());
        })
        // Bool (single timestep)
        .def("hasBoolData",  &rdl2::UserData::hasBoolData)
        .def("setBoolData", [](rdl2::UserData& self, const std::string& key,
                               const rdl2::BoolVector& values) {
            self.setBoolData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("getBoolKey",    &rdl2::UserData::getBoolKey)
        .def("getBoolValues", &rdl2::UserData::getBoolValues)
        // Int (single timestep)
        .def("hasIntData",  &rdl2::UserData::hasIntData)
        .def("setIntData", [](rdl2::UserData& self, const std::string& key,
                              const rdl2::IntVector& values) {
            self.setIntData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("getIntKey",    &rdl2::UserData::getIntKey)
        .def("getIntValues", &rdl2::UserData::getIntValues)
        // Float (dual timestep)
        .def("hasFloatData",  &rdl2::UserData::hasFloatData)
        .def("hasFloatData0", &rdl2::UserData::hasFloatData0)
        .def("hasFloatData1", &rdl2::UserData::hasFloatData1)
        .def("setFloatData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::FloatVector& values) {
            self.setFloatData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setFloatData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::FloatVector& values0,
                                const rdl2::FloatVector& values1) {
            self.setFloatData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getFloatKey",    &rdl2::UserData::getFloatKey)
        .def("getFloatValues", &rdl2::UserData::getFloatValues)
        .def("getFloatValues0",&rdl2::UserData::getFloatValues0)
        .def("getFloatValues1",&rdl2::UserData::getFloatValues1)
        // String (single timestep)
        .def("hasStringData",  &rdl2::UserData::hasStringData)
        .def("setStringData", [](rdl2::UserData& self, const std::string& key,
                                 const rdl2::StringVector& values) {
            self.setStringData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("getStringKey",    &rdl2::UserData::getStringKey)
        .def("getStringValues", &rdl2::UserData::getStringValues)
        // Color / Rgb (dual timestep)
        .def("hasColorData",  &rdl2::UserData::hasColorData)
        .def("hasColorData0", &rdl2::UserData::hasColorData0)
        .def("hasColorData1", &rdl2::UserData::hasColorData1)
        .def("setColorData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::RgbVector& values) {
            self.setColorData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setColorData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::RgbVector& values0,
                                const rdl2::RgbVector& values1) {
            self.setColorData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getColorKey",    &rdl2::UserData::getColorKey)
        .def("getColorValues", &rdl2::UserData::getColorValues)
        .def("getColorValues0",&rdl2::UserData::getColorValues0)
        .def("getColorValues1",&rdl2::UserData::getColorValues1)
        // Vec2f (dual timestep)
        .def("hasVec2fData",  &rdl2::UserData::hasVec2fData)
        .def("hasVec2fData0", &rdl2::UserData::hasVec2fData0)
        .def("hasVec2fData1", &rdl2::UserData::hasVec2fData1)
        .def("setVec2fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec2fVector& values) {
            self.setVec2fData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setVec2fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec2fVector& values0,
                                const rdl2::Vec2fVector& values1) {
            self.setVec2fData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getVec2fKey",    &rdl2::UserData::getVec2fKey)
        .def("getVec2fValues", &rdl2::UserData::getVec2fValues)
        .def("getVec2fValues0",&rdl2::UserData::getVec2fValues0)
        .def("getVec2fValues1",&rdl2::UserData::getVec2fValues1)
        // Vec3f (dual timestep)
        .def("hasVec3fData",  &rdl2::UserData::hasVec3fData)
        .def("hasVec3fData0", &rdl2::UserData::hasVec3fData0)
        .def("hasVec3fData1", &rdl2::UserData::hasVec3fData1)
        .def("setVec3fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec3fVector& values) {
            self.setVec3fData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setVec3fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Vec3fVector& values0,
                                const rdl2::Vec3fVector& values1) {
            self.setVec3fData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getVec3fKey",    &rdl2::UserData::getVec3fKey)
        .def("getVec3fValues", &rdl2::UserData::getVec3fValues)
        .def("getVec3fValues0",&rdl2::UserData::getVec3fValues0)
        .def("getVec3fValues1",&rdl2::UserData::getVec3fValues1)
        // Mat4f (dual timestep)
        .def("hasMat4fData",  &rdl2::UserData::hasMat4fData)
        .def("hasMat4fData0", &rdl2::UserData::hasMat4fData0)
        .def("hasMat4fData1", &rdl2::UserData::hasMat4fData1)
        .def("setMat4fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Mat4fVector& values) {
            self.setMat4fData(key, values);
        }, py::arg("key"), py::arg("values"))
        .def("setMat4fData", [](rdl2::UserData& self, const std::string& key,
                                const rdl2::Mat4fVector& values0,
                                const rdl2::Mat4fVector& values1) {
            self.setMat4fData(key, values0, values1);
        }, py::arg("key"), py::arg("values0"), py::arg("values1"))
        .def("getMat4fKey",    &rdl2::UserData::getMat4fKey)
        .def("getMat4fValues", &rdl2::UserData::getMat4fValues)
        .def("getMat4fValues0",&rdl2::UserData::getMat4fValues0)
        .def("getMat4fValues1",&rdl2::UserData::getMat4fValues1);
}
